#pragma once

#include <Metal/Metal.hpp>

namespace Babylon::Graphics
{
    using DeviceT = MTL::Device*;
    using TextureT = MTL::Texture*;
    using TextureFormatT = MTL::PixelFormat;

    struct PlatformInfo
    {
        MTL::Device* Device;
        MTL::CommandQueue* CommandQueue;
    };
}
