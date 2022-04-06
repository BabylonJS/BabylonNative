#pragma once

#include <MetalKit/MetalKit.h>

namespace Babylon::Graphics
{
    using DeviceType = id<MTLDevice>;
    using TextureType = id<MTLTexture>;

    struct DeviceConfiguration
    {
        DeviceType Device;
        float DevicePixelRatio{1.f};
    };
}
