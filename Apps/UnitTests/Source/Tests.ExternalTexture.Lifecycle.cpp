#include <gtest/gtest.h>

#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Graphics/DeviceContext.h>
#include <Babylon/Graphics/Texture.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/Window.h>
#include <Babylon/Plugins/ExternalTexture.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <Babylon/ScriptLoader.h>
#include <napi/pointer.h>

#include "Helpers.h"

#include <future>
#include <iostream>
#include <memory>

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
                global.Set("_externalTextureOwner", env.Undefined());
                global.Set("_externalInternalTexture", env.Undefined());
                global.Set("_externalNativeTexture", env.Undefined());
                global.Set("_externalEngine", env.Undefined());
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

TEST(ExternalTexture, NativeImportRetainsOwnerThroughHandleDestruction)
{
    for (bool afterRender : {false, true})
    {
        for (uint32_t action : {0u, 1u, 2u})
        {
            SCOPED_TRACE(afterRender);
            SCOPED_TRACE(action);
            Babylon::Graphics::Device device{g_deviceConfig};
            Babylon::AppRuntime runtime{};
            std::promise<Babylon::Graphics::DeviceContext*> result;
            auto future = result.get_future();
            runtime.Dispatch([&](Napi::Env env) {
                device.AddToJavaScript(env);
                result.set_value(&Babylon::Graphics::DeviceContext::GetFromJavaScript(env));
            });
            auto& context = *future.get();
            device.StartRenderingCurrentFrame();

            if (bgfx::getRendererType() != bgfx::RendererType::Direct3D11 &&
                bgfx::getRendererType() != bgfx::RendererType::Metal)
            {
                device.FinishRenderingCurrentFrame();
                GTEST_SKIP() << "Native pointer import coverage requires D3D11 or Metal.";
            }

            auto native = std::shared_ptr<void>{
                Helpers::CreateTexture(device.GetPlatformInfo().Device, 4, 4),
                Helpers::DestroyTexture};
            std::weak_ptr<void> retained = native;
            std::weak_ptr<void> replacement;
            auto texture = std::make_unique<Babylon::Graphics::Texture>(context);
            texture->Create2D(4, 4, false, 1, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_NONE,
                reinterpret_cast<uintptr_t>(native.get()), native);

            // Model Close releasing the producer while the caller keeps its wrapper.
            native.reset();
            device.FinishRenderingCurrentFrame();
            for (uint32_t frame = 0; frame < 2; ++frame)
            {
                EXPECT_TRUE(texture->IsValid());
                EXPECT_FALSE(retained.expired());
                device.StartRenderingCurrentFrame();
                device.FinishRenderingCurrentFrame();
            }
            EXPECT_FALSE(retained.expired());

            device.StartRenderingCurrentFrame();
            auto disposeOrReplace = [&] {
                if (action == 0)
                {
                    texture->Dispose();
                }
                else if (action == 1)
                {
                    texture.reset();
                }
                else
                {
                    // Replacing the producer's output must not release the old import.
                    native = std::shared_ptr<void>{
                        Helpers::CreateTexture(device.GetPlatformInfo().Device, 8, 8),
                        Helpers::DestroyTexture};
                    EXPECT_FALSE(retained.expired());
                    replacement = native;
                    texture->Create2D(8, 8, false, 1, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_NONE,
                        reinterpret_cast<uintptr_t>(native.get()), native);
                    native.reset();
                }
                EXPECT_FALSE(retained.expired());
            };
            if (afterRender)
            {
                arcana::make_task(context.AfterRenderScheduler(), arcana::cancellation::none(), disposeOrReplace);
            }
            else
            {
                disposeOrReplace();
            }
            device.FinishRenderingCurrentFrame();

            if (afterRender)
            {
                EXPECT_FALSE(retained.expired());
                device.StartRenderingCurrentFrame();
                device.FinishRenderingCurrentFrame();
            }
            EXPECT_TRUE(retained.expired());
            if (action == 0)
            {
                EXPECT_FALSE(texture->IsValid());
            }
            else if (action == 2)
            {
                EXPECT_TRUE(texture->IsValid());
                EXPECT_EQ(texture->Width(), 8);
                EXPECT_EQ(texture->Height(), 8);
                EXPECT_FALSE(replacement.expired());
                texture.reset();
                EXPECT_FALSE(replacement.expired());
                device.StartRenderingCurrentFrame();
                device.FinishRenderingCurrentFrame();
                EXPECT_TRUE(replacement.expired());
            }
        }
    }
}
