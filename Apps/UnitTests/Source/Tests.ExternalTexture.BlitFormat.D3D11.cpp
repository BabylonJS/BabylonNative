#include <gtest/gtest.h>

#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Graphics/Texture.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/Window.h>
#include <Babylon/Plugins/ExternalTexture.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <napi/pointer.h>

#include <winrt/base.h>

#include <array>
#include <chrono>
#include <future>
#include <iostream>
#include <stdexcept>

extern Babylon::Graphics::Configuration g_deviceConfig;

TEST(ExternalTexture, PreservesNativeFormatForBlits)
{
    Babylon::Graphics::Device device{g_deviceConfig};

    device.StartRenderingCurrentFrame();

    // A typeless resource is the D3D11 way to sample linear storage through an sRGB view.
    D3D11_TEXTURE2D_DESC desc{};
    desc.Width = 256;
    desc.Height = 256;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_TYPELESS;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    winrt::com_ptr<ID3D11Texture2D> nativeTexture;
    ASSERT_HRESULT_SUCCEEDED(device.GetPlatformInfo().Device->CreateTexture2D(&desc, nullptr, nativeTexture.put()));

    Babylon::Plugins::ExternalTexture externalTexture{nativeTexture.get(), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB};
    nativeTexture = nullptr;

    std::promise<void> done{};

    Babylon::AppRuntime runtime{};
    runtime.Dispatch([&device, &done, externalTexture](Napi::Env env) {
        device.AddToJavaScript(env);

        auto jsTexture = externalTexture.CreateForJavaScript(env);
        auto* texture = jsTexture.As<Napi::Pointer<Babylon::Graphics::Texture>>().Get();

        EXPECT_EQ(texture->Format(), bgfx::TextureFormat::RGBA8);
        EXPECT_NE(texture->Flags() & BGFX_TEXTURE_SRGB, 0u);
        EXPECT_EQ(texture->BlitFormat(), bgfx::TextureFormat::RGBA8);
        EXPECT_EQ(texture->BlitFlags() & BGFX_TEXTURE_SRGB, 0u);

        done.set_value();
    });

    done.get_future().wait();

    device.FinishRenderingCurrentFrame();
}

TEST(ExternalTexture, ReadsTypelessR32ThroughFloatOverride)
{
    Babylon::Graphics::Device device{g_deviceConfig};
    device.StartRenderingCurrentFrame();

    constexpr float SOURCE_VALUE{0.25f};
    D3D11_TEXTURE2D_DESC desc{};
    desc.Width = 1;
    desc.Height = 1;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R32_TYPELESS;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA initialData{};
    initialData.pSysMem = &SOURCE_VALUE;
    initialData.SysMemPitch = sizeof(SOURCE_VALUE);

    winrt::com_ptr<ID3D11Texture2D> nativeTexture;
    ASSERT_HRESULT_SUCCEEDED(device.GetPlatformInfo().Device->CreateTexture2D(&desc, &initialData, nativeTexture.put()));

    Babylon::Plugins::ExternalTexture externalTexture{nativeTexture.get(), DXGI_FORMAT_R32_FLOAT};
    nativeTexture = nullptr;

    std::promise<void> readQueued{};
    std::promise<std::array<float, 4>> readCompleted{};

    Babylon::AppRuntime runtime{};
    runtime.Dispatch([&device, &readQueued, &readCompleted, externalTexture](Napi::Env env) {
        try
        {
            env.Global().Set("globalThis", env.Global());
            device.AddToJavaScript(env);
            Babylon::Polyfills::Console::Initialize(env, [](const char* message, auto) {
                std::cout << message << std::endl;
            });
            Babylon::Polyfills::Window::Initialize(env);
            Babylon::Plugins::NativeEngine::Initialize(env);

            auto jsTexture = externalTexture.CreateForJavaScript(env);
            auto* texture = jsTexture.As<Napi::Pointer<Babylon::Graphics::Texture>>().Get();
            EXPECT_EQ(texture->Format(), bgfx::TextureFormat::R32F);
            EXPECT_EQ(texture->BlitFormat(), bgfx::TextureFormat::R32F);

            env.Global().Set("__externalTexture", jsTexture);
            auto promise = Napi::Eval(env, R"(
                (function () {
                    var engine = new _native.Engine();
                    var output = new ArrayBuffer(16);
                    return engine.readTexture2(__externalTexture, 0, 0, 0, 1, 1, output, 0, 16, -1)
                        .then(function (buffer) {
                            engine.dispose();
                            return buffer;
                        });
                })()
            )", "external-texture-readback-test.js").As<Napi::Promise>();

            auto onFulfilled = Napi::Function::New(env, [&readCompleted](const Napi::CallbackInfo& info) {
                const auto buffer = info[0].As<Napi::ArrayBuffer>();
                if (buffer.ByteLength() != sizeof(float) * 4)
                {
                    readCompleted.set_exception(std::make_exception_ptr(std::runtime_error{"Unexpected readback size"}));
                    return;
                }

                const auto* values = static_cast<const float*>(buffer.Data());
                readCompleted.set_value({values[0], values[1], values[2], values[3]});
            });
            auto onRejected = Napi::Function::New(env, [&readCompleted](const Napi::CallbackInfo& info) {
                readCompleted.set_exception(std::make_exception_ptr(
                    std::runtime_error{Napi::GetErrorString(info[0].As<Napi::Error>())}));
            });
            promise.Get("then").As<Napi::Function>().Call(promise, {onFulfilled, onRejected});
            readQueued.set_value();
        }
        catch (...)
        {
            readQueued.set_exception(std::current_exception());
        }
    });

    ASSERT_NO_THROW(readQueued.get_future().get());

    auto readFuture = readCompleted.get_future();
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds{30};
    while (readFuture.wait_for(std::chrono::milliseconds{16}) != std::future_status::ready &&
        std::chrono::steady_clock::now() < deadline)
    {
        device.FinishRenderingCurrentFrame();
        device.StartRenderingCurrentFrame();
    }
    device.FinishRenderingCurrentFrame();

    ASSERT_EQ(readFuture.wait_for(std::chrono::seconds{0}), std::future_status::ready);
    const auto values = readFuture.get();
    EXPECT_FLOAT_EQ(values[0], SOURCE_VALUE);
    EXPECT_FLOAT_EQ(values[1], 0.0f);
    EXPECT_FLOAT_EQ(values[2], 0.0f);
    EXPECT_FLOAT_EQ(values[3], 1.0f);
}
