#pragma once

#include <winrt/Windows.UI.Xaml.Controls.h>

namespace Babylon
{
    using WindowType = winrt::Windows::UI::Xaml::Controls::ISwapChainPanel;

    struct WindowConfiguration
    {
        WindowType Window{};
        size_t Width{};
        size_t Height{};
    };
}
