 #pragma once

#include <android/native_window.h>

using WindowType = ANativeWindow*;

struct GraphicsConfiguration
{
    WindowType windowPtr;
    void* windowTypePtr;
    size_t width;
    size_t height;
};