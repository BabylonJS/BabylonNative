#include "RuntimeImpl.h"

#include <android/native_window.h>

namespace Babylon::Integrations
{
    ViewImpl::QuerySizeResult ViewImpl::QuerySize(Babylon::Graphics::WindowT window)
    {
        if (window == nullptr)
        {
            return {0, 0, CoordinateUnits::Physical};
        }
        // ANativeWindow_getWidth/Height return the surface's
        // pixel-buffer size in physical (device) pixels.
        return {static_cast<uint32_t>(ANativeWindow_getWidth(window)),
                static_cast<uint32_t>(ANativeWindow_getHeight(window)),
                CoordinateUnits::Physical};
    }
}
