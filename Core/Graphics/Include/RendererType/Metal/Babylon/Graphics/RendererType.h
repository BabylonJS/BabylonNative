#pragma once

#include <MetalKit/MetalKit.h>

namespace Babylon::Graphics
{
    using DeviceT = id<MTLDevice>;
    using TextureT = id<MTLTexture>;
    using TextureFormatT = MTLPixelFormat;

    struct PlatformInfo
    {
        struct MTLDevice* Device;
        struct MTLCommandQueue* CommandQueue;
    };
}
