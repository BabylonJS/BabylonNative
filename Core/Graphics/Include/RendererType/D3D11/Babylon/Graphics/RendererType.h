#pragma once

#include <cstdint>
#include <d3d11.h>

#define GRAPHICS_BACK_BUFFER_SUPPORT

namespace Babylon::Graphics
{
    using DeviceT = ID3D11Device*;
    using TextureT = ID3D11Resource*;
    using TextureFormatT = DXGI_FORMAT;
    using BackBufferColorT = ID3D11RenderTargetView*;
    using BackBufferDepthStencilT = ID3D11DepthStencilView*;

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
