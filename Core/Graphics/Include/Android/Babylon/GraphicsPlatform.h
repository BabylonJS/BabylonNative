 #pragma once

#include <Babylon/Graphics.h>
#include <android/native_window.h>

using WindowType = ANativeWindow*;

struct Babylon::Graphics::Configuration
{
    WindowType WindowPtr;
    size_t Width;
    size_t Height;
};