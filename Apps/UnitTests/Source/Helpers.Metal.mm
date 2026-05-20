#include <gtest/gtest.h>
#include "Helpers.h"

namespace Helpers
{
    Babylon::Graphics::TextureT CreateTexture(Babylon::Graphics::DeviceT device, uint32_t width, uint32_t height, uint32_t arraySize, bool renderTarget, uint32_t samples)
    {
        if (samples > 1 && !renderTarget)
        {
            throw std::runtime_error{"samples > 1 requires renderTarget = true"};
        }
        if (samples > 1 && arraySize > 1)
        {
            throw std::runtime_error{"samples > 1 with arraySize > 1 not supported"};
        }

        MTL::TextureDescriptor* descriptor = MTL::TextureDescriptor::texture2DDescriptor(
            MTL::PixelFormatRGBA8Unorm, width, height, false);

        if (samples > 1)
        {
            descriptor->setTextureType(MTL::TextureType2DMultisample);
            descriptor->setSampleCount(samples);
            // MSAA textures must be GPU-private on Metal.
            descriptor->setStorageMode(MTL::StorageModePrivate);
        }
        else if (arraySize > 1)
        {
            descriptor->setTextureType(MTL::TextureType2DArray);
            descriptor->setArrayLength(arraySize);
        }

        MTL::TextureUsage usage = MTL::TextureUsageShaderRead;
        if (renderTarget)
        {
            usage |= MTL::TextureUsageRenderTarget;
        }
        descriptor->setUsage(usage);

        MTL::Texture* texture = device->newTexture(descriptor);
        EXPECT_NE(texture, nullptr);

        return texture;
    }

    void DestroyTexture(Babylon::Graphics::TextureT texture)
    {
        texture->release();
    }

    std::vector<uint8_t> ReadPixels(const Babylon::Graphics::PlatformInfo& platformInfo, Babylon::Graphics::TextureT texture, uint32_t width, uint32_t height)
    {
        const bool isMsaa = texture->sampleCount() > 1;

        // Staging texture in shared storage so the CPU can read it directly without an explicit
        // synchronizeResource. For MSAA, this also serves as the resolve target.
        MTL::TextureDescriptor* stagingDesc = MTL::TextureDescriptor::texture2DDescriptor(
            texture->pixelFormat(), width, height, false);
        stagingDesc->setStorageMode(MTL::StorageModeShared);
        stagingDesc->setUsage(MTL::TextureUsageRenderTarget | MTL::TextureUsageShaderRead);

        MTL::Texture* staging = platformInfo.Device->newTexture(stagingDesc);
        EXPECT_NE(staging, nullptr);

        // Use bgfx's command queue so this command buffer is serialized after the rendering
        // command buffers bgfx already committed -- avoids needing cross-queue fence/event sync.
        MTL::CommandBuffer* commandBuffer = platformInfo.CommandQueue->commandBuffer();

        if (isMsaa)
        {
            // One-encoder render pass with the MSAA texture as the color attachment and the
            // shared staging texture as the resolve target. storeAction = MultisampleResolve
            // collapses the samples into the staging texture when the encoder ends.
            MTL::RenderPassDescriptor* passDesc = MTL::RenderPassDescriptor::alloc()->init();
            auto* colorAttachment = passDesc->colorAttachments()->object(0);
            colorAttachment->setTexture(texture);
            colorAttachment->setResolveTexture(staging);
            colorAttachment->setLoadAction(MTL::LoadActionLoad);
            colorAttachment->setStoreAction(MTL::StoreActionMultisampleResolve);

            MTL::RenderCommandEncoder* encoder = commandBuffer->renderCommandEncoder(passDesc);
            encoder->endEncoding();

            passDesc->release();
        }
        else
        {
            MTL::BlitCommandEncoder* blit = commandBuffer->blitCommandEncoder();
            blit->copyFromTexture(
                texture, 0, 0,
                MTL::Origin{0, 0, 0},
                MTL::Size{width, height, 1},
                staging, 0, 0,
                MTL::Origin{0, 0, 0});
            blit->endEncoding();
        }

        commandBuffer->commit();
        commandBuffer->waitUntilCompleted();

        const uint32_t bytesPerRow = width * 4;
        std::vector<uint8_t> pixels(bytesPerRow * height);
        MTL::Region region = MTL::Region::Make2D(0, 0, width, height);
        staging->getBytes(pixels.data(), bytesPerRow, region, 0);

        staging->release();

        return pixels;
    }
}
