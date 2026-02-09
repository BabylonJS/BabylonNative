#pragma once

#include <vulkan/vulkan.h>

namespace Babylon::Graphics
{
    using DeviceT = VkDevice;
    using TextureT = VkImage;
    using TextureFormatT = VkFormat;

    struct PlatformInfo
    {
        DeviceT Device{};
    };
}
