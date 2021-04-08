 #pragma once

#include <android/native_window.h>

namespace Babylon
{
    using WindowType = ANativeWindow*;
    using ContextType = EGLContext;

    struct GraphicsConfiguration
    {
        WindowType WindowPtr;
        size_t Width;
        size_t Height;
    };
}
