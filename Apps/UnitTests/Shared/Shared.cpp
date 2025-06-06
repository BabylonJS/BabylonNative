#include "gtest/gtest.h"
#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Polyfills/XMLHttpRequest.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/Window.h>
#include <Babylon/Polyfills/Canvas.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <Babylon/ScriptLoader.h>
#include <Babylon/ShaderCache.h>
#include <chrono>
#include <thread>
#include <optional>
#include <future>
#include <iostream>
#include <fstream>

namespace
{
    Babylon::Graphics::Configuration deviceConfig{};

    const char* EnumToString(Babylon::Polyfills::Console::LogLevel logLevel)
    {
        switch (logLevel)
        {
            case Babylon::Polyfills::Console::LogLevel::Log:
                return "log";
            case Babylon::Polyfills::Console::LogLevel::Warn:
                return "warn";
            case Babylon::Polyfills::Console::LogLevel::Error:
                return "error";
        }

        return "unknown";
    }
}

TEST(JavaScript, All)
{
    // Change this to true to wait for the JavaScript debugger to attach (only applies to V8)
    constexpr const bool waitForDebugger = false;

    std::promise<int32_t> exitCodePromise;

    Babylon::Graphics::Device device{deviceConfig};

    std::optional<Babylon::Polyfills::Canvas> nativeCanvas;

    Babylon::AppRuntime::Options options{};

    options.UnhandledExceptionHandler = [&exitCodePromise](const Napi::Error& error) {
        std::cerr << "[Uncaught Error] " << error.Get("stack").As<Napi::String>().Utf8Value() << std::endl;
        std::cerr.flush();

        exitCodePromise.set_value(-1);
    };

    if (waitForDebugger)
    {
        std::cout << "Waiting for debugger..." << std::endl;
        options.WaitForDebugger = true;
    }

    Babylon::AppRuntime runtime{options};

    runtime.Dispatch([&exitCodePromise, &device, &nativeCanvas](Napi::Env env) {
        device.AddToJavaScript(env);

        Babylon::Polyfills::XMLHttpRequest::Initialize(env);
        Babylon::Polyfills::Console::Initialize(env, [](const char* message, Babylon::Polyfills::Console::LogLevel logLevel) {
            std::cout << "[" << EnumToString(logLevel) << "] " << message << std::endl;
            std::cout.flush();
        });
        Babylon::Polyfills::Window::Initialize(env);
        nativeCanvas.emplace(Babylon::Polyfills::Canvas::Initialize(env));
        Babylon::Plugins::NativeEngine::Initialize(env);

        auto setExitCodeCallback = Napi::Function::New(
            env, [&exitCodePromise](const Napi::CallbackInfo& info) {
                Napi::Env env = info.Env();
                exitCodePromise.set_value(info[0].As<Napi::Number>().Int32Value());
            },
            "setExitCode");
        env.Global().Set("setExitCode", setExitCodeCallback);
    });

    Babylon::ScriptLoader loader{runtime};
    loader.Eval("global = {};", "");             // Required for Chai.js as we do not have global in Babylon Native
    loader.Eval("location = { href: '' };", ""); // Required for Mocha.js as we do not have a location in Babylon Native
    loader.LoadScript("app:///Scripts/babylon.max.js");
    loader.LoadScript("app:///Scripts/babylonjs.materials.js");
    loader.Eval("exports = {};", "");             // Required for Chai.js as we do not have exports in Babylon Native
    // Required for Mocha.js as self and globalThis don't exist
    // This should be removed once using Webpack and making a bundle with test.ts instead of a .js
    loader.Eval(R"(
            (function() {
                if (typeof globalThis === 'object') return;

                Object.defineProperty(Object.prototype, '__magic__', {
                  get: function() {
                    return this;
                  },
                  configurable : true
                    });

                __magic__.globalThis = __magic__;
                delete Object.prototype.__magic__;
            })();

            if (typeof self === 'undefined') {
                self = globalThis;
            }
        )", "");
    loader.LoadScript("app:///Scripts/chai.umd.js");
    loader.Eval("globalThis.chai = exports;", "");
    loader.LoadScript("app:///Scripts/mocha.umd.js");
    loader.LoadScript("app:///Scripts/tests.js");

    device.StartRenderingCurrentFrame();
    device.FinishRenderingCurrentFrame();

    auto exitCode{exitCodePromise.get_future().get()};
    EXPECT_EQ(exitCode, 0);
}

/*
This test does a serie of initialization and shutdowns.
It needs the shutdown PR to be merged before running properly.
TEST(NativeAPI, LifeCycle)
{
    for (int cycle = 0; cycle < 20; cycle++)
    {
        Babylon::Graphics::Device device{deviceConfig};
        std::optional<Babylon::Polyfills::Canvas> nativeCanvas;

        Babylon::AppRuntime runtime{};
        runtime.Dispatch([&device, &nativeCanvas](Napi::Env env) {
            device.AddToJavaScript(env);

            Babylon::Polyfills::XMLHttpRequest::Initialize(env);
            Babylon::Polyfills::Console::Initialize(env, [](const char* message, auto) {
                printf("%s", message);
                fflush(stdout);
            });
            Babylon::Polyfills::Window::Initialize(env);
            nativeCanvas.emplace(Babylon::Polyfills::Canvas::Initialize(env));
            Babylon::Plugins::NativeEngine::Initialize(env);
        });

        Babylon::ScriptLoader loader{runtime};
        loader.LoadScript("app:///Scripts/babylon.max.js");
        loader.LoadScript("app:///Scripts/babylonjs.materials.js");

        for (int frame = 0; frame < 10; frame++)
        {
            device.StartRenderingCurrentFrame();
            device.FinishRenderingCurrentFrame();
        }
    }
}
*/

