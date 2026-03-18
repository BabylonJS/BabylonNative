#include <gtest/gtest.h>

#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/Window.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <Babylon/ScriptLoader.h>

#include <chrono>
#include <future>
#include <iostream>

using namespace std::chrono_literals;

extern Babylon::Graphics::Configuration g_deviceConfig;

// Exercises the SetInt → SetUniform → DrawInternal → bgfx::setUniform path
// to verify that sub-vec4 uniform data is padded to 16 bytes.
// Without the fix, AddressSanitizer detects a heap-buffer-overflow because
// bgfx always reads 16 bytes from uniform storage.
TEST(UniformPadding, SetIntDoesNotOverflowUnderASAN)
{
    Babylon::Graphics::Device device{g_deviceConfig};
    Babylon::Graphics::DeviceUpdate update{device.GetUpdate("update")};

    device.StartRenderingCurrentFrame();
    update.Start();

    std::promise<void> testDone{};

    Babylon::AppRuntime::Options options{};
    options.UnhandledExceptionHandler = [&testDone](const Napi::Error& error) {
        std::cerr << "[Uncaught Error] " << Napi::GetErrorString(error) << std::endl;
        testDone.set_exception(std::make_exception_ptr(std::exception{}));
    };

    Babylon::AppRuntime runtime{options};
    runtime.Dispatch([&device, &testDone](Napi::Env env) {
        device.AddToJavaScript(env);

        Babylon::Polyfills::Console::Initialize(env, [](const char* message, auto) {
            std::cout << message << std::endl;
        });
        Babylon::Polyfills::Window::Initialize(env);
        Babylon::Plugins::NativeEngine::Initialize(env);

        env.Global().Set("testDone",
            Napi::Function::New(env, [&testDone](const Napi::CallbackInfo&) {
                testDone.set_value();
            }, "testDone"));
    });

    Babylon::ScriptLoader loader{runtime};
    loader.LoadScript("app:///Assets/babylon.max.js");

    // Create a scene with a ShaderMaterial that uses setInt (sub-vec4 uniform).
    // When the scene renders, this triggers:
    //   NativeEngine::SetInt → Program::SetUniform → DrawInternal → bgfx::setUniform
    loader.Eval(R"(
        const engine = new BABYLON.NativeEngine();
        const scene = new BABYLON.Scene(engine);
        scene.createDefaultCamera();
        const sphere = BABYLON.MeshBuilder.CreateSphere("s", { diameter: 1 }, scene);
        const mat = new BABYLON.ShaderMaterial("test", scene, {
            vertexSource: `
                attribute vec3 position;
                uniform mat4 worldViewProjection;
                void main() { gl_Position = worldViewProjection * vec4(position, 1.0); }
            `,
            fragmentSource: `
                precision highp float;
                uniform int testInt;
                void main() { gl_FragColor = vec4(float(testInt), 0.0, 0.0, 1.0); }
            `
        }, {
            attributes: ["position"],
            uniforms: ["worldViewProjection", "testInt"]
        });
        mat.setInt("testInt", 1);
        sphere.material = mat;
        scene.executeWhenReady(function() {
            scene.render();
            testDone();
        });
    )", "uniform_padding_test.js");

    // Pump the render loop until the JS test signals completion.
    auto future = testDone.get_future();
    while (future.wait_for(16ms) != std::future_status::ready)
    {
        update.Finish();
        device.FinishRenderingCurrentFrame();
        device.StartRenderingCurrentFrame();
        update.Start();
    }

    // Rethrow any JS exception so the test properly fails.
    future.get();

    update.Finish();
    device.FinishRenderingCurrentFrame();
}
