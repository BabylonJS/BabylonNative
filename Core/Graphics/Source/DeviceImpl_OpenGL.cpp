#include <Babylon/Graphics/RendererType.h>
#include "DeviceImpl.h"

namespace Babylon::Graphics
{
    const bgfx::RendererType::Enum DeviceImpl::s_bgfxRenderType = bgfx::RendererType::OpenGLES;

    PlatformInfo DeviceImpl::GetPlatformInfo() const
    {
        return {static_cast<DeviceT>(bgfx::getInternalData()->context)};
    }

    void DeviceImpl::UpdateWindowSize(WindowT /*window*/, uint32_t /*width*/, uint32_t /*height*/)
    {
        // No-op: the surface size is managed elsewhere on this platform.
    }
}
