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

Babylon::Graphics::DeviceT CreateGraphicsDeviceForTest()
{
    // Metal does not expose a "create a fresh device for the same GPU" API. MTL::Device is a
    // per-GPU singleton: every call to MTL::CreateSystemDefaultDevice (or any entry of
    // MTL::CopyAllDevices) for the same physical GPU returns a reference to the same logical
    // device. On single-GPU machines (typical Apple Silicon Macs and most macOS CI runners that
    // expose Metal at all) two successive calls therefore return the same MTL::Device handle.
    //
    // For the UpdateDevice test this means the "swap from device A to device B" assertion only
    // proves a different device was actually adopted on multi-GPU hardware (Intel Mac with discrete
    // GPU, eGPU attached, etc.). On single-GPU hardware the test still exercises the bgfx
    // teardown/re-init path but the post-swap PlatformInfo.Device equals the pre-swap one because
    // there is only one Metal device to swap between.
    return MTL::CreateSystemDefaultDevice();
}

void DestroyGraphicsDeviceForTest(Babylon::Graphics::DeviceT device)
{
    if (device != nullptr)
    {
        device->release();
    }
}
