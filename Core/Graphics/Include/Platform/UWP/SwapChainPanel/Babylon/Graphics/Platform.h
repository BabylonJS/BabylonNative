#pragma once

#include <winrt/Windows.UI.Xaml.Controls.h>
#include <stdint.h>

namespace Babylon::Graphics
{
    using WindowType = winrt::Windows::UI::Xaml::Controls::ISwapChainPanel;

    struct WindowConfiguration
    {
        WindowType Window{};
        size_t Width{};
        size_t Height{};
        // MSAA sample values can be 2, 4, 8 or 16. Any other value will disable MSAA
        uint8_t MSAASamples{};
        // When enabled, back buffer will be premultiplied with alpha value
        bool AlphaPremultiplied{};
    };
}
