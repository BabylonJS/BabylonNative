#pragma once

#include <EGL/egl.h>

namespace Babylon::Graphics
{
    using DeviceType = EGLContext;
    using TextureType = GLuint;

    struct DeviceConfiguration
    {
        DeviceType Device;
        float DevicePixelRatio{1.f};
    };
}
