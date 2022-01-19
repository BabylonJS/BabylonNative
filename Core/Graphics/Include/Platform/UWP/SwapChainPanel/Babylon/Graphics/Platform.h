#pragma once

#include <winrt/Windows.UI.Xaml.Controls.h>

namespace Babylon::Graphics
{
    using WindowType = winrt::Windows::UI::Xaml::Controls::ISwapChainPanel;

    struct WindowConfiguration
    {
        WindowType SwapChainPanel{};
        size_t Width{};
        size_t Height{};
    };
}
