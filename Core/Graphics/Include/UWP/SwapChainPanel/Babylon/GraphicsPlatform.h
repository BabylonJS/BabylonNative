#pragma once

#include <winrt/Windows.UI.Xaml.Controls.h>

namespace Babylon
{
    using WindowType = winrt::Windows::UI::Xaml::Controls::ISwapChainPanel*;
    using ContextType = ID3D11DeviceContext*;

    struct GraphicsConfiguration
    {
        WindowType WindowPtr;
        size_t Width;
        size_t Height;
    };
}
