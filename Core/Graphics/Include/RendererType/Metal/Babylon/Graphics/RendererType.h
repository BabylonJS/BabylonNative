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
        // Add explicit default copy and move semantics to fix a retain issue.
        // Not sure why this is necessary. Not doing this leads to the command
        // queue being released early, causing a EXC_BAD_ACCESS in bgfx when
        // it tries to use the command queue.
        PlatformInfo(const PlatformInfo&) = default;
        PlatformInfo& operator=(const PlatformInfo&) = default;
        PlatformInfo(PlatformInfo&&) = default;
        PlatformInfo& operator=(PlatformInfo&&) = default;

        DeviceT Device;
        id<MTLCommandQueue> CommandQueue;
    };
}
