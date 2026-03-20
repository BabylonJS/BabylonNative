#include <gtest/gtest.h>
#include "Utils.h"

Babylon::Graphics::TextureT CreateTestTexture(Babylon::Graphics::DeviceT /*device*/, uint32_t /*width*/, uint32_t /*height*/, uint32_t /*arraySize*/)
{
    // Vulkan external texture creation not yet implemented
    return 0;
}

void DestroyTestTexture(Babylon::Graphics::TextureT /*texture*/)
{
    // Vulkan external texture destruction not yet implemented
}
