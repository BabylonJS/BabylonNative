#pragma once

#include <MetalKit/MetalKit.h>

namespace Babylon::Graphics
{
    using DeviceT = id<MTLDevice>;
    using TextureT = id<MTLTexture>;
    using TextureFormatT = MTLPixelFormat;

    struct PlatformInfo
    {
        id<MTLDevice> Device;
        id<MTLCommandQueue> CommandQueue;
    };
}
