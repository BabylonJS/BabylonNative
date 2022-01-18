#pragma once

#include <EGL/egl.h>

namespace Babylon
{
    using ContextType = EGLContext;
    using TexturePointerType = uintptr_t;

    struct ContextConfiguration
    {
        ContextType Context;
        float DevicePixelRatio{1.f};
    };
}
