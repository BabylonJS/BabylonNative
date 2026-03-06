#include <gtest/gtest.h>

#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/Window.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <Babylon/Plugins/ExternalTexture.h>

#include "Utils.h"

#include <iostream>

extern Babylon::Graphics::Configuration g_deviceConfig;

TEST(ExternalTexture, AddToContextAsyncAndUpdateWithLayerIndex)
{
#ifdef SKIP_EXTERNAL_TEXTURE_TESTS
    GTEST_SKIP();
#else
    Babylon::Graphics::Device device{g_deviceConfig};

    Babylon::Graphics::TextureT nativeTexture{};
    std::optional<Babylon::Plugins::ExternalTexture> externalTexture{};
    std::promise<void> promiseResolved{};

    Babylon::AppRuntime runtime{};
    runtime.Dispatch([&device, &promiseResolved, &externalTexture, &nativeTexture](Napi::Env env) {
        device.AddToJavaScript(env);

        Babylon::Polyfills::Console::Initialize(env, [](const char* message, auto) {
            std::cout << message << std::endl;
        });

        Babylon::Polyfills::Window::Initialize(env);

        Babylon::Plugins::NativeEngine::Initialize(env);

        nativeTexture = CreateTestTexture(device.GetPlatformInfo().Device, 256, 256, 3);
        externalTexture.emplace(nativeTexture);

        // Test with explicit layer index 1
        auto jsPromise = externalTexture->AddToContextAsync(env, 1);

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

    // Update the external texture to a new texture with explicit layer index 2.
    externalTexture->Update(nativeTexture, std::nullopt, 2);

    DestroyTestTexture(nativeTexture);

    device.Shutdown();
#endif
}
