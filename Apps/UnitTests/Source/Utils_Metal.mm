#include <gtest/gtest.h>
#include "Utils.h"
#import <Metal/Metal.h>

Babylon::Graphics::TextureT CreateTestTexture(Babylon::Graphics::DeviceT device, uint32_t width, uint32_t height, uint32_t arraySize)
{
    MTLTextureDescriptor* descriptor = [MTLTextureDescriptor
        texture2DDescriptorWithPixelFormat:MTLPixelFormatBGRA8Unorm
            width:width
            height:height
            mipmapped:NO];

    if (arraySize > 1)
    {
        descriptor.textureType = MTLTextureType2DArray;
        descriptor.arrayLength = arraySize;
    }

    id<MTLTexture> texture = [device newTextureWithDescriptor:descriptor];
    EXPECT_NE(texture, nil);

    return texture;
}

void DestroyTestTexture(Babylon::Graphics::TextureT texture)
{
    // ARC will handle the release automatically
    (void)texture;
}
