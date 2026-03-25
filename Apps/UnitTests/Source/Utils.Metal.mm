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

Babylon::Graphics::TextureT CreateTestTextureArrayWithData(Babylon::Graphics::DeviceT device, uint32_t width, uint32_t height, const Color* sliceColors, uint32_t sliceCount)
{
    MTL::TextureDescriptor* descriptor = MTL::TextureDescriptor::texture2DDescriptor(
        MTL::PixelFormatRGBA8Unorm, width, height, false);
    descriptor->setTextureType(MTL::TextureType2DArray);
    descriptor->setArrayLength(sliceCount);
    descriptor->setUsage(MTL::TextureUsageShaderRead);
    descriptor->setStorageMode(MTL::StorageModeManaged);

    MTL::Texture* texture = device->newTexture(descriptor);
    EXPECT_NE(texture, nullptr);

    const uint32_t bytesPerRow = width * 4;
    const uint32_t sliceSize = bytesPerRow * height;
    std::vector<uint8_t> pixels(sliceSize);

    for (uint32_t s = 0; s < sliceCount; ++s)
    {
        for (uint32_t i = 0; i < width * height; ++i)
        {
            uint8_t* p = pixels.data() + i * 4;
            p[0] = sliceColors[s].R;
            p[1] = sliceColors[s].G;
            p[2] = sliceColors[s].B;
            p[3] = sliceColors[s].A;
        }

        MTL::Region region = MTL::Region::Make2D(0, 0, width, height);
        texture->replaceRegion(region, 0, s, pixels.data(), bytesPerRow, 0);
    }

    return texture;
}

Babylon::Graphics::TextureT CreateRenderTargetTexture(Babylon::Graphics::DeviceT device, uint32_t width, uint32_t height)
{
    MTL::TextureDescriptor* descriptor = MTL::TextureDescriptor::texture2DDescriptor(
        MTL::PixelFormatRGBA8Unorm, width, height, false);
    descriptor->setUsage(MTL::TextureUsageRenderTarget | MTL::TextureUsageShaderRead);
    descriptor->setStorageMode(MTL::StorageModeManaged);

    MTL::Texture* texture = device->newTexture(descriptor);
    EXPECT_NE(texture, nullptr);

    return texture;
}

std::vector<uint8_t> ReadBackRenderTarget(Babylon::Graphics::DeviceT device, Babylon::Graphics::TextureT texture, uint32_t width, uint32_t height)
{
    const uint32_t bytesPerRow = width * 4;
    std::vector<uint8_t> pixels(bytesPerRow * height);

    MTL::Region region = MTL::Region::Make2D(0, 0, width, height);
    texture->getBytes(pixels.data(), bytesPerRow, region, 0);

    return pixels;
}

void DestroyRenderTargetTexture(Babylon::Graphics::TextureT texture)
{
    texture->release();
}
