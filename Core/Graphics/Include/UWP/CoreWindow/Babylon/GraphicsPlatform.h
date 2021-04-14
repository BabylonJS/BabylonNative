#pragma once

#include <windows.ui.core.h>
#include <winrt/windows.ui.core.h>
#include <winrt/Windows.ApplicationModel.h>
#include <d3d11.h>

namespace Babylon
{
    using WindowType = winrt::Windows::UI::Core::ICoreWindow*;
    using ContextType = ID3D11DeviceContext*;

    struct WindowConfiguration
    {
        WindowType WindowPtr;
        size_t Width;
        size_t Height;
    };

    struct ContextConfiguration
    {
        ContextType Context;
        float DevicePixelRatio{1.f};
    };
}
