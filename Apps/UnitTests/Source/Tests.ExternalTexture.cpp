#include <gtest/gtest.h>

#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/Window.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <Babylon/Plugins/ExternalTexture.h>

#include "Helpers.h"

#include <iostream>

extern Babylon::Graphics::Configuration g_deviceConfig;

TEST(ExternalTexture, Construction)
{
#ifdef SKIP_EXTERNAL_TEXTURE_TESTS
    GTEST_SKIP();
#else
    Babylon::Graphics::Device device{g_deviceConfig};
    Babylon::Graphics::DeviceUpdate update{device.GetUpdate("update")};

    device.StartRenderingCurrentFrame();
    update.Start();

    auto nativeTexture = Helpers::CreateTexture(device.GetPlatformInfo().Device, 256, 256);
    Babylon::Plugins::ExternalTexture externalTexture{nativeTexture};
    Helpers::DestroyTexture(nativeTexture);

    EXPECT_EQ(externalTexture.Width(), 256u);
    EXPECT_EQ(externalTexture.Height(), 256u);

    update.Finish();
    device.FinishRenderingCurrentFrame();
#endif
}

TEST(ExternalTexture, CreateForJavaScript)
{
#ifdef SKIP_EXTERNAL_TEXTURE_TESTS
    GTEST_SKIP();
#else
    Babylon::Graphics::Device device{g_deviceConfig};
    Babylon::Graphics::DeviceUpdate update{device.GetUpdate("update")};

    device.StartRenderingCurrentFrame();
    update.Start();

    auto nativeTexture = Helpers::CreateTexture(device.GetPlatformInfo().Device, 256, 256);
    Babylon::Plugins::ExternalTexture externalTexture{nativeTexture};
    Helpers::DestroyTexture(nativeTexture);

    std::promise<void> done{};

    Babylon::AppRuntime runtime{};
    runtime.Dispatch([&device, &done, externalTexture](Napi::Env env) {
        device.AddToJavaScript(env);

        Babylon::Polyfills::Console::Initialize(env, [](const char* message, auto) {
            std::cout << message << std::endl;
        });

        Babylon::Polyfills::Window::Initialize(env);

        Babylon::Plugins::NativeEngine::Initialize(env);

        auto jsTexture = externalTexture.CreateForJavaScript(env);
        EXPECT_TRUE(jsTexture.IsObject());

        done.set_value();
    });

    done.get_future().wait();

    update.Finish();
    device.FinishRenderingCurrentFrame();
#endif
}

TEST(ExternalTexture, Update)
{
#ifdef SKIP_EXTERNAL_TEXTURE_TESTS
    GTEST_SKIP();
#else
    Babylon::Graphics::Device device{g_deviceConfig};
    Babylon::Graphics::DeviceUpdate update{device.GetUpdate("update")};

    device.StartRenderingCurrentFrame();
    update.Start();

    auto nativeTexture = Helpers::CreateTexture(device.GetPlatformInfo().Device, 256, 256);
    Babylon::Plugins::ExternalTexture externalTexture{nativeTexture};
    Helpers::DestroyTexture(nativeTexture);

    EXPECT_EQ(externalTexture.Width(), 256u);
    EXPECT_EQ(externalTexture.Height(), 256u);

    update.Finish();
    device.FinishRenderingCurrentFrame();

    // Update the external texture to point at a new native texture with different dimensions.
    device.StartRenderingCurrentFrame();
    update.Start();

    auto nativeTexture2 = Helpers::CreateTexture(device.GetPlatformInfo().Device, 128, 128);
    externalTexture.Update(nativeTexture2);
    Helpers::DestroyTexture(nativeTexture2);

    EXPECT_EQ(externalTexture.Width(), 128u);
    EXPECT_EQ(externalTexture.Height(), 128u);

    update.Finish();
    device.FinishRenderingCurrentFrame();
#endif
}

TEST(ExternalTexture, AddToContextAsyncAndUpdate)
{
#ifdef SKIP_EXTERNAL_TEXTURE_TESTS
    GTEST_SKIP();
#else
    Babylon::Graphics::Device device{g_deviceConfig};
    Babylon::Graphics::DeviceUpdate update{device.GetUpdate("update")};

    device.StartRenderingCurrentFrame();
    update.Start();

    auto nativeTexture = Helpers::CreateTexture(device.GetPlatformInfo().Device, 256, 256);
    Babylon::Plugins::ExternalTexture externalTexture{nativeTexture};
    Helpers::DestroyTexture(nativeTexture);

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

        auto jsPromise = externalTexture.AddToContextAsync(env);
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

    // Close the frame in which the deprecated shim's synchronous CreateForJavaScript ran.
    update.Finish();
    device.FinishRenderingCurrentFrame();

    // Wait for promise to resolve.
    promiseResolved.get_future().wait();

    // Start a new frame.
    device.StartRenderingCurrentFrame();
    update.Start();

    // Update the external texture to a new texture.
    auto nativeTexture2 = Helpers::CreateTexture(device.GetPlatformInfo().Device, 256, 256);
    externalTexture.Update(nativeTexture2);
    Helpers::DestroyTexture(nativeTexture2);

    update.Finish();
    device.FinishRenderingCurrentFrame();
#endif
}
