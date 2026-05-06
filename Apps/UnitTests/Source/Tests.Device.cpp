#include <gtest/gtest.h>

#include <Babylon/Graphics/Device.h>

#include "Utils.h"

extern Babylon::Graphics::Configuration g_deviceConfig;

// Demonstrates the proper sequence for swapping the underlying graphics device at runtime.
//
// Motivating use cases:
//   - D3D11/D3D12 device-removed recovery (DXGI_ERROR_DEVICE_REMOVED on Present, GPU TDR, hot
//     unplug). The app detects the removal, creates a fresh device, and swaps it in.
//   - Host-driven device swap in embedding scenarios (e.g. WPF/Win32 interop where the host's
//     ID3D11Device changes).
//
// Mechanics:
//   UpdateDevice on its own only stores the new device pointer in Bgfx.InitState.platformData.
//   While bgfx is already initialized, EnableRendering (called from StartRenderingCurrentFrame)
//   early-outs because Bgfx.Initialized == true, so the new pointer never reaches bgfx::init.
//   DisableRendering must be called first to shut bgfx down so the next frame re-runs bgfx::init
//   with the new device. The render-reset callback (wired by NativeEngine to the JS-side
//   setDeviceLostCallback) fires automatically as part of that second EnableRendering, allowing
//   JS to rebuild GPU resources on the new device.
//
// Quiescence requirement (not exercised here, but required in production):
//   DisableRendering must be called between frames and with no outstanding JS / FrameCompletionScope
//   work. This test runs purely on the render thread with no JS engine attached, so the requirement
//   is trivially satisfied.
//
// Production note for D3D11 callers using a caller-owned BackBufferColor:
//   When the caller provides Configuration::BackBufferColor (an ID3D11RenderTargetView) and then
//   swaps to a new ID3D11Device, the existing RTV is bound to the old device and must be replaced
//   with one created on the new device via UpdateBackBuffer. This test does not exercise that
//   path -- it lets bgfx manage its own swap chain on the App layer's HWND so the test focuses
//   purely on the UpdateDevice contract. The caller-owned BackBuffer flow is covered by
//   TEST(Device, BackBuffer) in Tests.Device.D3D11.cpp.
//
// Per-platform scope:
//   D3D11 / D3D12 -> the test runs using bgfx-managed swap chain bound to the App-layer HWND.
//                    On both backends bgfx::init / bgfx::shutdown handle a caller-provided
//                    ID3D11Device or ID3D12Device cleanly through the
//                    EnableRendering -> DisableRendering -> EnableRendering lifecycle.
//                    Note: D3D12CreateDevice(WARP) returns the same singleton pointer on
//                    successive calls (deviceA and deviceB compare equal), so post-swap distinctness
//                    cannot be asserted on D3D12 + WARP. The test still exercises the full bgfx
//                    teardown / re-init path -- bgfx::shutdown actually releases the command queue,
//                    descriptor heaps, root signatures, etc. and bgfx::init recreates them, even
//                    when the underlying ID3D12Device pointer happens to be the same. D3D11
//                    (D3D11CreateDevice(WARP)) does return distinct pointers per call.
//   Metal / OpenGL -> the test is not built on these platforms (see Apps/UnitTests/CMakeLists.txt).
//                     Metal does not expose an API to create distinct devices for the same GPU
//                     (MTL::Device is a per-GPU singleton), and the bgfx OpenGL backend does not
//                     support a caller-provided GLXContext through bgfx::init. The shipping code
//                     paths on those backends always use bgfx-owned contexts.
//
// Cleanup order:
//   The Babylon::Graphics::Device destructor calls DisableRendering internally, which still expects
//   the active graphics device to be valid. The Device is therefore scoped so it destructs before
//   any caller-owned device handles are released.
TEST(Device, UpdateDevice)
{
    Babylon::Graphics::DeviceT deviceA = CreateGraphicsDeviceForTest();
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

        deviceB = CreateGraphicsDeviceForTest();
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

    DestroyGraphicsDeviceForTest(deviceB);
    DestroyGraphicsDeviceForTest(deviceA);
}
