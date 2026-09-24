#include "render/Renderer.hpp"

#include <QOperatingSystemVersion>

namespace pmp {

RendererInfo Renderer::detect() {
    const QString requested = QStringLiteral(PMP_RENDER_BACKEND_NAME).toLower();

    if (requested == QStringLiteral("native-vulkan")) {
        return {RendererBackend::NativeVulkan, QStringLiteral("Vulkan"),
                QStringLiteral("Requested Vulkan backend; device initialization is next.")};
    }
    if (requested == QStringLiteral("bgfx")) {
        return {RendererBackend::Bgfx, QStringLiteral("bgfx"),
                QStringLiteral("bgfx backend selected; package integration is next.")};
    }
    if (requested == QStringLiteral("native-d3d12") ||
        (requested == QStringLiteral("auto") &&
         QOperatingSystemVersion::currentType() == QOperatingSystemVersion::Windows)) {
        return {RendererBackend::NativeD3D12, QStringLiteral("Direct3D 12"),
                QStringLiteral("Windows native backend selected; device initialization is next.")};
    }
    return {RendererBackend::Unavailable, QStringLiteral("Unavailable"),
            QStringLiteral("No renderer backend is configured for this platform yet.")};
}

} // namespace pmp

