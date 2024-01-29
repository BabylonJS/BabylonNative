#pragma once

#include <MetalKit/MetalKit.h>

namespace Babylon::Graphics
{
    using DeviceT = id<MTLDevice>;
    using TextureT = id<MTLTexture>;

    struct DeviceConfiguration
    {
        DeviceT Device;
        float DevicePixelRatio{1.f};
    };

    struct PlatformInfo
    {
        struct MTLDevice* Device;
        struct MTLCommandQueue* CommandQueue;
    };
}
