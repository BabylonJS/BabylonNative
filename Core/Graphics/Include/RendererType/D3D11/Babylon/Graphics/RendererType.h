#pragma once

#include <d3d11.h>

namespace Babylon::Graphics
{
    using DeviceType = ID3D11Device*;
    using TextureType = ID3D11Texture2D*;

    struct DeviceConfiguration
    {
        DeviceType Device;
        float DevicePixelRatio{1.f};
    };
}
