 #pragma once

#include <X11/Xlib.h>

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
}
