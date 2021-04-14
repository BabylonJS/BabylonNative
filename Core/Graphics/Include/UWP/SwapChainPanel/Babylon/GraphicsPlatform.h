#pragma once

#include <winrt/Windows.UI.Xaml.Controls.h>
#include <d3d11.h>

namespace Babylon
{
    using WindowType = winrt::Windows::UI::Xaml::Controls::ISwapChainPanel*;
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
