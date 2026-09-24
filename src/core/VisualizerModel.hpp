#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace pmp::core {

enum class WorkbenchMode { Compose, Preview, Inspect };
enum class ActiveTool { Select, Move, Scale, Orbit };

struct Transform {
    float x = 0.0F;
    float y = 0.0F;
    float z = 0.0F;
    float scale = 1.0F;
    float rotation = 0.0F;
    float opacity = 1.0F;
};

struct Component {
    std::string id;
    std::string label;
    std::string kind;
    Transform transform;
    bool visible = true;
    bool enabled = true;
    bool selected = false;
    float response = 1.0F;
};

struct AudioAnalysis {
    float energy = 0.0F;
    float smoothedEnergy = 0.0F;
    float energyDelta = 0.0F;
    float bass = 0.0F;
    float mid = 0.0F;
    float treble = 0.0F;
    float spectralCentroid = 0.0F;
    float peakValue = 0.0F;
    int peakBin = 0;
    float beatStrength = 0.0F;
    std::array<float, 64> spectrum{};
};

struct TimelineState {
    double durationMs = 180'000.0;
    double playheadMs = 0.0;
    double loopStartMs = 0.0;
    double loopEndMs = 180'000.0;
    bool playing = true;
    bool loopEnabled = false;
};

struct WorkbenchSnapshot {
    WorkbenchMode mode = WorkbenchMode::Compose;
    ActiveTool tool = ActiveTool::Select;
    std::string sceneId = "audio-visualizer";
    std::string sceneTitle = "Audio Visualizer";
    std::vector<Component> components;
    std::string selectedComponentId;
    TimelineState timeline;
    AudioAnalysis audio;
    uint64_t frameNumber = 0;
    double renderScale = 1.0;
    double actualFps = 60.0;
    double frameTimeMs = 0.0;
};

class AudioAnalysisEngine {
public:
    AudioAnalysis process(const std::array<float, 64>& bins, double timestampSeconds);
    AudioAnalysis generateDemo(double timestampSeconds);

private:
    AudioAnalysis state_;
    double previousTimestampSeconds_ = 0.0;
};

class FrameGovernor {
public:
    bool beginFrame(double timestampMs);
    void endFrame(double timestampMs, double renderDurationMs);
    void invalidate() { invalidated_ = true; }
    double renderScale() const { return renderScale_; }
    double actualFps() const { return actualFps_; }
    double frameTimeMs() const { return lastFrameMs_; }

private:
    double lastRenderMs_ = -1.0;
    double evaluationStartMs_ = -1.0;
    double sampleDurationMs_ = 0.0;
    int sampleFrames_ = 0;
    bool invalidated_ = true;
    double renderScale_ = 1.0;
    double actualFps_ = 60.0;
    double lastFrameMs_ = 0.0;
};

class WorkbenchModel {
public:
    WorkbenchModel();

    void advance(double deltaMs);
    void setMode(WorkbenchMode mode);
    void setTool(ActiveTool tool);
    void setPlaying(bool playing);
    void seek(double playheadMs);
    void selectComponent(const std::string& id);
    void toggleComponentVisibility(const std::string& id);

    const WorkbenchSnapshot& snapshot() const { return snapshot_; }

private:
    void initializeScene();
    Component* findComponent(const std::string& id);

    WorkbenchSnapshot snapshot_;
    AudioAnalysisEngine analysisEngine_;
    FrameGovernor frameGovernor_;
    double elapsedMs_ = 0.0;
};

} // namespace pmp::core

