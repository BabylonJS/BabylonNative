#include <Babylon/Graphics.h>
#include <Babylon/GraphicsPlatform.h>
#include "../GraphicsImpl.h"

constexpr float MILLIMETERS_TO_INCHES = 0.03937;

namespace Babylon
{
    void Graphics::Impl::SetNativeWindow(GraphicsConfiguration config)
    {
        SetNativeWindowInternal(config);
    }

    float Graphics::Impl::UpdateDevicePixelRatio()
    {
        // TODO: We should persist a Display object instead of opening a new display.
        // See https://github.com/BabylonJS/BabylonNative/issues/625

        auto display = XOpenDisplay(nullptr);
        auto screen = DefaultScreen(display);

        auto width = DisplayWidthMM(display, screen);
        auto pixelWidth = DisplayWidth(display, screen);

        std::scoped_lock lock{m_state.Mutex};
        if (width > 0){
            auto dpi = pixelWidth/(width * MILLIMETERS_TO_INCHES);

            // X11 does not enforce a default dpi.
            // Use 96 dpi as our baseline DPI to match the behavior of Windows, and the default behavior of Linux Desktop Environments such as Gnome and KDE.
            // See: https://scanline.ca/dpi/
            m_state.Resolution.DevicePixelRatio = dpi/96.0f;
        }

        return m_state.Resolution.DevicePixelRatio;
    }
}