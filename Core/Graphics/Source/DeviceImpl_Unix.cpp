#include <Babylon/Graphics/Platform.h>
#include "DeviceImpl.h"

namespace Babylon::Graphics
{
    void DeviceImpl::ConfigureBgfxPlatformData(bgfx::PlatformData& pd, WindowT window)
    {
        pd.nwh = reinterpret_cast<void*>(window);
    }

    void DeviceImpl::ConfigureBgfxRenderType(bgfx::PlatformData& /*pd*/, bgfx::RendererType::Enum& /*renderType*/)
    {
    }

    float DeviceImpl::GetDevicePixelRatio(WindowT)
    {
        // TODO: We should persist a Display object instead of opening a new display.
        // See https://github.com/BabylonJS/BabylonNative/issues/625

        auto display = XOpenDisplay(nullptr);
        auto screen = DefaultScreen(display);

        auto width = DisplayWidthMM(display, screen);
        auto pixelWidth = DisplayWidth(display, screen);

        XCloseDisplay(display);

        if (width > 0)
        {
            constexpr float MILLIMETERS_TO_INCHES = 0.03937f;
            auto dpi = pixelWidth / (width * MILLIMETERS_TO_INCHES);

            // X11 does not enforce a default dpi.
            // Use 96 dpi as our baseline DPI to match the behavior of Windows, and the default behavior of Linux Desktop Environments such as Gnome and KDE.
            // See: https://scanline.ca/dpi/
            return dpi / 96.0f;
        }

        return 1;
    }
}