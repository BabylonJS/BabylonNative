#pragma once

#include <cstdint>

namespace Babylon::Graphics
{
    // Opaque handles for the Vulkan backend. ExternalTexture is not implemented yet, so these
    // are only used by PlatformInfo / Configuration::Device plumbing today.
    using DeviceT = void*;
    using TextureT = void*;
    using TextureFormatT = uint32_t;

    struct PlatformInfo
    {
        DeviceT Device;
    };
}
