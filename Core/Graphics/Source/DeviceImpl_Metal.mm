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

    void DeviceImpl::ResizeRenderSurface(WindowT window, uint32_t width, uint32_t height)
    {
        // The window is the CAMetalLayer we render into. Resize its drawable to
        // match the render resolution bgfx will use so the backbuffer and the
        // layer stay in lockstep (e.g. across hardware scaling level changes).
        // The window is null before the first UpdateWindow (e.g. during
        // construction), in which case there's nothing to size.
        if (window != nullptr)
        {
            window->setDrawableSize(CGSizeMake(static_cast<CGFloat>(width), static_cast<CGFloat>(height)));
        }
    }
}
