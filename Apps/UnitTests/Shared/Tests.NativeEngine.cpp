#include <gtest/gtest.h>
#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/Window.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <Babylon/ScriptLoader.h>
#include <Babylon/ShaderCache.h>
#include <chrono>
#include <thread>
#include <optional>
#include <future>
#include <iostream>
#include <fstream>

using namespace std::chrono_literals;

extern Babylon::Graphics::Configuration g_deviceConfig;

TEST(NativeEngine, ShaderCache)
{
    Babylon::ShaderCache::Enabled(true);

    Babylon::Graphics::Device device{g_deviceConfig};
    Babylon::Graphics::DeviceUpdate update{device.GetUpdate("update")};

    device.StartRenderingCurrentFrame();
    update.Start();

    std::promise<void> scriptIsDone{};
    std::promise<void> sceneIsReady{};

    Babylon::AppRuntime runtime{};
    runtime.Dispatch([&device, &sceneIsReady](Napi::Env env) {
        device.AddToJavaScript(env);

        Babylon::Polyfills::Console::Initialize(env, [](const char* message, auto) {
            std::cout << message << std::endl;
            std::cout.flush();
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
    loader.LoadScript("app:///Assets/tests.nativeEngine.shaderCache.js");
    loader.Dispatch([&scriptIsDone](Napi::Env) {
        scriptIsDone.set_value();
    });

    scriptIsDone.get_future().get();

    auto sceneIsReadyFuture = sceneIsReady.get_future();
    while (sceneIsReadyFuture.wait_for(16ms) != std::future_status::ready)
    {
        update.Finish();
        device.FinishRenderingCurrentFrame();
        device.StartRenderingCurrentFrame();
        update.Start();
    }

    static const char* shaderCacheFileName = "shaderCache.bin";
    uint32_t shaderCount{};
    {
        std::ofstream fileSerialize(shaderCacheFileName, std::ios::binary);
        shaderCount = Babylon::ShaderCache::Serialize(fileSerialize);
        EXPECT_EQ(shaderCount, 1);
    }
    {
        std::ifstream file(shaderCacheFileName, std::ios::binary);
        auto deserializedCount = Babylon::ShaderCache::Deserialize(file);
        EXPECT_EQ(deserializedCount, shaderCount);
    }

    update.Finish();
    device.FinishRenderingCurrentFrame();
}
