#include <Babylon/Graphics/RendererType.h>
#include "DeviceImpl.h"

namespace Babylon::Graphics
{
    const bgfx::RendererType::Enum DeviceImpl::s_bgfxRenderType = bgfx::RendererType::Vulkan;

    PlatformInfo DeviceImpl::GetPlatformInfo() const
    {
        return {static_cast<DeviceT>(bgfx::getInternalData()->context)};
    }
}
