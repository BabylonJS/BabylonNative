 #pragma once

#include <X11/Xlib.h>
#include <GL/glx.h>

namespace Babylon
{
    using WindowType = void*;
    using ContextType = GLXContext;

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
