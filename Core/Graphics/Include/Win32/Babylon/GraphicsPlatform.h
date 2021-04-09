#pragma once

#include <Windows.h>
#include <d3d11.h>

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

    struct ContextConfiguration
    {
        ContextType Context;
        float DevicePixelRatio{1.f};
    };
}
