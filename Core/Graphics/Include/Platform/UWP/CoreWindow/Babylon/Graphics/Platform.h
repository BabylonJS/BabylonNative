#pragma once

#include <winrt/windows.ui.core.h>

namespace Babylon::Graphics
{
    using WindowType = winrt::Windows::UI::Core::ICoreWindow*;

    struct WindowConfiguration
    {
        WindowType Window{};
        size_t Width{};
        size_t Height{};
    };
}
