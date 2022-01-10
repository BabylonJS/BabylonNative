#pragma once

#include <EGL/egl.h>

namespace Babylon
{
    using ContextType = EGLContext;

    struct ContextConfiguration
    {
        ContextType Context;
        float DevicePixelRatio{1.f};
    };
}
