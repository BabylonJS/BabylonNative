#pragma once

#include <d3d11.h>

namespace Babylon::Graphics
{
    using DeviceType = ID3D11Device*;
    using BackBufferType = ID3D11RenderTargetView*;
    using DepthStencilBufferType = ID3D11DepthStencilView*;
    using TexturePointerType = ID3D11Texture2D*;
    using TextureFormat = DXGI_FORMAT;
    using WindowType = HWND;

    struct DeviceConfiguration
    {
        DeviceType Device;
        BackBufferType Backbuffer;
        DepthStencilBufferType DepthStencil;
        float DevicePixelRatio{1.f};
    };

    struct BackBufferUpdateInfo
    {
        BackBufferType Backbuffer;
        DepthStencilBufferType DepthStencil;
    };
}
