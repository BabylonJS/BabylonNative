#pragma once

#include <EGL/egl.h>

namespace Babylon::Graphics
{
    using DeviceType = EGLContext;
    using TextureType = unsigned int;

    struct DeviceConfiguration
    {
        DeviceType Device;
        float DevicePixelRatio{1.f};
    };
}
