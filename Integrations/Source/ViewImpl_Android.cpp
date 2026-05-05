#include "RuntimeImpl.h"

#include <android/native_window.h>

namespace Babylon::Integrations
{
    std::pair<uint32_t, uint32_t> ViewImpl::QuerySize(Babylon::Graphics::WindowT window)
    {
        if (window == nullptr)
        {
            return {0, 0};
        }
        return {static_cast<uint32_t>(ANativeWindow_getWidth(window)),
                static_cast<uint32_t>(ANativeWindow_getHeight(window))};
    }

    std::pair<float, float> ViewImpl::ToLogicalCoords(float x, float y) const
    {
        // Android `MotionEvent.getX/getY` returns coordinates in physical
        // pixels (the SurfaceView's pixel space). NativeInput / Babylon.js
        // expect logical (CSS) pixels — divide by the device-pixel-ratio
        // the underlying Device has cached.
        const auto& impl = *m_runtime.m_impl;
        const float dpr = impl.m_device ? impl.m_device->GetDevicePixelRatio() : 1.0f;
        return {x / dpr, y / dpr};
    }
}
