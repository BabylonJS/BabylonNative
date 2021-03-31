 #pragma once

#include <Babylon/Graphics.h>
#include <X11/Xlib.h>

using WindowType = void*;

struct Babylon::GraphicsConfiguration
{
    WindowType WindowPtr;
    size_t Width;
    size_t Height;
};
