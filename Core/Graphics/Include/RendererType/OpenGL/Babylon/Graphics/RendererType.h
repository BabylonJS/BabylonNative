#pragma once

#include <cstdint>

#include <Babylon/Graphics/GL/Texture.h>

namespace Babylon::Graphics
{
    using DeviceT = void*;
    using TextureFormatT = unsigned int;
    using TextureT = GL::Texture*;

    // The value the renderer wants as this texture's native handle: the GL name itself, not the
    // address of the wrapper holding it.
    inline uintptr_t NativeTextureHandle(TextureT ptr)
    {
        return ptr == nullptr ? 0 : static_cast<uintptr_t>(ptr->Handle());
    }

    struct PlatformInfo
    {
        DeviceT Device;
    };
}
