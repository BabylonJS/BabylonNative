#include "gtest/gtest.h"
#include <optional>
#include <future>
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
#include <napi/env.h>

Babylon::Graphics::Configuration deviceTestConfig{};

TEST(JSTest, JavaScriptTests)
{
    std::promise<int32_t> exitCode;
    Babylon::Graphics::Device device{deviceTestConfig};

    std::optional<Babylon::Polyfills::Canvas> nativeCanvas;

    Babylon::AppRuntime runtime{};
    runtime.Dispatch([&exitCode, &device, &nativeCanvas](Napi::Env env)
    {
        device.AddToJavaScript(env);

        Babylon::Polyfills::XMLHttpRequest::Initialize(env);
        Babylon::Polyfills::Console::Initialize(env, [](const char* message, auto)
        {
            printf("%s", message);
            fflush(stdout);
        });
        Babylon::Polyfills::Window::Initialize(env);
        nativeCanvas.emplace(Babylon::Polyfills::Canvas::Initialize(env));
        Babylon::Plugins::NativeEngine::Initialize(env);
        
        env.Global().Set("SetExitCode", Napi::Function::New(env, [&exitCode](const Napi::CallbackInfo& info)
        {
            Napi::Env env = info.Env();
            exitCode.set_value(info[0].As<Napi::Number>().Int32Value());
        }, "SetExitCode"));
    });

    Babylon::ScriptLoader loader{runtime};
    loader.Eval("global = {};", ""); // Required for Chai.js as we do not have global in Babylon Native
    loader.Eval("location = { href: '' };", ""); // Required for Mocha.js as we do not have a location in Babylon Native
    loader.LoadScript("app:///Scripts/babylon.max.js");
    loader.LoadScript("app:///Scripts/babylonjs.materials.js");
    loader.LoadScript("app:///Scripts/chai.js");
    loader.LoadScript("app:///Scripts/mocha.js");
    loader.LoadScript("app:///Scripts/tests.js");

    device.StartRenderingCurrentFrame();
    device.FinishRenderingCurrentFrame();

    // Add CPP tests here

    auto code{exitCode.get_future().get()};
    EXPECT_EQ(code, 0);
}

/*
This test does a serie of initialization and shutdowns.
It needs the shutdown PR to be merged before running properly.
TEST(NativeAPI, LifeCycle)
{
    for (int cycle = 0; cycle < 20; cycle++)
    {
        Babylon::Graphics::Device device = deviceTestConfig;
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
    static const std::string script{ R"(
        console.log("Setting up Performance test.");
        var engine = new BABYLON.NativeEngine();
        var scene = new BABYLON.Scene(engine);
        function CreateSpheresAsync(scene) {
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

            return Promise.resolve();
        }
        CreateSpheresAsync(scene).then(function () {
            scene.createDefaultCamera(true, true, true);
            scene.activeCamera.alpha += Math.PI;
            scene.createDefaultLight(true);
            engine.runRenderLoop(function () {
                scene.render();
            });
            console.log("Ready!");
            SetReady();
        }, function (ex) {
            console.log(ex.message, ex.stack);
        });
        )" };

    Babylon::Graphics::Device device = deviceTestConfig;
    std::optional<Babylon::Polyfills::Canvas> nativeCanvas;
    std::optional<Babylon::Graphics::DeviceUpdate> update{};
    std::promise<int32_t> ready;
    update.emplace(device.GetUpdate("update"));

    Babylon::AppRuntime runtime{};
    runtime.Dispatch([&ready, &device, &nativeCanvas](Napi::Env env) {
        device.AddToJavaScript(env);

        Babylon::Polyfills::XMLHttpRequest::Initialize(env);
        Babylon::Polyfills::Console::Initialize(env, [](const char* message, auto) {
            printf("%s", message);
            fflush(stdout);
            });
        Babylon::Polyfills::Window::Initialize(env);
        nativeCanvas.emplace(Babylon::Polyfills::Canvas::Initialize(env));
        Babylon::Plugins::NativeEngine::Initialize(env);
        env.Global().Set("SetReady", Napi::Function::New(env, [&ready](const Napi::CallbackInfo& info)
        {
            Napi::Env env = info.Env();
            ready.set_value(1);
        }, "SetReady"));
    });

    Babylon::ScriptLoader loader{ runtime };
    loader.LoadScript("app:///Scripts/babylon.max.js");
    loader.LoadScript("app:///Scripts/babylonjs.materials.js");
    loader.Eval(script, "code");

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
    const float durationSeconds = float(double(duration.count()) / 1000.);
    printf("Duration is %f seconds.\n", durationSeconds);
}

int Run()
{
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}