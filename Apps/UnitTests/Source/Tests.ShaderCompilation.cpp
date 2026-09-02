#include <gtest/gtest.h>

#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/Window.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <Babylon/ScriptLoader.h>

#include <chrono>
#include <cstdlib>
#include <future>
#include <iostream>
#include <string>

using namespace std::chrono_literals;

extern Babylon::Graphics::Configuration g_deviceConfig;

TEST(ShaderCompilation, CompileComprehensiveGLSL)
{
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
    loader.LoadScript("app:///Assets/tests.shaderCompilation.comprehensiveGLSL.js");
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

    device.FinishRenderingCurrentFrame();
}

TEST(ShaderCompilation, ExistingVec4UniformArrayIsNotReshaped)
{
    Babylon::Graphics::Device device{g_deviceConfig};

    device.StartRenderingCurrentFrame();

    Babylon::AppRuntime::Options options{};
    options.UnhandledExceptionHandler = [](const Napi::Error& error) {
        std::cerr << "[Uncaught Error] " << Napi::GetErrorString(error) << std::endl;
        std::quick_exit(1);
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
    loader.Eval(R"(
        const engine = new BABYLON.NativeEngine();
        engine.getCaps().parallelShaderCompile = null;
        const effect = engine.createEffect({
            vertexSource: `
                attribute vec2 position;
                void main() { gl_Position = vec4(position, 0.0, 1.0); }
            `,
            fragmentSource: `
                precision highp float;
                uniform vec4 values[2];
                vec4 readValue() { return values[0]; }
                void main() { gl_FragColor = readValue(); }
            `
        }, ["position"], ["values"], []);
        if (!effect.isReady()) { throw new Error("Vec4 uniform array effect should compile synchronously"); }
        effect.dispose();
        engine.dispose();
    )", "existing_vec4_uniform_array_test.js");

    std::promise<void> done{};
    loader.Dispatch([&done](Napi::Env) {
        done.set_value();
    });
    done.get_future().get();

    device.FinishRenderingCurrentFrame();
}

TEST(ShaderCompilation, ReturnedUniformIsShapeConverted)
{
    Babylon::Graphics::Device device{g_deviceConfig};

    device.StartRenderingCurrentFrame();

    Babylon::AppRuntime::Options options{};
    options.UnhandledExceptionHandler = [](const Napi::Error& error) {
        std::cerr << "[Uncaught Error] " << Napi::GetErrorString(error) << std::endl;
        std::quick_exit(1);
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
    loader.Eval(R"(
        const engine = new BABYLON.NativeEngine();
        engine.getCaps().parallelShaderCompile = null;
        const effect = engine.createEffect({
            vertexSource: `
                attribute vec2 position;
                void main() { gl_Position = vec4(position, 0.0, 1.0); }
            `,
            fragmentSource: `
                precision highp float;
                uniform vec3 tint;
                vec3 readTint() { return tint; }
                void main() { gl_FragColor = vec4(readTint(), 1.0); }
            `
        }, ["position"], ["tint"], []);
        if (!effect.isReady()) { throw new Error("Vec3 return effect should compile synchronously"); }
        effect.dispose();
        engine.dispose();
    )", "returned_uniform_shape_conversion_test.js");

    std::promise<void> done{};
    loader.Dispatch([&done](Napi::Env) {
        done.set_value();
    });
    done.get_future().get();

    device.FinishRenderingCurrentFrame();
}

TEST(ShaderCompilation, DisjointStageSamplersCompile)
{
    Babylon::Graphics::Device device{g_deviceConfig};
    device.StartRenderingCurrentFrame();

    Babylon::AppRuntime::Options options{};
    options.UnhandledExceptionHandler = [](const Napi::Error& error) {
        std::cerr << "[Uncaught Error] " << Napi::GetErrorString(error) << std::endl;
        std::quick_exit(1);
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
    loader.Eval(R"(
        const engine = new BABYLON.NativeEngine();
        engine.getCaps().parallelShaderCompile = null;
        const effect = engine.createEffect({
            vertexSource: `
                attribute vec2 position;
                uniform sampler2D vsOnly;
                void main() {
                    gl_Position = vec4(position, 0.0, 1.0) + texture2D(vsOnly, position) * 0.0;
                }
            `,
            fragmentSource: `
                precision highp float;
                uniform sampler2D fsOnly;
                void main() { gl_FragColor = texture2D(fsOnly, vec2(0.5)); }
            `
        }, ["position"], ["vsOnly", "fsOnly"], []);
        if (!effect.isReady()) { throw new Error("Disjoint stage sampler effect should compile"); }
        effect.dispose();
        engine.dispose();
    )", "disjoint_stage_samplers_test.js");

    std::promise<void> done{};
    loader.Dispatch([&done](Napi::Env) {
        done.set_value();
    });
    done.get_future().get();
    device.FinishRenderingCurrentFrame();
}

TEST(ShaderCompilation, PartiallySharedStageSamplersCompile)
{
    Babylon::Graphics::Device device{g_deviceConfig};
    device.StartRenderingCurrentFrame();

    Babylon::AppRuntime::Options options{};
    options.UnhandledExceptionHandler = [](const Napi::Error& error) {
        std::cerr << "[Uncaught Error] " << Napi::GetErrorString(error) << std::endl;
        std::quick_exit(1);
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
    loader.Eval(R"(
        const engine = new BABYLON.NativeEngine();
        engine.getCaps().parallelShaderCompile = null;
        const effect = engine.createEffect({
            vertexSource: `
                attribute vec2 position;
                uniform sampler2D sharedTex;
                uniform sampler2D vsOnly;
                void main() {
                    gl_Position = vec4(position, 0.0, 1.0)
                        + texture2D(sharedTex, position) * 0.0
                        + texture2D(vsOnly, position) * 0.0;
                }
            `,
            fragmentSource: `
                precision highp float;
                uniform sampler2D sharedTex;
                uniform sampler2D fsOnly;
                void main() {
                    gl_FragColor = texture2D(sharedTex, vec2(0.5)) + texture2D(fsOnly, vec2(0.5)) * 0.0;
                }
            `
        }, ["position"], ["sharedTex", "vsOnly", "fsOnly"], []);
        if (!effect.isReady()) { throw new Error("Partially shared stage sampler effect should compile"); }
        effect.dispose();
        engine.dispose();
    )", "partially_shared_stage_samplers_test.js");

    std::promise<void> done{};
    loader.Dispatch([&done](Napi::Env) {
        done.set_value();
    });
    done.get_future().get();
    device.FinishRenderingCurrentFrame();
}

#if defined(BABYLON_NATIVE_GRAPHICS_API_VULKAN)
TEST(ShaderCompilation, VulkanRejectsMoreThanSixteenSamplers)
{
    Babylon::Graphics::Device device{g_deviceConfig};
    device.StartRenderingCurrentFrame();

    Babylon::AppRuntime::Options options{};
    std::string failureMessage;
    options.UnhandledExceptionHandler = [&failureMessage](const Napi::Error& error) {
        failureMessage = Napi::GetErrorString(error);
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

    std::string uniforms;
    std::string decls;
    std::string sum;
    for (int i = 0; i < 17; ++i)
    {
        decls += "uniform sampler2D t" + std::to_string(i) + ";\n";
        if (i > 0)
        {
            uniforms += ", ";
            sum += " + ";
        }
        uniforms += "\"t" + std::to_string(i) + "\"";
        sum += "texture2D(t" + std::to_string(i) + ", vec2(0.5))";
    }

    std::string script = R"(
        const engine = new BABYLON.NativeEngine();
        engine.getCaps().parallelShaderCompile = null;
        let failed = false;
        let message = "";
        try {
            const effect = engine.createEffect({
                vertexSource: `
                    attribute vec2 position;
                    void main() { gl_Position = vec4(position, 0.0, 1.0); }
                `,
                fragmentSource: `
                    precision highp float;
)" + decls + R"(
                    void main() { gl_FragColor = )" + sum + R"(; }
                `
            }, ["position"], [)" + uniforms + R"(], []);
            if (effect.isReady()) { throw new Error("expected compile failure for >16 samplers"); }
        } catch (e) {
            failed = true;
            message = String(e && e.message ? e.message : e);
        }
        engine.dispose();
        globalThis.__failed = failed;
        globalThis.__message = message;
    )";

    Babylon::ScriptLoader loader{runtime};
    loader.LoadScript("app:///Assets/babylon.max.js");
    loader.Eval(script, "vulkan_too_many_samplers_test.js");

    bool failed = false;
    std::string message;
    std::promise<void> done{};
    loader.Dispatch([&done, &failed, &message](Napi::Env env) {
        failed = env.Global().Get("__failed").ToBoolean();
        message = env.Global().Get("__message").ToString();
        done.set_value();
    });
    done.get_future().get();
    device.FinishRenderingCurrentFrame();

    ASSERT_TRUE(failed || !failureMessage.empty()) << "expected compile failure";
    const std::string combined = message + failureMessage;
    EXPECT_NE(combined.find("16"), std::string::npos) << combined;
}
#endif
