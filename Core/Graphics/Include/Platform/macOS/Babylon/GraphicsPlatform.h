#pragma once

#include <MetalKit/MTKView.h>

namespace Babylon
{
    using WindowType = MTKView*;

    struct WindowConfiguration
    {
        WindowType WindowPtr;
        size_t Width;
        size_t Height;
    };
}
