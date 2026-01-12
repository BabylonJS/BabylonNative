#pragma once

#include <d3d12.h>

namespace Babylon::Graphics
{
    using DeviceT = ID3D12Device*;
    using TextureT = ID3D12Resource*;
    using TextureFormatT = DXGI_FORMAT;

    struct PlatformInfo
    {
        DeviceT Device;
    };
}
