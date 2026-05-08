#include <gtest/gtest.h>

#include <Babylon/Graphics/Device.h>

#include "Utils.h"

extern Babylon::Graphics::Configuration g_deviceConfig;

// Exercises the canonical UpdateDevice flow: drive a frame, DisableRendering, UpdateDevice to a
// new graphics device, drive another frame, and verify the active device pointer reflects the swap.
// The motivating use cases are D3D11/D3D12 device-removed recovery (DXGI_ERROR_DEVICE_REMOVED on
// Present, GPU TDR, hot unplug) and host-driven device swaps in embedding scenarios (e.g. WPF/Win32
// interop where the host's ID3D11Device changes).
//
// Idle requirement (not exercised here, but required in production):
//   DisableRendering must be called between frames and with no outstanding JS / FrameCompletionScope
//   work. This test runs purely on the render thread with no JS engine attached, so the requirement
//   is trivially satisfied.
//
// Per-platform scope:
//   D3D11 / D3D12 -> the test runs using a bgfx-managed swap chain bound to the App-layer HWND.
//                    Note: D3D12CreateDevice(WARP) returns the same singleton pointer on
//                    successive calls (deviceA and deviceB compare equal), so post-swap
//                    distinctness cannot be asserted on D3D12 + WARP. The test still exercises the
//                    full teardown / re-init path. D3D11 (D3D11CreateDevice(WARP)) does return
//                    distinct pointers per call.
//   Metal / OpenGL -> the test is not built on these platforms (see Apps/UnitTests/CMakeLists.txt).
//                     Metal does not expose an API to create distinct devices for the same GPU
//                     (MTL::Device is a per-GPU singleton), and the OpenGL backend does not
//                     support a caller-provided GLXContext at init. The shipping code paths on
//                     those backends always use backend-owned contexts.
//
// The test does not exercise the caller-owned BackBufferColor path; it lets the swap chain be
// managed on the App layer's HWND so the test focuses purely on UpdateDevice. The caller-owned
// BackBuffer flow is covered by TEST(Device, BackBuffer) in Tests.Device.D3D11.cpp.
//
// Cleanup order:
//   The Babylon::Graphics::Device destructor calls DisableRendering internally, which still expects
//   the active graphics device to be valid. The Device is therefore scoped so it destructs before
//   any caller-owned device handles are released.
TEST(Device, UpdateDevice)
{
    Babylon::Graphics::DeviceT deviceA = CreateTestGraphicsDevice();
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

        deviceB = CreateTestGraphicsDevice();
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

    DestroyTestGraphicsDevice(deviceB);
    DestroyTestGraphicsDevice(deviceA);
}
