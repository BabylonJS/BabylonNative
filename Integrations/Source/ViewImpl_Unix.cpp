#include "RuntimeImpl.h"

#include <X11/Xlib.h>

namespace Babylon::Integrations
{
    std::pair<uint32_t, uint32_t> ViewImpl::QuerySize(Babylon::Graphics::WindowT window)
    {
        if (window == 0)
        {
            return {0, 0};
        }

        // X11 `Window` is just an XID; querying its geometry needs a
        // Display connection. Open one transiently — same pattern as
        // `Core/Graphics/Source/DeviceImpl_Unix.cpp::GetDevicePixelRatio`.
        // (See https://github.com/BabylonJS/BabylonNative/issues/625.)
        Display* display = XOpenDisplay(nullptr);
        if (display == nullptr)
        {
            return {0, 0};
        }

        ::Window root{};
        int x{}, y{};
        unsigned int width{}, height{}, borderWidth{}, depth{};
        Status status = XGetGeometry(display, window, &root, &x, &y,
                                      &width, &height, &borderWidth, &depth);

        XCloseDisplay(display);

        if (status == 0)
        {
            return {0, 0};
        }
        return {width, height};
    }

    std::pair<float, float> ViewImpl::ToLogicalCoords(float x, float y) const
    {
        // X11 button-event coordinates are in physical pixels. Divide
        // by the Device's queried DPR.
        const auto& impl = *m_runtime.m_impl;
        const float dpr = impl.m_device ? impl.m_device->GetDevicePixelRatio() : 1.0f;
        return {x / dpr, y / dpr};
    }
}
