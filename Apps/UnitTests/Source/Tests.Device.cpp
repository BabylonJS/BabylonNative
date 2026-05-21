#include <gtest/gtest.h>

#include <Babylon/Graphics/Device.h>

#include "Helpers.h"

extern Babylon::Graphics::Configuration g_deviceConfig;

// Verifies UpdateDevice replaces the active graphics device after a DisableRendering / EnableRendering cycle.
TEST(Device, UpdateDevice)
{
    Babylon::Graphics::DeviceT deviceA = Helpers::CreateTestGraphicsDevice();
    ASSERT_NE(deviceA, nullptr);

    Babylon::Graphics::DeviceT deviceB = nullptr;

    {
        // Inherit Window / Width / Height from the App layer's config so bgfx can manage its own
        // swap chain on the HWND. We deliberately do NOT set BackBufferColor here -- the
        // caller-provided-BackBuffer flow is a separate concern covered by TEST(Device, BackBuffer).
        Babylon::Graphics::Configuration config = g_deviceConfig;
        config.Device = deviceA;

        Babylon::Graphics::Device device{config};

        // Drive a frame to force EnableRendering -> bgfx::init with deviceA.
        device.StartRenderingCurrentFrame();
        device.FinishRenderingCurrentFrame();

        EXPECT_EQ(device.GetPlatformInfo().Device, deviceA);

        deviceB = Helpers::CreateTestGraphicsDevice();
        ASSERT_NE(deviceB, nullptr);

        // Tear bgfx down before pointing the device at the new graphics device.
        device.DisableRendering();

        device.UpdateDevice(deviceB);

        // Drive another frame to force EnableRendering -> bgfx::init with deviceB.
        device.StartRenderingCurrentFrame();
        device.FinishRenderingCurrentFrame();

        EXPECT_EQ(device.GetPlatformInfo().Device, deviceB);
        // Note: no EXPECT_NE(deviceB, deviceA). On D3D12 with WARP, D3D12CreateDevice returns the
        // same singleton pointer on successive calls so distinctness is not assertable. On D3D11
        // distinctness holds but exercising it does not add value over the EXPECT_EQ above.
    }   // Babylon::Graphics::Device destructs here, calling DisableRendering on deviceB.

    Helpers::DestroyTestGraphicsDevice(deviceB);
    Helpers::DestroyTestGraphicsDevice(deviceA);
}
