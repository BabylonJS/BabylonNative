#include <gtest/gtest.h>

#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/Window.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <Babylon/Plugins/ExternalTexture.h>

#include "Utils.h"

#include <iostream>
#include <optional>

extern Babylon::Graphics::Configuration g_deviceConfig;

TEST(ExternalTexture, Construction)
{
#ifdef SKIP_EXTERNAL_TEXTURE_TESTS
    GTEST_SKIP();
#else
    Babylon::Graphics::Device device{g_deviceConfig};

    device.EnableRendering();

	{ // scope to ensure destruction of externalTexture before device.Shutdown()
        auto nativeTexture = CreateTestTexture(device.GetPlatformInfo().Device, 256, 256);
        Babylon::Plugins::ExternalTexture externalTexture{nativeTexture};
        DestroyTestTexture(nativeTexture);

		EXPECT_EQ(externalTexture.Width(), 256u);
		EXPECT_EQ(externalTexture.Height(), 256u);
    }

    device.DisableRendering();
#endif
}

TEST(ExternalTexture, AddToContextAsyncAndUpdate)
{
#ifdef SKIP_EXTERNAL_TEXTURE_TESTS
    GTEST_SKIP();
#else
    Babylon::Graphics::Device device{g_deviceConfig};

    std::optional<Babylon::Plugins::ExternalTexture> externalTexture{};
    std::promise<void> promiseResolved{};

    Babylon::AppRuntime runtime{};
    runtime.Dispatch([&device, &promiseResolved, &externalTexture](Napi::Env env) {
        device.AddToJavaScript(env);

        Babylon::Polyfills::Console::Initialize(env, [](const char* message, auto) {
            std::cout << message << std::endl;
        });

        Babylon::Polyfills::Window::Initialize(env);

        Babylon::Plugins::NativeEngine::Initialize(env);

        auto nativeTexture = CreateTestTexture(device.GetPlatformInfo().Device, 256, 256);
        externalTexture.emplace(nativeTexture);
        DestroyTestTexture(nativeTexture);

        auto jsPromise = externalTexture->AddToContextAsync(env);

        auto jsOnFulfilled = Napi::Function::New(env, [&promiseResolved](const Napi::CallbackInfo& info) {
            promiseResolved.set_value();
        });

        auto jsOnRejected = Napi::Function::New(env, [&promiseResolved](const Napi::CallbackInfo& info) {
            promiseResolved.set_exception(std::make_exception_ptr(info[0].As<Napi::Error>()));
        });

        jsPromise.Get("then").As<Napi::Function>().Call(jsPromise, {jsOnFulfilled, jsOnRejected});
    });

    // Pump RenderFrame() on the test thread (render thread) until
    // the AddToContextAsync promise resolves.
    auto promiseResolvedFuture = promiseResolved.get_future();
    while (promiseResolvedFuture.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready)
    {
        device.RenderFrame();
    }

    promiseResolvedFuture.get();

    // Update the external texture to a new texture.
    auto nativeTexture2 = CreateTestTexture(device.GetPlatformInfo().Device, 256, 256);
    externalTexture->Update(nativeTexture2);
    DestroyTestTexture(nativeTexture2);

    device.Shutdown();
#endif
}
