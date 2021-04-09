 #pragma once

#include <X11/Xlib.h>
#include <GL/glx.h>

namespace Babylon
{
    using WindowType = void*;
    using ContextType = GLXContext;

    struct GraphicsConfiguration
    {
        WindowType WindowPtr;
        size_t Width;
        size_t Height;
    };

    struct ContextConfiguration
    {
        ContextType Context;
        float DefaultPixelRatio{1.f};
    };
}
