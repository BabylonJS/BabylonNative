#include "RuntimeImpl.h"

#include <Windows.h>

namespace Babylon::Integrations
{
    ViewImpl::QuerySizeResult ViewImpl::QuerySize(Babylon::Graphics::WindowT window)
    {
        RECT rect{};
        if (window == nullptr || !GetClientRect(window, &rect))
        {
            return {0, 0, CoordinateUnits::Physical};
        }
        // For DPI-aware apps, `GetClientRect` returns the surface's
        // pixel-buffer size in physical pixels.
        return {static_cast<uint32_t>(rect.right - rect.left),
                static_cast<uint32_t>(rect.bottom - rect.top),
                CoordinateUnits::Physical};
    }
}
