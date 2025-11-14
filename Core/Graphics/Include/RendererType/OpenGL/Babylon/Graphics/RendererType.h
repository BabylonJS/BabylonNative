#pragma once

#include <EGL/egl.h>
#include <GL/gl.h>

namespace Babylon::Graphics
{
    using DeviceT = EGLContext;
    using TextureT = GLenum;
    using TextureFormatT = GLenum;

    struct PlatformInfo
    {
        DeviceT Device;
    };
}
