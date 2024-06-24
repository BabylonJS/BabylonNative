#include "gtest/gtest.h"
#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Polyfills/XMLHttpRequest.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/Window.h>
#include <Babylon/Polyfills/Canvas.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <Babylon/ScriptLoader.h>
#include <chrono>
#include <thread>
#include <optional>
#include <future>
#include <iostream>

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
    loader.LoadScript("app:///Scripts/chai.js");
    loader.LoadScript("app:///Scripts/mocha.js");
    loader.LoadScript("app:///Scripts/tests.js");

    device.StartRenderingCurrentFrame();
    device.FinishRenderingCurrentFrame();

    auto exitCode{exitCodePromise.get_future().get()};
    EXPECT_EQ(exitCode, 0);
}

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

TEST(Shutdown, AsyncShaderCompilation)
{
    std::promise<int32_t> exitCodePromise;
    Babylon::Graphics::Device device{ deviceConfig };
    auto update{ device.GetUpdate("update") };
    std::optional<Babylon::Polyfills::Canvas> nativeCanvas;
    Babylon::AppRuntime runtime{};
    
    device.StartRenderingCurrentFrame();
    update.Start();
    update.Finish();
    device.FinishRenderingCurrentFrame();

    runtime.Dispatch([&device, &nativeCanvas, &exitCodePromise](Napi::Env env) {
        device.AddToJavaScript(env);
        Babylon::Polyfills::XMLHttpRequest::Initialize(env);
        Babylon::Polyfills::Console::Initialize(env, [](const char* message, auto) {
            std::cout << message << std::endl;
            std::cout.flush();
            });
        Babylon::Polyfills::Window::Initialize(env);
        nativeCanvas.emplace(Babylon::Polyfills::Canvas::Initialize(env));
        Babylon::Plugins::NativeEngine::Initialize(env);
        auto setExitCodeCallback = Napi::Function::New(
            env, [&exitCodePromise](const Napi::CallbackInfo& info) {
                Napi::Env env = info.Env();
                exitCodePromise.set_value(info[0].As<Napi::Number>().Int32Value());
            }, "setExitCode");
        env.Global().Set("setExitCode", setExitCodeCallback);
        });
    Babylon::ScriptLoader loader{ runtime };
    loader.LoadScript("app:///Scripts/babylon.max.js");
    loader.LoadScript("app:///Scripts/babylonjs.materials.js");
    loader.Eval(R"(
            function CreateBoxAsync(scene) {
                BABYLON.Mesh.CreateBox("box1", 0.2, scene);
                return Promise.resolve();
            }
            var engine = new BABYLON.NativeEngine();
            var scene = new BABYLON.Scene(engine);
            CreateBoxAsync(scene).then(function () {
                var disc = BABYLON.Mesh.CreateDisc("disc", 3, 60, scene);
                scene.createDefaultCamera(true, true, true);
                scene.activeCamera.alpha += Math.PI;
                scene.createDefaultLight(true);
                var mainMaterial = new BABYLON.PBRMaterial("main", scene);
                disc.material = mainMaterial;
                engine.runRenderLoop(function () {
                    scene.render();
                });
                console.log("Ready to shutdown.");
                setExitCode(1);
            }, function (ex) {
                console.log(ex.message, ex.stack);
            });
            )", "script");
    auto exitCode{ exitCodePromise.get_future().get() };
    device.StartRenderingCurrentFrame();
    update.Start();
    update.Finish();
    device.FinishRenderingCurrentFrame();
}

int RunTests(const Babylon::Graphics::Configuration& config)
{
    deviceConfig = config;
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
