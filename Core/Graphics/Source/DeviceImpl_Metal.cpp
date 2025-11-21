#include <Babylon/Graphics/RendererType.h>
#include "DeviceImpl.h"

namespace Babylon::Graphics
{
    const bgfx::RendererType::Enum DeviceImpl::s_bgfxRenderType = bgfx::RendererType::Metal;

    PlatformInfo DeviceImpl::GetPlatformInfo() const
    {
        auto internalData = bgfx::getInternalData();
        id<MTLDevice> device = (id<MTLDevice>)internalData->context;
        id<MTLCommandQueue> commandQueue = (id<MTLCommandQueue>)internalData->commandQueue;
        return {device, commandQueue};
    }
}
