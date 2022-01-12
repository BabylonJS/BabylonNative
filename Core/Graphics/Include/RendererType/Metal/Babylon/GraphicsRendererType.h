#pragma once

#include <MetalKit/MetalKit.h>

namespace Babylon
{
    using ContextType = id<MTLDevice>;

    struct ContextConfiguration
    {
        ContextType Context;
        float DevicePixelRatio{1.f};
    };
}
