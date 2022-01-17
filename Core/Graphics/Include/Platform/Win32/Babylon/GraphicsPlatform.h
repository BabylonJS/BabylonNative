#pragma once

#include <Windows.h>

namespace Babylon
{
    constexpr BGFX_RESET_FLAGS = BGFX_RESET_VSYNC | BGFX_RESET_MSAA_X4 | BGFX_RESET_MAXANISOTROPY;
    using WindowType = HWND;

    struct WindowConfiguration
    {
        WindowType WindowPtr;
        size_t Width;
        size_t Height;
    };
}
