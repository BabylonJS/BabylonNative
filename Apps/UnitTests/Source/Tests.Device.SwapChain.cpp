#include <gtest/gtest.h>

#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Graphics/DeviceContext.h>
#include <Babylon/Graphics/FrameBuffer.h>
#include <gsl/util>

#include <array>
#include <atomic>
#include <future>
#include <memory>
#include <optional>

#ifdef _WIN32
#include <Windows.h>
#endif
#if defined(_WIN32) && defined(BABYLON_NATIVE_GRAPHICS_API_D3D12)
#include <d3d12sdklayers.h>
#include <winrt/base.h>
#endif

extern Babylon::Graphics::Configuration g_deviceConfig;

#if !defined(USE_NOOP_METAL_DEVICE)
namespace
{
    Babylon::Graphics::DeviceContext& GetContext(Babylon::Graphics::Device& device, Babylon::AppRuntime& runtime)
    {
        std::promise<Babylon::Graphics::DeviceContext*> result;
        auto future = result.get_future();
        runtime.Dispatch([&](Napi::Env env) {
            device.AddToJavaScript(env);
            result.set_value(&Babylon::Graphics::DeviceContext::GetFromJavaScript(env));
        });
        return *future.get();
    }

    std::vector<uint8_t> ClearAndCapture(
        Babylon::Graphics::Device& device, Babylon::Graphics::DeviceContext& context, uint32_t color)
    {
        auto captured = std::make_shared<std::optional<std::vector<uint8_t>>>();
        context.RequestScreenShot([captured](auto pixels) { captured->emplace(std::move(pixels)); });
        for (size_t frame = 0; frame < 3 && !captured->has_value(); ++frame)
        {
            device.StartRenderingCurrentFrame();
            Babylon::Graphics::FrameBuffer backBuffer{context, BGFX_INVALID_HANDLE, 0, 0, true, true, true};
            backBuffer.Clear(*context.GetActiveEncoder(), BGFX_CLEAR_COLOR, color, 1.0f, 0);
            device.FinishRenderingCurrentFrame();
        }
        if (!captured->has_value())
        {
            throw std::runtime_error{"Window screenshot did not complete within three frames."};
        }
        return std::move(captured->value());
    }

    void ExpectSolidColor(const std::vector<uint8_t>& pixels, uint32_t width, uint32_t height, uint32_t color)
    {
        ASSERT_EQ(pixels.size(), static_cast<size_t>(width) * height * 4);
        const std::array<uint8_t, 4> expected{
            static_cast<uint8_t>(color >> 24), static_cast<uint8_t>(color >> 16),
            static_cast<uint8_t>(color >> 8), static_cast<uint8_t>(color)};
        for (size_t i = 0; i < pixels.size(); ++i)
        {
            if (pixels[i] != expected[i % 4])
            {
                ADD_FAILURE() << "Unexpected channel at byte " << i << ": " << unsigned(pixels[i])
                              << ", expected " << unsigned(expected[i % 4]);
                break;
            }
        }
    }
}

TEST(Device, SwapChainResizeAndMsaaPreserveCapture)
{
    for (auto depth : {Babylon::Graphics::DepthStencilFormat::None,
                      Babylon::Graphics::DepthStencilFormat::Depth32,
                      Babylon::Graphics::DepthStencilFormat::Depth24Stencil8})
    {
        auto config = g_deviceConfig;
        config.Width = 64;
        config.Height = 48;
        config.BackBufferDepthStencilFormat = depth;
        Babylon::Graphics::Device device{config};
        Babylon::AppRuntime runtime{};
        auto& context = GetContext(device, runtime);
        ExpectSolidColor(ClearAndCapture(device, context, 0x2050a0ff), 64, 48, 0x2050a0ff);
        const auto handle = context.GetBackBufferHandle();
        ASSERT_TRUE(bgfx::isValid(handle));

        for (uint8_t samples : std::array<uint8_t, 4>{1, 2, 4, 1})
        {
            device.UpdateSize(32, 24);
            device.UpdateMSAA(samples);
            device.StartRenderingCurrentFrame();
            device.FinishRenderingCurrentFrame();
            EXPECT_EQ(context.GetBackBufferHandle().idx, handle.idx);
            ExpectSolidColor(ClearAndCapture(device, context, 0x4080c0ff), 32, 24, 0x4080c0ff);
        }
    }
}

