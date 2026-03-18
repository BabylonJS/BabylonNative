#include <gtest/gtest.h>

#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/Window.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <Babylon/ScriptLoader.h>

#include <future>
#include <iostream>

extern Babylon::Graphics::Configuration g_deviceConfig;

// Exercises all sub-vec4 uniform setter paths followed by a draw call to
// verify that uniform data is padded to at least 16 bytes (vec4).
// bgfx always reads 16 bytes from uniform storage, so anything smaller
// causes a heap-buffer-overflow detectable by AddressSanitizer.
//
// Paths exercised:
//   setInt    → NativeEngine::SetInt    → Program::SetUniform (1 float + 3 pad)
//   setFloat  → NativeEngine::SetFloat  → SetFloatN<1>        (1 float + 3 pad)
//   setVector2→ NativeEngine::SetFloat2 → SetFloatN<2>        (2 floats + 2 pad)
//   setVector3→ NativeEngine::SetFloat3 → SetFloatN<3>        (3 floats + 1 pad)
TEST(UniformPadding, SubVec4UniformsDoNotOverflow)
{
    Babylon::Graphics::Device device{g_deviceConfig};
    Babylon::Graphics::DeviceUpdate update{device.GetUpdate("update")};

    device.StartRenderingCurrentFrame();
    update.Start();

    std::promise<void> done{};

    Babylon::AppRuntime::Options options{};
    options.UnhandledExceptionHandler = [&done](const Napi::Error& error) {
        std::cerr << "[Uncaught Error] " << Napi::GetErrorString(error) << std::endl;
        done.set_exception(std::make_exception_ptr(std::exception{}));
    };

    Babylon::AppRuntime runtime{options};
    runtime.Dispatch([&device](Napi::Env env) {
        device.AddToJavaScript(env);

        Babylon::Polyfills::Console::Initialize(env, [](const char* message, auto) {
            std::cout << message << std::endl;
        });
        Babylon::Polyfills::Window::Initialize(env);
        Babylon::Plugins::NativeEngine::Initialize(env);
    });

    Babylon::ScriptLoader loader{runtime};
    loader.LoadScript("app:///Assets/babylon.max.js");

    // Disable async shader compilation so shaders are ready immediately.
    // Create a scene with a ShaderMaterial that declares int, float, vec2,
    // and vec3 uniforms. Setting and rendering these exercises every sub-vec4
    // code path through Program::SetUniform → DrawInternal → bgfx::setUniform.
    loader.Eval(R"(
        const engine = new BABYLON.NativeEngine();
        engine.getCaps().parallelShaderCompile = null;
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
                uniform float testFloat;
                uniform vec2 testVec2;
                uniform vec3 testVec3;
                void main() {
                    gl_FragColor = vec4(
                        float(testInt) + testFloat + testVec2.x + testVec3.x,
                        testVec2.y + testVec3.y,
                        testVec3.z,
                        1.0);
                }
            `
        }, {
            attributes: ["position"],
            uniforms: ["worldViewProjection", "testInt", "testFloat", "testVec2", "testVec3"]
        });
        mat.setInt("testInt", 1);
        mat.setFloat("testFloat", 2.0);
        mat.setVector2("testVec2", new BABYLON.Vector2(3.0, 4.0));
        mat.setVector3("testVec3", new BABYLON.Vector3(5.0, 6.0, 7.0));
        sphere.material = mat;
        scene.render();
        if (!scene.isReady()) { throw new Error("Scene should be ready with synchronous shader compilation"); }
    )", "uniform_padding_test.js");

    loader.Dispatch([&done](Napi::Env) {
        done.set_value();
    });

    done.get_future().get();

    update.Finish();
    device.FinishRenderingCurrentFrame();
}
