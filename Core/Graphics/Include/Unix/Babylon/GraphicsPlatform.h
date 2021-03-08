 #pragma once

#include <X11/Xlib.h>

using WindowType = Window*;

struct GraphicsConfiguration
{
    WindowType windowPtr;
    Screen* screen;
    void* windowTypePtr;
    size_t width;
    size_t height;
};