#pragma once

#include <android/native_window.h>

namespace Babylon::Graphics
{
    using WindowType = ANativeWindow*;

    struct WindowConfiguration
    {
        WindowType WindowPtr;
        size_t Width;
        size_t Height;
    };
}
