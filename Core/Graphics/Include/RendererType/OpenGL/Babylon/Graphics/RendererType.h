#pragma once

#include <EGL/egl.h>

namespace Babylon::Graphics
{
    using DeviceT = EGLContext;
    using TextureT = unsigned int;
    using TextureFormatT = unsigned int;

    struct PlatformInfo
    {
        DeviceT Device;
    };
}
