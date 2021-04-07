#pragma once

#include <Windows.h>

namespace Babylon
{
    using WindowType = HWND;

    struct GraphicsConfiguration
    {
        WindowType WindowPtr;
        size_t Width;
        size_t Height;
    };
}
