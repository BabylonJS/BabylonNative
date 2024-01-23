#include <Babylon/Graphics/RendererType.h>
#include "DeviceImpl.h"

namespace Babylon::Graphics
{
    const bgfx::RendererType::Enum DeviceImpl::s_bgfxRenderType = bgfx::RendererType::Metal;

    PlatformInfo DeviceImpl::GetPlatformInfo() const
    {
        return {
            static_cast<DeviceT>(bgfx::getInternalData()->context),
            static_cast<id<MTLCommandQueue>>(bgfx::getInternalData()->commandQueue)
        };
    }
}
