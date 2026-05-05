#include "RuntimeImpl.h"

#include <Windows.h>

namespace Babylon::Integrations
{
    std::pair<uint32_t, uint32_t> ViewImpl::QuerySize(Babylon::Graphics::WindowT window)
    {
        RECT rect{};
        if (window == nullptr || !GetClientRect(window, &rect))
        {
            return {0, 0};
        }
        return {static_cast<uint32_t>(rect.right - rect.left),
                static_cast<uint32_t>(rect.bottom - rect.top)};
    }

    std::pair<float, float> ViewImpl::ToLogicalCoords(float x, float y) const
    {
        // Win32 `WM_MOUSE*` / `WM_POINTER*` coordinates are in physical
        // pixels for DPI-aware apps. Divide by the Device's queried DPR.
        const auto& impl = *m_runtime.m_impl;
        const float dpr = impl.m_device ? impl.m_device->GetDevicePixelRatio() : 1.0f;
        return {x / dpr, y / dpr};
    }
}
