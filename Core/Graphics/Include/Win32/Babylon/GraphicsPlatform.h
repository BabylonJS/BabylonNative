#pragma once

#include <Windows.h>

namespace Babylon
{
    using WindowType = HWND;
    using ContextType = ID3D11DeviceContext*;

    struct GraphicsConfiguration
    {
        WindowType WindowPtr;
        size_t Width;
        size_t Height;
    };
}
