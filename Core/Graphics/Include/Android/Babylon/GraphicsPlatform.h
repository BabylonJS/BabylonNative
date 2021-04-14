 #pragma once

#include <android/native_window.h>
#include <EGL/egl.h>

namespace Babylon
{
    using WindowType = ANativeWindow*;
    using ContextType = EGLContext;

    struct WindowConfiguration
    {
        WindowType WindowPtr;
        size_t Width;
        size_t Height;
    };

    struct ContextConfiguration
    {
        ContextType Context;
        float DevicePixelRatio{1.f};
    };
}
