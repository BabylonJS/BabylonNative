#pragma once

#include <d3d11.h>

namespace Babylon
{
    using ContextType = ID3D11Device*;
    using TexturePointerType = uintptr_t;

    struct ContextConfiguration
    {
        ContextType Context;
        float DevicePixelRatio{1.f};
    };
}
