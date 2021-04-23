 #pragma once

#include <MetalKit/MetalKit.h>

namespace Babylon
{
    using WindowType = MTKView*;
    using ContextType = id<MTLDevice>;

    struct WindowConfiguration
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
