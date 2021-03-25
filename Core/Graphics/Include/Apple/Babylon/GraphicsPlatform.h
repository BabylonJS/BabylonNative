 #pragma once

#include <MetalKit/MetalKit.h>

using WindowType = MTKView*;

struct GraphicsConfiguration
{
    MTKView* windowPtr;
    size_t width;
    size_t height;
};
