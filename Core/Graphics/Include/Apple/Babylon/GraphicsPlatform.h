 #pragma once

#include <MetalKit/MetalKit.h>

namespace Babylon
{
    using WindowType = MTKView*;
    using ContextType = id<MTLDevice>;

    struct GraphicsConfiguration
    {
        WindowType WindowPtr;
        size_t Width;
        size_t Height;
    };
}
