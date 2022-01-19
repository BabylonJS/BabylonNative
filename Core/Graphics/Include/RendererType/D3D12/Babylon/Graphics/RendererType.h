#pragma once

#include <d3d12.h>

namespace Babylon::Graphics
{
    using ContextType = ID3D12Device*;

    struct ContextConfiguration
    {
        ContextType Context;
        float DevicePixelRatio{1.f};
    };
}
