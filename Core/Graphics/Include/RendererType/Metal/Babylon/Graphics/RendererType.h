#pragma once

#include <MetalKit/MetalKit.h>

namespace Babylon::Graphics
{
    using DeviceType = id<MTLDevice>;

    struct DeviceConfiguration
    {
        DeviceType Device;
        float DevicePixelRatio{1.f};
    };
}
