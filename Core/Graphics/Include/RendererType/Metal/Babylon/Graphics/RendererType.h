#pragma once

namespace Babylon::Graphics
{
    using DeviceT = void*;
    using TextureT = void*;
    using TextureFormatT = unsigned int;

    struct DeviceConfiguration
    {
        DeviceT Device;
        float DevicePixelRatio{1.f};
    };

    struct PlatformInfo
    {
        void* Device;
        void* CommandQueue;
    };
}
