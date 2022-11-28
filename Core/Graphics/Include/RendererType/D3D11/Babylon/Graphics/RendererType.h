#pragma once

#include <d3d11.h>

namespace Babylon::Graphics
{
    using DeviceT = ID3D11Device*;
    using TextureT = ID3D11Resource*;

    struct DeviceConfiguration
    {
        DeviceT Device;
        float DevicePixelRatio{1.f};
    };
}