TEST(Performance, Spheres)
{
    // create a bunch of sphere, does the rendering for a number of frames, log time it took
    std::string script{R"(
        console.log("Setting up Performance test.");
        var engine = new BABYLON.NativeEngine();
        var scene = new BABYLON.Scene(engine);

        var size = 12;
        for (var i = 0; i < size; i++) {
            for (var j = 0; j < size; j++) {
                for (var k = 0; k < size; k++) {
                    var sphere = BABYLON.Mesh.CreateSphere("sphere" + i + j + k, 32, 0.9, scene);
                    sphere.position.x = i;
                    sphere.position.y = j;
                    sphere.position.z = k;
                }
            }
        }

        scene.createDefaultCamera(true, true, true);
        scene.activeCamera.alpha += Math.PI;
        scene.createDefaultLight(true);
        engine.runRenderLoop(function () {
            scene.render();
        });
        console.log("Ready!");
        setReady();
    )"};

    Babylon::Graphics::Device device{deviceConfig};
    std::optional<Babylon::Graphics::DeviceUpdate> update{};
    std::promise<int32_t> ready;
    update.emplace(device.GetUpdate("update"));

    Babylon::AppRuntime runtime{};
    runtime.Dispatch([&ready, &device](Napi::Env env) {
        device.AddToJavaScript(env);

        Babylon::Polyfills::Console::Initialize(env, [](const char* message, auto) {
            std::cout << message << std::endl;
            std::cout.flush();
        });
        Babylon::Polyfills::Window::Initialize(env);
        Babylon::Plugins::NativeEngine::Initialize(env);
        env.Global().Set("setReady", Napi::Function::New(
                                         env, [&ready](const Napi::CallbackInfo& info) {
                                             Napi::Env env = info.Env();
                                             ready.set_value(1);
                                         },
                                         "setReady"));
    });

    Babylon::ScriptLoader loader{runtime};
    loader.LoadScript("app:///Scripts/babylon.max.js");
    loader.LoadScript("app:///Scripts/babylonjs.materials.js");
    loader.Eval(std::move(script), "code");

    ready.get_future().get();

    const auto start = std::chrono::high_resolution_clock::now();

    for (int frame = 0; frame < 100; frame++)
    {
        device.StartRenderingCurrentFrame();
        update->Start();
        update->Finish();
        device.FinishRenderingCurrentFrame();
    }
    // Stop measuring time
    const auto stop = std::chrono::high_resolution_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    const float durationSeconds = float(duration.count()) / 1000.f;
    std::cout << "Duration is " << durationSeconds << " seconds. " << std::endl;
    std::cout.flush();
}

TEST(Performance, ShaderCache)
{
    std::string script{ R"(
        console.log("Setting rendering to cache shader.");
        var engine = new BABYLON.NativeEngine();
        var scene = new BABYLON.Scene(engine);

        var sphere = BABYLON.Mesh.CreateSphere("sphere", 32, 0.9, scene);

        scene.createDefaultCamera(true, true, true);
        scene.activeCamera.alpha += Math.PI;
        scene.createDefaultLight(true);
        scene.whenReadyAsync().then(function () {
            setSceneReady();
        });
        setReady();
    )" };

    Babylon::ShaderCache::Enabled(true);

    Babylon::Graphics::Device device{ deviceConfig };
    std::optional<Babylon::Graphics::DeviceUpdate> update{};
    std::promise<int32_t> ready{};
    std::atomic<bool> sceneIsReady{};
    update.emplace(device.GetUpdate("update"));

    Babylon::AppRuntime runtime{};
    runtime.Dispatch([&ready, &device, &sceneIsReady](Napi::Env env) {
        device.AddToJavaScript(env);

        Babylon::Polyfills::Console::Initialize(env, [](const char* message, auto) {
            std::cout << message << std::endl;
            std::cout.flush();
            });
        Babylon::Polyfills::Window::Initialize(env);
        Babylon::Plugins::NativeEngine::Initialize(env);
        env.Global().Set("setReady", Napi::Function::New(
            env, [&ready](const Napi::CallbackInfo&) {
                ready.set_value(1);
            },
            "setReady"));

        env.Global().Set("setSceneReady", Napi::Function::New(
            env, [&sceneIsReady](const Napi::CallbackInfo&) {
                sceneIsReady = true;
            },
            "setSceneReady"));
        });

    Babylon::ScriptLoader loader{ runtime };
    loader.LoadScript("app:///Scripts/babylon.max.js");
    loader.LoadScript("app:///Scripts/babylonjs.materials.js");
    loader.Eval(std::move(script), "code");

    ready.get_future().get();

    while(!sceneIsReady)
    {
        device.StartRenderingCurrentFrame();
        update->Start();
        update->Finish();
        device.FinishRenderingCurrentFrame();
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
}

int RunTests(const Babylon::Graphics::Configuration& config)
{
    deviceConfig = config;
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
