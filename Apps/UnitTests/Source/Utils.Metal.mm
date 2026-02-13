#include <gtest/gtest.h>
#include "Utils.h"

Babylon::Graphics::TextureT CreateTestTexture(Babylon::Graphics::DeviceT device, uint32_t width, uint32_t height, uint32_t arraySize)
{
    MTL::TextureDescriptor* descriptor = MTL::TextureDescriptor::texture2DDescriptor(
        MTL::PixelFormatBGRA8Unorm, width, height, false);

    if (arraySize > 1)
    {
        descriptor->setTextureType(MTL::TextureType2DArray);
        descriptor->setArrayLength(arraySize);
    }

    MTL::Texture* texture = device->newTexture(descriptor);
    EXPECT_NE(texture, nullptr);

    return texture;
}

void DestroyTestTexture(Babylon::Graphics::TextureT texture)
{
    texture->release();
}
