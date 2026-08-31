#include <gtest/gtest.h>

#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Graphics/Texture.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/Window.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <Babylon/Plugins/ExternalTexture.h>
#include <Babylon/ScriptLoader.h>
#include <napi/pointer.h>

#include "Helpers.h"

#include <future>
#include <iostream>

extern Babylon::Graphics::Configuration g_deviceConfig;

namespace
{
    using NativeTexture = Babylon::Graphics::Texture;

    void TestJavaScriptDisposePreventsUpdate(bool useThinTexture)
    {
        Babylon::Graphics::Device device{g_deviceConfig};

        Babylon::AppRuntime::Options options{};
        options.UnhandledExceptionHandler = [](const Napi::Error& error) {
            std::cerr << "[Uncaught Error] " << Napi::GetErrorString(error) << std::endl;
            std::quick_exit(1);
        };
        Babylon::AppRuntime runtime{options};

        runtime.Dispatch([&device](Napi::Env env) {
            env.Global().Set("globalThis", env.Global());
            device.AddToJavaScript(env);
            Babylon::Polyfills::Console::Initialize(env, [](const char* message, auto) {
                std::cout << message << std::endl;
            });
            Babylon::Polyfills::Window::Initialize(env);
            Babylon::Plugins::NativeEngine::Initialize(env);
        });

        Babylon::ScriptLoader loader{runtime};
        loader.LoadScript("app:///Assets/babylon.max.js");

        device.StartRenderingCurrentFrame();

        auto nativeTexture = Helpers::CreateTexture(device.GetPlatformInfo().Device, 256, 256);
        Babylon::Plugins::ExternalTexture externalTexture{nativeTexture};
        Helpers::DestroyTexture(nativeTexture);

        std::promise<Babylon::Graphics::Texture*> disposed;
        loader.Dispatch([&externalTexture, useThinTexture, &disposed](Napi::Env env) {
            try
            {
                auto global = env.Global();
                auto babylon = global.Get("BABYLON").As<Napi::Object>();
                auto jsNativeTexture = externalTexture.CreateForJavaScript(env);
                auto* texture = jsNativeTexture.As<Napi::Pointer<NativeTexture>>().Get();
                auto engine = babylon.Get("NativeEngine").As<Napi::Function>().New({});
                auto internalTexture = engine.Get("wrapNativeTexture").As<Napi::Function>().Call(engine, {jsNativeTexture}).As<Napi::Object>();
                auto textureOwner = useThinTexture
                    ? babylon.Get("ThinTexture").As<Napi::Function>().New({internalTexture})
                    : internalTexture;

                global.Set("_externalNativeTexture", jsNativeTexture);
                global.Set("_externalEngine", engine);
                global.Set("_externalInternalTexture", internalTexture);
                global.Set("_externalTextureOwner", textureOwner);

                textureOwner.Get("dispose").As<Napi::Function>().Call(textureOwner, {});
                disposed.set_value(texture);
            }
            catch (...)
            {
                disposed.set_exception(std::current_exception());
            }
        });

        auto* texture = disposed.get_future().get();
        ASSERT_NE(texture, nullptr);
        EXPECT_FALSE(texture->IsValid());

        device.FinishRenderingCurrentFrame();

        for (uint32_t size : {128u, 64u})
        {
            device.StartRenderingCurrentFrame();

            auto replacement = Helpers::CreateTexture(device.GetPlatformInfo().Device, size, size);
            externalTexture.Update(replacement);
            Helpers::DestroyTexture(replacement);

            EXPECT_FALSE(texture->IsValid());

            device.FinishRenderingCurrentFrame();
        }

        std::promise<void> cleanedUp;
        loader.Dispatch([&cleanedUp](Napi::Env env) {
            try
            {
                auto global = env.Global();
                auto engine = global.Get("_externalEngine").As<Napi::Object>();
                engine.Get("dispose").As<Napi::Function>().Call(engine, {});
                global.Delete("_externalTextureOwner");
                global.Delete("_externalInternalTexture");
                global.Delete("_externalNativeTexture");
                global.Delete("_externalEngine");
                cleanedUp.set_value();
            }
            catch (...)
            {
                cleanedUp.set_exception(std::current_exception());
            }
        });
        cleanedUp.get_future().get();
    }
}

TEST(ExternalTexture, Construction)
{
#ifdef SKIP_EXTERNAL_TEXTURE_TESTS
    GTEST_SKIP();
#else
    Babylon::Graphics::Device device{g_deviceConfig};

    device.StartRenderingCurrentFrame();

    auto nativeTexture = Helpers::CreateTexture(device.GetPlatformInfo().Device, 256, 256);
    Babylon::Plugins::ExternalTexture externalTexture{nativeTexture};
    Helpers::DestroyTexture(nativeTexture);

    EXPECT_EQ(externalTexture.Width(), 256u);
    EXPECT_EQ(externalTexture.Height(), 256u);

    device.FinishRenderingCurrentFrame();
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

    device.FinishRenderingCurrentFrame();
#endif
}

TEST(ExternalTexture, JavaScriptDisposeInternalTexturePreventsUpdate)
{
#ifdef SKIP_EXTERNAL_TEXTURE_TESTS
    GTEST_SKIP();
#else
    TestJavaScriptDisposePreventsUpdate(false);
#endif
}

TEST(ExternalTexture, JavaScriptDisposeThinTexturePreventsUpdate)
{
#ifdef SKIP_EXTERNAL_TEXTURE_TESTS
    GTEST_SKIP();
#else
    TestJavaScriptDisposePreventsUpdate(true);
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
    Babylon::Plugins::ExternalTexture externalTexture{nativeTexture};
    Helpers::DestroyTexture(nativeTexture);

    EXPECT_EQ(externalTexture.Width(), 256u);
    EXPECT_EQ(externalTexture.Height(), 256u);

    device.FinishRenderingCurrentFrame();

    // Update the external texture to point at a new native texture with different dimensions.
    device.StartRenderingCurrentFrame();

    auto nativeTexture2 = Helpers::CreateTexture(device.GetPlatformInfo().Device, 128, 128);
    externalTexture.Update(nativeTexture2);
    Helpers::DestroyTexture(nativeTexture2);

    EXPECT_EQ(externalTexture.Width(), 128u);
    EXPECT_EQ(externalTexture.Height(), 128u);

    device.FinishRenderingCurrentFrame();
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
    device.FinishRenderingCurrentFrame();

    // Wait for promise to resolve.
    promiseResolved.get_future().wait();

    // Start a new frame.
    device.StartRenderingCurrentFrame();

    // Update the external texture to a new texture.
    auto nativeTexture2 = Helpers::CreateTexture(device.GetPlatformInfo().Device, 256, 256);
    externalTexture.Update(nativeTexture2);
    Helpers::DestroyTexture(nativeTexture2);

    device.FinishRenderingCurrentFrame();
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
#endif
}
