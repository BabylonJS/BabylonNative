#pragma once

#include <EGL/egl.h>
#include <GL/gl.h>

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
