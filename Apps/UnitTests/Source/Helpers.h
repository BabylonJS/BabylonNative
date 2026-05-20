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

    // Reads back the texture's pixels into an 8-bit RGBA/BGRA byte buffer (backend's native order).
    // If the texture is multisampled, the samples are resolved down to 1 before the readback.
    // Implemented for D3D11 and Metal; D3D12 / OpenGL throw.
    std::vector<uint8_t> ReadPixels(const Babylon::Graphics::PlatformInfo& platformInfo, Babylon::Graphics::TextureT texture, uint32_t width, uint32_t height);
}
