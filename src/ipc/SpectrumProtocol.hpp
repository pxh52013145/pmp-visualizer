#pragma once

#include <cstdint>
#include <span>
#include <vector>

namespace pmp::ipc {

enum class SpectrumTap { PreDsp, PostDsp };

struct SpectrumFrame {
    uint64_t frameId = 0;
    uint64_t timestampMs = 0;
    SpectrumTap tap = SpectrumTap::PostDsp;
    uint32_t sampleRate = 0;
    std::vector<uint8_t> bins;
    std::vector<uint8_t> timeDomain;
};

// Decodes PMS1 frames emitted by the existing PMP native audio service.
// The decoder is allocation-bounded by the frame header and rejects malformed input.
bool decodeSpectrumFrame(std::span<const uint8_t> bytes, SpectrumFrame& output);

} // namespace pmp::ipc

