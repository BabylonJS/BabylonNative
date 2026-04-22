#pragma once

namespace Babylon::Graphics
{
    using DeviceT = void*;
    using TextureT = unsigned int;
    using TextureFormatT = unsigned int;

    struct PlatformInfo
    {
        DeviceT Device;
    };
}
