#pragma once

#include <Windows.h>

namespace Babylon
{
    using WindowType = HWND;

    struct WindowConfiguration
    {
        WindowType WindowPtr;
        size_t Width;
        size_t Height;
    };
}
