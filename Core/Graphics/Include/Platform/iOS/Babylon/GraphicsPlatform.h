#pragma once

#include <MetalKit/MTKView.h>

namespace Babylon
{
    constexpr BGFX_RESET_FLAGS = BGFX_RESET_VSYNC | BGFX_RESET_MAXANISOTROPY;
    using WindowType = MTKView*;

    struct WindowConfiguration
    {
        WindowType WindowPtr;
        size_t Width;
        size_t Height;
    };
}
