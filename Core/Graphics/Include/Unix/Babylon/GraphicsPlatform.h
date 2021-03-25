 #pragma once

#include <X11/Xlib.h>

using WindowType = void*;

struct GraphicsConfiguration
{
    void* windowPtr;
    size_t width;
    size_t height;
};