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
    Babylon::Graphics::DeviceUpdate update{device.GetUpdate("update")};

    device.StartRenderingCurrentFrame();
    update.Start();

    auto nativeTexture = CreateTestTexture(device.GetPlatformInfo().Device, 256, 256, 3);

    Babylon::Plugins::ExternalTexture externalTexture{nativeTexture};

    std::promise<void> addToContext{};
    std::promise<void> promiseResolved{};

    Babylon::AppRuntime runtime{};
    runtime.Dispatch([&device, &addToContext, &promiseResolved, externalTexture](Napi::Env env) {
        device.AddToJavaScript(env);

        Babylon::Polyfills::Console::Initialize(env, [](const char* message, auto) {
            std::cout << message << std::endl;
        });

        Babylon::Polyfills::Window::Initialize(env);

        Babylon::Plugins::NativeEngine::Initialize(env);

        // Test with explicit layer index 1
        auto jsPromise = externalTexture.AddToContextAsync(env, 1);
        addToContext.set_value();

        auto jsOnFulfilled = Napi::Function::New(env, [&promiseResolved](const Napi::CallbackInfo& info) {
            promiseResolved.set_value();
        });

        auto jsOnRejected = Napi::Function::New(env, [&promiseResolved](const Napi::CallbackInfo& info) {
            promiseResolved.set_exception(std::make_exception_ptr(info[0].As<Napi::Error>()));
        });

        jsPromise.Get("then").As<Napi::Function>().Call(jsPromise, {jsOnFulfilled, jsOnRejected});
    });

    // Wait for AddToContextAsync to be called.
    addToContext.get_future().wait();

    // Render a frame so that AddToContextAsync will complete.
    update.Finish();
    device.FinishRenderingCurrentFrame();

    // Wait for promise to resolve.
    promiseResolved.get_future().wait();

    // Start a new frame.
    device.StartRenderingCurrentFrame();
    update.Start();

    // Update the external texture to a new texture with explicit layer index 2.
    externalTexture.Update(nativeTexture, std::nullopt, 2);

    DestroyTestTexture(nativeTexture);

    update.Finish();
    device.FinishRenderingCurrentFrame();
#endif
}
