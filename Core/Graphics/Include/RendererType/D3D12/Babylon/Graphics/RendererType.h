#pragma once

#include <cstdint>
#include <d3d12.h>

namespace Babylon::Graphics
{
    using DeviceT = ID3D12Device*;
    using TextureT = ID3D12Resource*;
    using TextureFormatT = DXGI_FORMAT;

    // The value the renderer wants as this texture's native handle.
    inline uintptr_t NativeTextureHandle(TextureT ptr)
    {
        return reinterpret_cast<uintptr_t>(ptr);
    }

    struct PlatformInfo
    {
        DeviceT Device;
    };
}
