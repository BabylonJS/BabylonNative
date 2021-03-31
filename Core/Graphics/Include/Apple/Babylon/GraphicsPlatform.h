 #pragma once

#include <Babylon/Graphics.h>
#include <MetalKit/MetalKit.h>

using WindowType = MTKView*;

struct Babylon::GraphicsConfiguration
{
    WindowType WindowPtr;
    size_t Width;
    size_t Height;
};
