#include "RuntimeImpl.h"

#include <X11/Xlib.h>

namespace Babylon::Integrations
{
    ViewImpl::QuerySizeResult ViewImpl::QuerySize(Babylon::Graphics::WindowT window)
    {
        if (window == 0)
        {
            return {0, 0, CoordinateUnits::Physical};
        }

        // X11 `Window` is just an XID; querying its geometry needs a
        // Display connection. Open one transiently — same pattern as
        // `Core/Graphics/Source/DeviceImpl_Unix.cpp::GetDevicePixelRatio`.
        // (See https://github.com/BabylonJS/BabylonNative/issues/625.)
        Display* display = XOpenDisplay(nullptr);
        if (display == nullptr)
        {
            return {0, 0, CoordinateUnits::Physical};
        }

        ::Window root{};
        int x{}, y{};
        unsigned int width{}, height{}, borderWidth{}, depth{};
        Status status = XGetGeometry(display, window, &root, &x, &y,
                                      &width, &height, &borderWidth, &depth);

        XCloseDisplay(display);

        if (status == 0)
        {
            return {0, 0, CoordinateUnits::Physical};
        }
        // XGetGeometry returns the window's size in physical pixels.
        return {width, height, CoordinateUnits::Physical};
    }
}
