#include <gtest/gtest.h>

#include <Babylon/Graphics/Device.h>

#include "Utils.h"

extern Babylon::Graphics::Configuration g_deviceConfig;

// Demonstrates the proper sequence for swapping the underlying graphics device at runtime.
//
// Motivating use cases:
//   - D3D11/D3D12 device-removed recovery (DXGI_ERROR_DEVICE_REMOVED on Present, GPU TDR, hot
//     unplug). The app detects the removal, creates a fresh device, and swaps it in.
//   - Voluntary GPU swap on Metal multi-GPU machines (integrated <-> discrete, eGPU connect /
//     disconnect, multi-display compositing).
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
// Per-platform notes:
//   D3D11   -> back-buffer textures are owned by a specific ID3D11Device, so UpdateBackBuffer must
//              accompany UpdateDevice. UpdateDevice and UpdateBackBuffer form a single logical swap
//              and must NOT be separated by rendering.
//   D3D12   -> SKIPPED at runtime (see GRAPHICS_API_D3D12 guard below). The bgfx D3D12 backend
//              hits a fatal lifecycle issue when the caller provides their own ID3D12Device:
//              bgfx::shutdown unconditionally enforces that the device's COM refcount drops to 0,
//              but with a caller-provided device the caller still holds a reference, so shutdown
//              terminates the process. The same ref-count assumption is also flagged in
//              App.Win32.cpp (the App-layer workaround creates an HWND so bgfx can manage the
//              device internally). Until that bgfx limitation is addressed, exercising
//              DisableRendering -> UpdateDevice -> re-init on D3D12 with a caller-provided device
//              is not feasible from a unit test.
//   Metal   -> MTL::Device is a per-GPU singleton; MTL::CreateSystemDefaultDevice returns the same
//              handle on successive calls for the same physical GPU. There is no Metal API that
//              creates a second device for the same GPU. On single-GPU machines (typical Apple
//              Silicon dev hardware and any macOS CI runner where Metal is exposed at all)
//              deviceA == deviceB; the test still exercises the bgfx teardown/re-init path but the
//              EXPECT_EQ on the post-swap PlatformInfo.Device passes for the same reason as before
//              the swap, not because a different device was actually adopted. Distinct-device
//              coverage requires running on multi-GPU hardware. Additionally, GitHub Actions macOS
//              runners are headless VMs without a real Metal device; the BabylonNative CI
//              workflow forces the bgfx noop renderer via BABYLON_NATIVE_TESTS_USE_NOOP_METAL_DEVICE,
//              which defines USE_NOOP_METAL_DEVICE here -- the test is skipped in that environment.
//   OpenGL  -> the bgfx GL backend production path on Linux passes pd.context = nullptr and lets
//              bgfx own the GLX context. This test exercises the alternate caller-provided-context
//              path. If the test environment can't construct a GLX context (e.g. Mesa software
//              rasterizer without the requested visual), the helper returns nullptr and the test
//              skips.
//
// Cleanup order:
//   The Babylon::Graphics::Device destructor calls DisableRendering internally, which still expects
//   the active back-buffer and graphics device to be valid. The Device is therefore scoped so it
//   destructs before any caller-owned device / back-buffer handles are released.
TEST(Device, UpdateDevice)
{
#ifdef GRAPHICS_API_D3D12
    GTEST_SKIP() << "Skipping: bgfx D3D12 backend asserts a zero device refcount on shutdown, which "
                    "the caller-provided ID3D12Device pattern cannot satisfy. See file header.";
#elif defined(USE_NOOP_METAL_DEVICE)
    GTEST_SKIP() << "Skipping: BABYLON_NATIVE_TESTS_USE_NOOP_METAL_DEVICE is enabled, so the bgfx "
                    "noop renderer is in use and there is no real Metal device to exercise.";
#else
    Babylon::Graphics::DeviceT deviceA = CreateGraphicsDeviceForTest();
    if (deviceA == nullptr)
    {
        GTEST_SKIP() << "Skipping: graphics device creation failed in this test environment.";
    }

#ifdef GRAPHICS_BACK_BUFFER_SUPPORT
    const uint32_t backBufferWidth = g_deviceConfig.Width != 0 ? static_cast<uint32_t>(g_deviceConfig.Width) : 1280u;
    const uint32_t backBufferHeight = g_deviceConfig.Height != 0 ? static_cast<uint32_t>(g_deviceConfig.Height) : 720u;
    Babylon::Graphics::BackBufferColorT backBufferA = CreateBackBufferForTest(deviceA, backBufferWidth, backBufferHeight);
#endif

    Babylon::Graphics::DeviceT deviceB = nullptr;
#ifdef GRAPHICS_BACK_BUFFER_SUPPORT
    Babylon::Graphics::BackBufferColorT backBufferB = nullptr;
#endif

    {
        // Inherit Window / Width / Height (and on Apple any platform-default Device) from the App
        // layer's config -- bgfx GL / D3D12 backends need a real window handle for their swap chain.
        Babylon::Graphics::Configuration config = g_deviceConfig;
        config.Device = deviceA;
#ifdef GRAPHICS_BACK_BUFFER_SUPPORT
        config.BackBufferColor = backBufferA;
        config.Width = backBufferWidth;
        config.Height = backBufferHeight;
#endif

        Babylon::Graphics::Device device{config};

        // Drive a frame to force EnableRendering -> bgfx::init with deviceA.
        device.StartRenderingCurrentFrame();
        device.FinishRenderingCurrentFrame();

        EXPECT_EQ(device.GetPlatformInfo().Device, deviceA);

        deviceB = CreateGraphicsDeviceForTest();
        ASSERT_NE(deviceB, nullptr);

#ifdef GRAPHICS_BACK_BUFFER_SUPPORT
        backBufferB = CreateBackBufferForTest(deviceB, backBufferWidth, backBufferHeight);
#endif

        // Tear bgfx down before pointing the device at the new graphics device.
        device.DisableRendering();

        // UpdateDevice + UpdateBackBuffer (on D3D11) form a single logical swap: do not start a
        // frame between these calls or the temporary mismatched back-buffer/device state would be
        // visible to bgfx::init.
        device.UpdateDevice(deviceB);
#ifdef GRAPHICS_BACK_BUFFER_SUPPORT
        device.UpdateBackBuffer(backBufferB);
#endif

        // Drive another frame to force EnableRendering -> bgfx::init with deviceB.
        device.StartRenderingCurrentFrame();
        device.FinishRenderingCurrentFrame();

        // On Metal single-GPU hardware deviceA == deviceB, so this assertion is also satisfied for
        // the trivial reason that nothing changed. On all other platforms (and on multi-GPU Metal)
        // it proves the new device was adopted.
        EXPECT_EQ(device.GetPlatformInfo().Device, deviceB);
    }   // Babylon::Graphics::Device destructs here, calling DisableRendering on deviceB / backBufferB.

#ifdef GRAPHICS_BACK_BUFFER_SUPPORT
    DestroyBackBufferForTest(backBufferB);
    DestroyBackBufferForTest(backBufferA);
#endif
    DestroyGraphicsDeviceForTest(deviceB);
    DestroyGraphicsDeviceForTest(deviceA);
#endif
}
