#pragma once

#include <d3d12.h>

namespace Babylon::Graphics
{
    using DeviceType = ID3D12Device*;

    using TextureType = ID3D12Resource*;
    using TextureFormat = DXGI_FORMAT;

    struct DeviceConfiguration
    {
        DeviceType Device;
        float DevicePixelRatio{1.f};
    };
}
