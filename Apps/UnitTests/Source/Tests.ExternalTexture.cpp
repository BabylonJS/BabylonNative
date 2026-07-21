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

    device.StartRenderingCurrentFrame();

    auto nativeTexture = Helpers::CreateTexture(device.GetPlatformInfo().Device, 256, 256);
    Babylon::Plugins::ExternalTexture externalTexture{nativeTexture, {}, 256, 256};

    EXPECT_EQ(externalTexture.Width(), 256u);
    EXPECT_EQ(externalTexture.Height(), 256u);

    device.FinishRenderingCurrentFrame();

    // The OpenGL backend does not take ownership of the handle, so the source texture must
    // outlive the ExternalTexture (see ExternalTexture.h). Destroy it only now.
    Helpers::DestroyTexture(nativeTexture);
#endif
}

TEST(ExternalTexture, CreateForJavaScript)
{
#ifdef SKIP_EXTERNAL_TEXTURE_TESTS
    GTEST_SKIP();
#else
    Babylon::Graphics::Device device{g_deviceConfig};

    device.StartRenderingCurrentFrame();

    auto nativeTexture = Helpers::CreateTexture(device.GetPlatformInfo().Device, 256, 256);
    Babylon::Plugins::ExternalTexture externalTexture{nativeTexture, {}, 256, 256};

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

    device.FinishRenderingCurrentFrame();

    // The GL backend doesn't own the handle; destroy the source texture only after bgfx has
    // finished using it (see ExternalTexture.h).
    Helpers::DestroyTexture(nativeTexture);
#endif
}

TEST(ExternalTexture, Update)
{
#ifdef SKIP_EXTERNAL_TEXTURE_TESTS
    GTEST_SKIP();
#else
    Babylon::Graphics::Device device{g_deviceConfig};

    device.StartRenderingCurrentFrame();

    auto nativeTexture = Helpers::CreateTexture(device.GetPlatformInfo().Device, 256, 256);
    Babylon::Plugins::ExternalTexture externalTexture{nativeTexture, {}, 256, 256};

    EXPECT_EQ(externalTexture.Width(), 256u);
    EXPECT_EQ(externalTexture.Height(), 256u);

    device.FinishRenderingCurrentFrame();

    // Update the external texture to point at a new native texture with different dimensions.
    device.StartRenderingCurrentFrame();

    auto nativeTexture2 = Helpers::CreateTexture(device.GetPlatformInfo().Device, 128, 128);
    externalTexture.Update(nativeTexture2, {}, {}, 128, 128);

    EXPECT_EQ(externalTexture.Width(), 128u);
    EXPECT_EQ(externalTexture.Height(), 128u);

    device.FinishRenderingCurrentFrame();

    // The GL backend doesn't own these handles; destroy them only after all rendering that
    // referenced them has completed (see ExternalTexture.h).
    Helpers::DestroyTexture(nativeTexture);
    Helpers::DestroyTexture(nativeTexture2);
#endif
}

TEST(ExternalTexture, AddToContextAsyncAndUpdate)
{
#ifdef SKIP_EXTERNAL_TEXTURE_TESTS
    GTEST_SKIP();
#else
    Babylon::Graphics::Device device{g_deviceConfig};

    device.StartRenderingCurrentFrame();

    auto nativeTexture = Helpers::CreateTexture(device.GetPlatformInfo().Device, 256, 256);
    Babylon::Plugins::ExternalTexture externalTexture{nativeTexture, {}, 256, 256};

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
    device.FinishRenderingCurrentFrame();

    // Wait for promise to resolve.
    promiseResolved.get_future().wait();

    // Start a new frame.
    device.StartRenderingCurrentFrame();

    // Update the external texture to a new texture.
    auto nativeTexture2 = Helpers::CreateTexture(device.GetPlatformInfo().Device, 256, 256);
    externalTexture.Update(nativeTexture2, {}, {}, 256, 256);

    device.FinishRenderingCurrentFrame();

    // The GL backend doesn't own these handles; destroy them only after all frames that
    // referenced them have completed (see ExternalTexture.h).
    Helpers::DestroyTexture(nativeTexture);
    Helpers::DestroyTexture(nativeTexture2);
#endif
}

TEST(ExternalTexture, AddToContextAsyncWithLayerIndex)
{
#if defined(SKIP_EXTERNAL_TEXTURE_TESTS) || defined(SKIP_EXTERNAL_TEXTURE_ARRAY_TESTS)
    GTEST_SKIP();
#else
    Babylon::Graphics::Device device{g_deviceConfig};

    device.StartRenderingCurrentFrame();

    // Array texture (3 layers) so a non-zero layer index is valid.
    auto nativeTexture = Helpers::CreateTexture(device.GetPlatformInfo().Device, 256, 256, 3);
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

        // Backwards-compat: the deprecated AddToContextAsync must still accept a layer
        // index and forward it to CreateForJavaScript (views only that array slice).
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

    // Close the frame in which the deprecated shim's synchronous CreateForJavaScript ran.
    device.FinishRenderingCurrentFrame();

    // get() (not wait()) so a rejected promise rethrows and fails the test.
    EXPECT_NO_THROW(promiseResolved.get_future().get());

    // The GL backend doesn't own the handle; destroy the source texture only after all
    // rendering that referenced it has completed (see ExternalTexture.h). Harmless on
    // D3D/Metal, which retain the native texture.
    Helpers::DestroyTexture(nativeTexture);
#endif
}
