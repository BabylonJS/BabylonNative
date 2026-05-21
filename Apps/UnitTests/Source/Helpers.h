#pragma once

#include <Babylon/Graphics/Device.h>

#include <cstdint>
#include <vector>

namespace Helpers
{
    // Creates a 2D texture (or 2D array if arraySize > 1). When renderTarget is true, the texture
    // also gets the render-target bind flag so it can be wrapped as the color attachment of a
    // bgfx framebuffer via wrapNativeTexture. samples > 1 requires renderTarget = true.
    // Implemented for D3D11 and Metal; D3D12 / OpenGL throw.
    Babylon::Graphics::TextureT CreateTexture(Babylon::Graphics::DeviceT device, uint32_t width, uint32_t height, uint32_t arraySize = 1, bool renderTarget = false, uint32_t samples = 1);
    void DestroyTexture(Babylon::Graphics::TextureT texture);

    // Reads back the texture's pixels into an 8-bit RGBA byte buffer (R, G, B, A byte order on all backends).
    // If the texture is multisampled, the samples are resolved down to 1 before the readback.
    // Implemented for D3D11 and Metal; D3D12 / OpenGL throw.
    std::vector<uint8_t> ReadPixels(const Babylon::Graphics::PlatformInfo& platformInfo, Babylon::Graphics::TextureT texture, uint32_t width, uint32_t height);

    // Returns a graphics device suitable for use as Babylon::Graphics::Configuration::Device. The
    // returned handle is owned by the caller and must be released with DestroyDevice.
    //
    // Defined only on D3D11 and D3D12 -- the Tests.Device.cpp test that consumes these helpers is
    // gated to those backends. On D3D11 returns an ID3D11Device created via D3D11CreateDevice(WARP);
    // on D3D12 returns an ID3D12Device created via D3D12CreateDevice on the WARP DXGI adapter.
    //
    // Note: on D3D12, D3D12CreateDevice(WARP) returns the same singleton pointer on successive calls
    // in the same process, so two calls in a row may return equal handles. D3D11CreateDevice(WARP)
    // does return distinct handles per call. Metal / OpenGL throw.
    Babylon::Graphics::DeviceT CreateDevice();
    void DestroyDevice(Babylon::Graphics::DeviceT device);
}
