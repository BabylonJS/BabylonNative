 #pragma once

#include <X11/Xlib.h>

using WindowType = void*;

struct GraphicsConfiguration
{
    WindowType windowPtr;
    void* windowTypePtr;
    size_t width;
    size_t height;
};