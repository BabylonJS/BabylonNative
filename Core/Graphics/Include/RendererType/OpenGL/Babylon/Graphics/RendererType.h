#pragma once

#include <EGL/egl.h>

namespace Babylon::Graphics
{
    using DeviceT = EGLContext;
    using TextureT = unsigned int;
    using TextureFormatT = unsigned int;

    struct DeviceConfiguration
    {
        DeviceT Device;
        float DevicePixelRatio{1.f};
    };

    struct PlatformInfo
    {
        DeviceT Device;
    };
}
