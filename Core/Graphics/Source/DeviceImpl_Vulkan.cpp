#include <Babylon/Graphics/RendererType.h>
#include "DeviceImpl.h"

namespace Babylon::Graphics
{
    const bgfx::RendererType::Enum DeviceImpl::s_bgfxRenderType = bgfx::RendererType::Vulkan;

    PlatformInfo DeviceImpl::GetPlatformInfo() const
    {
        return {static_cast<DeviceT>(bgfx::getInternalData()->context)};
    }

    void DeviceImpl::ResizeRenderSurface(WindowT window, uint32_t width, uint32_t height)
    {
#if defined(__linux__) && !defined(__ANDROID__)
        if (window != WindowT{} && width != 0 && height != 0 && m_nativeDisplay)
        {
            auto* display = static_cast<Display*>(m_nativeDisplay.get());
            XResizeWindow(display, window, width, height);
            // Vulkan uses the X11 surface's current extent, so apply the resize before updating the swap chain.
            XSync(display, False);
        }
#else
        (void)window;
        (void)width;
        (void)height;
#endif
    }
}
