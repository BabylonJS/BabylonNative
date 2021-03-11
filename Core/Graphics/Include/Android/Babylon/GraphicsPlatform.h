 #pragma once

#include <android/native_window.h>

using WindowType = ANativeWindow*;

struct GraphicsConfiguration
{
    WindowType windowPtr;
    size_t width;
    size_t height;
};