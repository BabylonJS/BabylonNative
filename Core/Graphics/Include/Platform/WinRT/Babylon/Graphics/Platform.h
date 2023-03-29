#pragma once

#include <stdint.h>
#include <Unknwn.h>
#include <winrt/base.h>

namespace Babylon::Graphics
{
    // Supported types:
    // - Windows::Core::UI::ICoreWindow
    // - Windows::UI::Xaml::Controls::ISwapChainPanel
    // - Microsoft::UI::Xaml::Controls::ISwapChainPanel
    using WindowType = winrt::Windows::Foundation::IInspectable;

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
