#pragma once

#include <MetalKit/MetalKit.h>

namespace Babylon::Graphics
{
    using ContextType = id<MTLDevice>;

    struct ContextConfiguration
    {
        ContextType Context;
        float DevicePixelRatio{1.f};
    };
}
