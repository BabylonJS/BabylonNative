#pragma once

#include <winrt/Windows.UI.Xaml.Controls.h>

namespace Babylon
{
    constexpr BGFX_RESET_FLAGS = BGFX_RESET_VSYNC | BGFX_RESET_MSAA_X4 | BGFX_RESET_MAXANISOTROPY;
    using WindowType = winrt::Windows::UI::Xaml::Controls::ISwapChainPanel;

    struct WindowConfiguration
    {
        WindowType SwapChainPanel{};
        size_t Width{};
        size_t Height{};
    };
}
