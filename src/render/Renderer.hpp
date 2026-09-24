#pragma once

#include <QString>

namespace pmp {

enum class RendererBackend { NativeD3D12, NativeVulkan, Bgfx, Unavailable };

struct RendererInfo {
    RendererBackend backend;
    QString backendName;
    QString status;
};

class Renderer {
public:
    static RendererInfo detect();
};

} // namespace pmp

