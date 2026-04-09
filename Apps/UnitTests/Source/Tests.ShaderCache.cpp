#include <gtest/gtest.h>

#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/Window.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <Babylon/Plugins/ShaderCache.h>
#include <Babylon/ScriptLoader.h>

#include <chrono>
#include <cstdlib>
#include <optional>
#include <future>
#include <iostream>
#include <fstream>

using namespace std::chrono_literals;

extern Babylon::Graphics::Configuration g_deviceConfig;

TEST(ShaderCache, SaveAndLoad)
{
    Babylon::Plugins::ShaderCache::Enable();

    Babylon::Graphics::Device device{g_deviceConfig};

    device.StartRenderingCurrentFrame();

    Babylon::AppRuntime::Options options{};

    options.UnhandledExceptionHandler = [](const Napi::Error& error) {
        std::cerr << "[Uncaught Error] " << Napi::GetErrorString(error) << std::endl;
        std::quick_exit(1);
    };

    Babylon::AppRuntime runtime{options};

    std::promise<void> scriptIsDone{};
    std::promise<void> sceneIsReady{};

    runtime.Dispatch([&device, &sceneIsReady](Napi::Env env) {
        device.AddToJavaScript(env);

        Babylon::Polyfills::Console::Initialize(env, [](const char* message, auto) {
            std::cout << message << std::endl;
        });
        Babylon::Polyfills::Window::Initialize(env);
        Babylon::Plugins::NativeEngine::Initialize(env);

        env.Global().Set("setSceneReady",
            Napi::Function::New(
                env, [&sceneIsReady](const Napi::CallbackInfo&) {
                    sceneIsReady.set_value();
                },
                "setSceneReady"));
    });

    Babylon::ScriptLoader loader{runtime};
    loader.LoadScript("app:///Assets/babylon.max.js");
    loader.LoadScript("app:///Assets/tests.shaderCache.basicScene.js");
    loader.Dispatch([&scriptIsDone](Napi::Env) {
        scriptIsDone.set_value();
    });

    scriptIsDone.get_future().get();

    auto sceneIsReadyFuture = sceneIsReady.get_future();
    while (sceneIsReadyFuture.wait_for(16ms) != std::future_status::ready)
    {
        device.FinishRenderingCurrentFrame();
        device.StartRenderingCurrentFrame();
    }

    static const char* shaderCacheFileName = "shaderCache.bin";
    uint32_t shaderCount{};
    {
        std::ofstream stream(shaderCacheFileName, std::ios::binary);
        shaderCount = Babylon::Plugins::ShaderCache::Save(stream);
        EXPECT_EQ(shaderCount, 1);
    }
    {
        std::ifstream stream(shaderCacheFileName, std::ios::binary);
        auto deserializedCount = Babylon::Plugins::ShaderCache::Load(stream);
        EXPECT_EQ(deserializedCount, shaderCount);
    }

    device.FinishRenderingCurrentFrame();

    Babylon::Plugins::ShaderCache::Disable();
}