TEST(Device, SwapChainIsRecreatedAfterRenderingIsReenabled)
{
    auto config = g_deviceConfig;
    config.Width = 32;
    config.Height = 24;
    Babylon::Graphics::Device device{config};
    Babylon::AppRuntime runtime{};
    auto& context = GetContext(device, runtime);
    ExpectSolidColor(ClearAndCapture(device, context, 0xff0000ff), 32, 24, 0xff0000ff);
    const auto id = context.GetDeviceId();
    device.DisableRendering();
    ExpectSolidColor(ClearAndCapture(device, context, 0x00ff00ff), 32, 24, 0x00ff00ff);
    EXPECT_NE(context.GetDeviceId(), id);
}

#if defined(_WIN32) && defined(BABYLON_NATIVE_GRAPHICS_API_D3D12) && defined(__ID3D12InfoQueue1_INTERFACE_DEFINED__)
namespace
{
    void WINAPI CountD3D12Errors(D3D12_MESSAGE_CATEGORY, D3D12_MESSAGE_SEVERITY severity,
        D3D12_MESSAGE_ID, LPCSTR, void* context)
    {
        if (severity == D3D12_MESSAGE_SEVERITY_ERROR || severity == D3D12_MESSAGE_SEVERITY_CORRUPTION)
        {
            static_cast<std::atomic_uint32_t*>(context)->fetch_add(1, std::memory_order_relaxed);
        }
    }
}

TEST(Device, SwapChainMsaaUsesValidResourceStates)
{
    auto config = g_deviceConfig;
    config.Width = 32;
    config.Height = 24;
    config.MSAASamples = 1;
    Babylon::Graphics::Device device{config};
    Babylon::AppRuntime runtime{};
    auto& context = GetContext(device, runtime);
    ExpectSolidColor(ClearAndCapture(device, context, 0x4080c0ff), 32, 24, 0x4080c0ff);
    winrt::com_ptr<ID3D12InfoQueue1> infoQueue;
    const auto result = device.GetPlatformInfo().Device->QueryInterface(IID_PPV_ARGS(infoQueue.put()));
    if (FAILED(result))
    {
        GTEST_SKIP() << "D3D12 debug-layer callbacks are unavailable: " << result;
    }

    std::atomic_uint32_t errors{};
    DWORD cookie{};
    winrt::check_hresult(infoQueue->RegisterMessageCallback(
        CountD3D12Errors, D3D12_MESSAGE_CALLBACK_FLAG_NONE, &errors, &cookie));
    const auto unregister = gsl::finally([&] { infoQueue->UnregisterMessageCallback(cookie); });

    for (uint8_t samples : std::array<uint8_t, 4>{4, 1, 2, 4})
    {
        device.UpdateMSAA(samples);
        device.StartRenderingCurrentFrame();
        device.FinishRenderingCurrentFrame();
        ExpectSolidColor(ClearAndCapture(device, context, 0x4080c0ff), 32, 24, 0x4080c0ff);
    }
    EXPECT_EQ(errors.load(std::memory_order_relaxed), 0u);
}
#endif

#ifdef _WIN32
TEST(Device, ReplacingWindowPreservesDeviceAndTargetsNewSurface)
{
    HWND first = CreateWindowExW(0, L"STATIC", L"First surface", WS_POPUP, 0, 0, 64, 48, nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);
    HWND second = CreateWindowExW(0, L"STATIC", L"Second surface", WS_POPUP, 0, 0, 64, 48, nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);
    const auto cleanup = gsl::finally([&] {
        if (first)
        {
            DestroyWindow(first);
        }
        if (second)
        {
            DestroyWindow(second);
        }
    });
    ASSERT_NE(first, nullptr);
    ASSERT_NE(second, nullptr);

    auto config = g_deviceConfig;
    config.Window = first;
    config.Width = 32;
    config.Height = 24;
    Babylon::Graphics::Device device{config};
    Babylon::AppRuntime runtime{};
    auto& context = GetContext(device, runtime);
    ExpectSolidColor(ClearAndCapture(device, context, 0xff0000ff), 32, 24, 0xff0000ff);
    const auto id = context.GetDeviceId();

    device.UpdateWindow(second);
    device.UpdateSize(48, 36);
    device.StartRenderingCurrentFrame();
    device.FinishRenderingCurrentFrame();
    ASSERT_TRUE(DestroyWindow(first));
    first = nullptr;

    EXPECT_EQ(context.GetDeviceId(), id);
    ExpectSolidColor(ClearAndCapture(device, context, 0x0000ffff), 48, 36, 0x0000ffff);
}
#endif
#endif
