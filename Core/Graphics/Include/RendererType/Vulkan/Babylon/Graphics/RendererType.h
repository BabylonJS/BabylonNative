#pragma once

#include <cstdint>

namespace Babylon::Graphics
{
    using DeviceT = void*;
    using TextureT = uint64_t;
    using TextureFormatT = uint32_t;

    struct PlatformInfo
    {
        DeviceT Device;
    };
}
