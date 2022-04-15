#pragma once

#include <d3d12.h>

namespace Babylon::Graphics
{
    using DeviceT = ID3D12Device*;
    using TextureT = ID3D12Resource*;

    struct DeviceConfiguration
    {
        DeviceT Device;
        float DevicePixelRatio{1.f};
    };
}
