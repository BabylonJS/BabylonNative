 #pragma once

#include <MetalKit/MetalKit.h>

using WindowType = MTKView*;

namespace Babylon::Graphics
{
    struct Configuration
    {
        WindowType WindowPtr;
        size_t Width;
        size_t Height;
    };
}
