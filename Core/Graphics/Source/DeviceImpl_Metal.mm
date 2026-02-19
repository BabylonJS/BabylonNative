#include <Babylon/Graphics/RendererType.h>
#include "DeviceImpl.h"

namespace Babylon::Graphics
{
    const bgfx::RendererType::Enum DeviceImpl::s_bgfxRenderType = bgfx::RendererType::Metal;

    PlatformInfo DeviceImpl::GetPlatformInfo() const
    {
        auto internalData = bgfx::getInternalData();
        return {
            reinterpret_cast<MTL::Device*>(internalData->context),
            reinterpret_cast<MTL::CommandQueue*>(internalData->commandQueue),
        };
    }
}
