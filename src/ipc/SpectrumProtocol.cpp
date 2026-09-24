#include "ipc/SpectrumProtocol.hpp"

#include <algorithm>
#include <cstring>

namespace pmp::ipc {

namespace {
constexpr size_t kHeaderBytes = 32;
constexpr uint8_t kVersion = 1;
constexpr uint8_t kHasTimeDomain = 0x01;

uint16_t readU16(const uint8_t* data) {
    return static_cast<uint16_t>(data[0]) | static_cast<uint16_t>(data[1] << 8U);
}

uint32_t readU32(const uint8_t* data) {
    return static_cast<uint32_t>(data[0]) |
           (static_cast<uint32_t>(data[1]) << 8U) |
           (static_cast<uint32_t>(data[2]) << 16U) |
           (static_cast<uint32_t>(data[3]) << 24U);
}

uint64_t readU64(const uint8_t* data) {
    return static_cast<uint64_t>(readU32(data)) |
           (static_cast<uint64_t>(readU32(data + 4)) << 32U);
}
} // namespace

bool decodeSpectrumFrame(std::span<const uint8_t> bytes, SpectrumFrame& output) {
    if (bytes.size() < kHeaderBytes || bytes[0] != 'P' || bytes[1] != 'M' ||
        bytes[2] != 'S' || bytes[3] != '1' || bytes[4] != kVersion) {
        return false;
    }
    if (bytes[5] > 1) return false;
    const uint8_t flags = bytes[6];
    const uint16_t binsLength = readU16(bytes.data() + 28);
    const uint16_t timeLength = readU16(bytes.data() + 30);
    const size_t payloadLength = static_cast<size_t>(binsLength) + timeLength;
    if (bytes.size() != kHeaderBytes + payloadLength ||
        ((flags & kHasTimeDomain) == 0 && timeLength != 0)) return false;

    output.frameId = readU64(bytes.data() + 8);
    output.timestampMs = readU64(bytes.data() + 16);
    output.tap = bytes[5] == 0 ? SpectrumTap::PreDsp : SpectrumTap::PostDsp;
    output.sampleRate = readU32(bytes.data() + 24);
    output.bins.assign(bytes.begin() + static_cast<std::ptrdiff_t>(kHeaderBytes),
                       bytes.begin() + static_cast<std::ptrdiff_t>(kHeaderBytes + binsLength));
    output.timeDomain.assign(bytes.begin() + static_cast<std::ptrdiff_t>(kHeaderBytes + binsLength), bytes.end());
    return !output.bins.empty();
}

} // namespace pmp::ipc

