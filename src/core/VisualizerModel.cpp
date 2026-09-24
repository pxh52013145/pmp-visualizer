#include "core/VisualizerModel.hpp"

#include <algorithm>
#include <cmath>

namespace pmp::core {

namespace {
constexpr double kPi = 3.14159265358979323846;

float clamp01(float value) {
    return std::clamp(value, 0.0F, 1.0F);
}

float averageRange(const std::array<float, 64>& bins, int begin, int end) {
    begin = std::clamp(begin, 0, 64);
    end = std::clamp(end, begin + 1, 64);
    float total = 0.0F;
    for (int index = begin; index < end; ++index) total += bins[static_cast<size_t>(index)];
    return total / static_cast<float>(end - begin);
}
} // namespace

AudioAnalysis AudioAnalysisEngine::process(const std::array<float, 64>& bins, double timestampSeconds) {
    const float energy = averageRange(bins, 0, 64);
    const float attack = 0.72F;
    const float release = 0.24F;
    const float alpha = energy > state_.energy ? attack : release;
    const float previousEnergy = state_.energy;
    state_.energy = energy;
    state_.smoothedEnergy += (energy - state_.smoothedEnergy) * alpha;
    state_.energyDelta = energy - previousEnergy;
    state_.bass = averageRange(bins, 0, 12);
    state_.mid = averageRange(bins, 12, 36);
    state_.treble = averageRange(bins, 36, 64);

    float weighted = 0.0F;
    float total = 0.0F;
    state_.peakValue = 0.0F;
    state_.peakBin = 0;
    for (int index = 0; index < 64; ++index) {
        const float value = clamp01(bins[static_cast<size_t>(index)]);
        weighted += value * static_cast<float>(index);
        total += value;
        if (value > state_.peakValue) {
            state_.peakValue = value;
            state_.peakBin = index;
        }
    }
    state_.spectralCentroid = total > 0.0001F ? clamp01(weighted / total / 63.0F) : 0.0F;
    const double deltaSeconds = std::max(0.001, timestampSeconds - previousTimestampSeconds_);
    state_.beatStrength = clamp01(std::max(0.0F, state_.energyDelta) * 12.0F +
                                  state_.smoothedEnergy * 0.25F);
    state_.beatStrength *= static_cast<float>(std::min(1.0, deltaSeconds * 60.0));
    state_.spectrum = bins;
    previousTimestampSeconds_ = timestampSeconds;
    return state_;
}

AudioAnalysis AudioAnalysisEngine::generateDemo(double timestampSeconds) {
    std::array<float, 64> bins{};
    for (int index = 0; index < 64; ++index) {
        const double position = static_cast<double>(index) / 63.0;
        const double lowPulse = 0.5 + 0.5 * std::sin(timestampSeconds * 3.2 + position * 6.0);
        const double harmonic = 0.5 + 0.5 * std::sin(timestampSeconds * 1.3 + position * 19.0);
        const double rolloff = 1.0 - position * 0.48;
        bins[static_cast<size_t>(index)] = static_cast<float>(
            std::clamp((0.08 + lowPulse * 0.58 + harmonic * 0.26) * rolloff, 0.0, 1.0));
    }
    return process(bins, timestampSeconds);
}

bool FrameGovernor::beginFrame(double timestampMs) {
    constexpr double targetIntervalMs = 1000.0 / 60.0;
    if (!invalidated_ && lastRenderMs_ >= 0.0 && timestampMs - lastRenderMs_ < targetIntervalMs) return false;
    invalidated_ = false;
    lastRenderMs_ = timestampMs;
    return true;
}

void FrameGovernor::endFrame(double timestampMs, double renderDurationMs) {
    lastFrameMs_ = std::max(0.0, renderDurationMs);
    if (evaluationStartMs_ < 0.0) evaluationStartMs_ = timestampMs;
    sampleDurationMs_ += lastFrameMs_;
    ++sampleFrames_;
    const double elapsed = timestampMs - evaluationStartMs_;
    if (elapsed < 1000.0 || sampleFrames_ == 0) return;

    actualFps_ = static_cast<double>(sampleFrames_) * 1000.0 / elapsed;
    const double averageMs = sampleDurationMs_ / static_cast<double>(sampleFrames_);
    if (averageMs > 14.0) renderScale_ = std::max(0.55, renderScale_ - 0.05);
    if (averageMs < 9.0) renderScale_ = std::min(1.0, renderScale_ + 0.05);
    evaluationStartMs_ = timestampMs;
    sampleDurationMs_ = 0.0;
    sampleFrames_ = 0;
}

WorkbenchModel::WorkbenchModel() {
    initializeScene();
}

void WorkbenchModel::initializeScene() {
    snapshot_.components = {
        {"phase", "Phase Scope", "analysis", {-0.02F, 0.0F, 0.0F, 1.0F, 0.0F, 1.0F}, true, true, false, 1.0F},
        {"freq", "Frequency Ring", "spectrum", {0.0F, 0.0F, 0.1F, 1.0F, 0.0F, 1.0F}, true, true, false, 1.15F},
        {"chords", "Chord Wheel", "spectrum", {0.0F, 0.0F, 0.2F, 1.0F, 0.0F, 1.0F}, true, true, false, 0.8F},
        {"particles", "Particle Flow", "particles", {0.0F, 0.0F, 0.3F, 1.0F, 0.0F, 1.0F}, true, true, false, 0.95F},
        {"morse", "Morse Telemetry", "telemetry", {0.0F, 0.0F, 0.4F, 1.0F, 0.0F, 1.0F}, true, true, false, 0.7F},
        {"center", "Center Console", "overlay", {0.0F, 0.0F, 0.5F, 1.0F, 0.0F, 1.0F}, true, true, true, 0.85F},
        {"hud", "Track Header", "overlay", {-0.72F, -0.76F, 0.6F, 1.0F, 0.0F, 1.0F}, true, true, false, 0.5F},
    };
    snapshot_.selectedComponentId = "center";
}

void WorkbenchModel::advance(double deltaMs) {
    elapsedMs_ += std::max(0.0, deltaMs);
    if (!frameGovernor_.beginFrame(elapsedMs_)) return;
    if (snapshot_.timeline.playing) {
        snapshot_.timeline.playheadMs += deltaMs;
        if (snapshot_.timeline.playheadMs > snapshot_.timeline.durationMs) {
            snapshot_.timeline.playheadMs = snapshot_.timeline.loopEnabled
                ? snapshot_.timeline.loopStartMs
                : snapshot_.timeline.durationMs;
            if (!snapshot_.timeline.loopEnabled) snapshot_.timeline.playing = false;
        }
    }
    snapshot_.audio = analysisEngine_.generateDemo(elapsedMs_ / 1000.0);
    snapshot_.frameNumber++;
    snapshot_.renderScale = frameGovernor_.renderScale();
    snapshot_.actualFps = frameGovernor_.actualFps();
    snapshot_.frameTimeMs = frameGovernor_.frameTimeMs();
}

void WorkbenchModel::setMode(WorkbenchMode mode) { snapshot_.mode = mode; frameGovernor_.invalidate(); }
void WorkbenchModel::setTool(ActiveTool tool) { snapshot_.tool = tool; frameGovernor_.invalidate(); }
void WorkbenchModel::setPlaying(bool playing) { snapshot_.timeline.playing = playing; frameGovernor_.invalidate(); }

void WorkbenchModel::seek(double playheadMs) {
    snapshot_.timeline.playheadMs = std::clamp(playheadMs, 0.0, snapshot_.timeline.durationMs);
    frameGovernor_.invalidate();
}

Component* WorkbenchModel::findComponent(const std::string& id) {
    for (auto& component : snapshot_.components) if (component.id == id) return &component;
    return nullptr;
}

void WorkbenchModel::selectComponent(const std::string& id) {
    if (!findComponent(id)) return;
    snapshot_.selectedComponentId = id;
    for (auto& component : snapshot_.components) component.selected = component.id == id;
    frameGovernor_.invalidate();
}

void WorkbenchModel::toggleComponentVisibility(const std::string& id) {
    if (auto* component = findComponent(id)) {
        component->visible = !component->visible;
        frameGovernor_.invalidate();
    }
}

} // namespace pmp::core

