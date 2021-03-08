 #pragma once

#include <MetalKit/MetalKit.h>

using WindowType = MTKView*;

struct GraphicsConfiguration
{
    WindowType windowPtr;
    void* windowTypePtr;
    size_t width;
    size_t height;
};