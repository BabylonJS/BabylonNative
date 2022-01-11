#pragma once

#ifdef __OBJ_C__
#include <MetalKit/MTKView.h>
#else
using MTKView = void;
#endif

namespace Babylon
{
    using WindowType = MTKView*;

    struct WindowConfiguration
    {
        WindowType WindowPtr;
        size_t Width;
        size_t Height;
    };
}
