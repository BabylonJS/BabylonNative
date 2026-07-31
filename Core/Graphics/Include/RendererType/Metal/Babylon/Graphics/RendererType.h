#pragma once

#include <cstdint>
#include <Metal/Metal.hpp>

namespace Babylon::Graphics
{
    using DeviceT = MTL::Device*;
    using TextureT = MTL::Texture*;
    using TextureFormatT = MTL::PixelFormat;

    // The value the renderer wants as this texture's native handle.
    inline uintptr_t NativeTextureHandle(TextureT ptr)
    {
        return reinterpret_cast<uintptr_t>(ptr);
    }

    struct PlatformInfo
    {
        MTL::Device* Device;
        MTL::CommandQueue* CommandQueue;
    };
}
