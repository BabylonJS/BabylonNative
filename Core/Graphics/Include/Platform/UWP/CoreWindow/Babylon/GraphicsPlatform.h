#pragma once

#include <winrt/windows.ui.core.h>

namespace Babylon
{
    using WindowType = winrt::Windows::UI::Core::ICoreWindow*;

    struct WindowConfiguration
    {
        WindowType WindowPtr;
        size_t Width;
        size_t Height;
    };
}
