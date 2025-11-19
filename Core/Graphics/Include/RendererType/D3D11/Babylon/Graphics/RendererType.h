#pragma once

#include <d3d11.h>

#define GRAPHICS_BACK_BUFFER_SUPPORT

namespace Babylon::Graphics
{
    using DeviceT = ID3D11Device*;
    using TextureT = ID3D11Resource*;
    using TextureFormatT = DXGI_FORMAT;
    using BackBufferColorT = ID3D11RenderTargetView*;
    using BackBufferDepthStencilT = ID3D11DepthStencilView*;

    struct PlatformInfo
    {
        DeviceT Device;
    };
}
