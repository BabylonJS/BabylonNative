#include <gtest/gtest.h>

#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/Window.h>
#include <Babylon/Plugins/NativeEngine.h>
#ifdef HAS_SHADER_COMPILER
#include <Babylon/Plugins/ShaderCompiler.h>
#endif
#include <Babylon/ScriptLoader.h>

#include <chrono>
#include <cstdlib>
#include <future>
#include <iostream>
#include <string>

#if defined(BABYLON_NATIVE_GRAPHICS_API_VULKAN)
#include <Babylon/Plugins/ShaderCompiler.h>
#include <spirv_cross.hpp>
#include <cstring>
#include <stdexcept>
#endif

using namespace std::chrono_literals;

extern Babylon::Graphics::Configuration g_deviceConfig;

#ifdef HAS_SHADER_COMPILER
TEST(ShaderCompilation, NativeCompilerAcceptsExistingVec4UniformArray)
{
    Babylon::Plugins::ShaderCompiler compiler{};
    auto shader = compiler.Compile(
        R"(
            in vec2 position;
            void main() { gl_Position = vec4(position, 0.0, 1.0); }
        )",
        R"(
            precision highp float;
            uniform vec4 values[2];
            layout(location = 0) out vec4 fragColor;
            vec4 readValue() { return values[0]; }
            void main() { fragColor = readValue(); }
        )");

    EXPECT_FALSE(shader.VertexBytes.empty());
    EXPECT_FALSE(shader.FragmentBytes.empty());
}
#endif

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
namespace
{
    spirv_cross::Compiler ReadVulkanShader(const std::vector<uint8_t>& bytes)
    {
        // These fixtures have no uniforms: bgfx v12's code length starts at byte 22.
        constexpr size_t codeOffset = 26;
        if (bytes.size() < codeOffset || bytes[3] != 12 || bytes[20] != 0 || bytes[21] != 0)
        {
            throw std::runtime_error{"Expected a uniform-free bgfx v12 shader"};
        }
        uint32_t codeSize{};
        std::memcpy(&codeSize, bytes.data() + codeOffset - sizeof(codeSize), sizeof(codeSize));
        if (codeSize % sizeof(uint32_t) != 0 || codeSize > bytes.size() - codeOffset)
        {
            throw std::runtime_error{"Invalid SPIR-V byte length"};
        }
        std::vector<uint32_t> words(codeSize / sizeof(uint32_t));
        std::memcpy(words.data(), bytes.data() + codeOffset, codeSize);
        return spirv_cross::Compiler{std::move(words)};
    }

    struct SamplerCompilationResult
    {
        bool IsReady;
        std::string Error;
    };

    SamplerCompilationResult CompileVulkanSamplers(int samplerCount)
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

        std::string decls;
        std::string sum;
        for (int i = 0; i < samplerCount; ++i)
        {
            decls += "uniform sampler2D t" + std::to_string(i) + ";\n";
            if (i > 0)
            {
                sum += " + ";
            }
            sum += "texture(t" + std::to_string(i) + ", vec2(0.5))";
        }

        // Effect keeps only the stack on JavaScriptCore. Use the synchronous
        // program API to preserve the native compiler's error message.
        std::string script = R"(
            const engine = new BABYLON.NativeEngine();
            engine.getCaps().parallelShaderCompile = null;
            const pipeline = engine.createPipelineContext();
            let ready = false;
            let message = "";
            try {
                engine.createShaderProgram(pipeline,
                    `#version 300 es
                        precision highp float;
                        in vec2 position;
                        void main() { gl_Position = vec4(position, 0.0, 1.0); }
                    `,
                    `#version 300 es
                        precision highp float;
                        out vec4 color;
)" + decls + R"(
                        void main() { color = )" + sum + R"(; }
                    `, "");
                ready = pipeline.isReady;
            } catch (e) {
                message = String(e && e.message ? e.message : e);
            }
            engine._deletePipelineContext(pipeline);
            pipeline.dispose();
            engine.dispose();
            globalThis.__samplerReady = ready;
            globalThis.__samplerError = message;
        )";

        Babylon::ScriptLoader loader{runtime};
        loader.LoadScript("app:///Assets/babylon.max.js");
        loader.Eval(script, "vulkan_sampler_boundary_test.js");

        std::promise<SamplerCompilationResult> done{};
        loader.Dispatch([&done](Napi::Env env) {
            done.set_value({
                env.Global().Get("__samplerReady").ToBoolean().Value(),
                env.Global().Get("__samplerError").ToString(),
            });
        });
        auto result = done.get_future().get();
        device.FinishRenderingCurrentFrame();
        return result;
    }
}

TEST(ShaderCompilation, VulkanAcceptsSixteenSamplers)
{
    const auto result = CompileVulkanSamplers(16);
    EXPECT_TRUE(result.IsReady) << result.Error;
    EXPECT_TRUE(result.Error.empty()) << result.Error;
}

TEST(ShaderCompilation, VulkanRejectsMoreThanSixteenSamplers)
{
    const auto result = CompileVulkanSamplers(17);
    // Assert outside the script's catch path so unexpected success cannot pass.
    EXPECT_FALSE(result.IsReady);
    EXPECT_NE(result.Error.find("Vulkan shader uses more than 16 distinct sampler textures"), std::string::npos) << result.Error;
}

TEST(ShaderCompilation, VulkanAssignsMatchingVaryingLocations)
{
    Babylon::Plugins::ShaderCompiler compiler;
    for (unsigned explicitStages = 0; explicitStages < 4; ++explicitStages)
    {
        SCOPED_TRACE(explicitStages);
        const std::string vertexLocation = (explicitStages & 1) ? "layout(location = 3) " : "";
        const std::string fragmentLocation = (explicitStages & 2) ? "layout(location = 3) " : "";
        const auto info = compiler.Compile(
            "#version 310 es\nprecision highp float;\nin vec3 position;\n" +
                vertexLocation + R"(out vec3 colorValue;
                out vec2 extraValue;
                void main() {
                    gl_Position = vec4(position, 1.0);
                    colorValue = position;
                    extraValue = position.xy;
                })",
            "#version 310 es\nprecision highp float;\nin vec2 extraValue;\n" +
                fragmentLocation + R"(in vec3 colorValue;
                layout(location = 0) out vec4 color;
                void main() { color = vec4(colorValue.xy + extraValue, colorValue.z, 1.0); })");
        auto vertex = ReadVulkanShader(info.VertexBytes);
        auto fragment = ReadVulkanShader(info.FragmentBytes);
        const auto outputs = vertex.get_shader_resources().stage_outputs;
        const auto inputs = fragment.get_shader_resources().stage_inputs;
        ASSERT_EQ(outputs.size(), 2u);
        ASSERT_EQ(inputs.size(), 2u);
        EXPECT_NE(vertex.get_decoration(outputs[0].id, spv::DecorationLocation),
            vertex.get_decoration(outputs[1].id, spv::DecorationLocation));
        for (const auto& output : outputs)
        {
            ASSERT_TRUE(vertex.has_decoration(output.id, spv::DecorationLocation)) << output.name;
            bool matched{};
            for (const auto& input : inputs)
            {
                if (input.name == output.name)
                {
                    ASSERT_TRUE(fragment.has_decoration(input.id, spv::DecorationLocation)) << input.name;
                    EXPECT_EQ(vertex.get_decoration(output.id, spv::DecorationLocation),
                        fragment.get_decoration(input.id, spv::DecorationLocation));
                    if (explicitStages != 0 && output.name == "colorValue")
                    {
                        EXPECT_EQ(vertex.get_decoration(output.id, spv::DecorationLocation), 3u);
                    }
                    matched = true;
                }
            }
            EXPECT_TRUE(matched) << output.name;
        }
    }
}

TEST(ShaderCompilation, VulkanReservesMatrixArrayLocations)
{
    Babylon::Plugins::ShaderCompiler compiler;
    const auto info = compiler.Compile(
        R"(#version 310 es
            precision highp float;
            in vec3 position;
            out mat2 matrixValue[2];
            out vec3 colorValue;
            out vec2 vertexOnly;
            void main() {
                gl_Position = vec4(position, 1.0);
                matrixValue[0] = mat2(1.0);
                matrixValue[1] = mat2(2.0);
                colorValue = position;
                vertexOnly = position.xy;
            })",
        R"(#version 310 es
            precision highp float;
            layout(location = 3) in vec3 colorValue;
            in mat2 matrixValue[2];
            out vec4 color;
            void main() { color = vec4(matrixValue[0] * matrixValue[1] * colorValue.xy, colorValue.z, 1.0); })");
    auto vertex = ReadVulkanShader(info.VertexBytes);
    auto fragment = ReadVulkanShader(info.FragmentBytes);
    for (const auto* stage : {&vertex, &fragment})
    {
        const auto resources = stage->get_shader_resources();
        const auto& varyings = stage == &vertex ? resources.stage_outputs : resources.stage_inputs;
        ASSERT_EQ(varyings.size(), stage == &vertex ? 3u : 2u);
        for (const auto& varying : varyings)
        {
            ASSERT_TRUE(stage->has_decoration(varying.id, spv::DecorationLocation));
            const auto location = stage->get_decoration(varying.id, spv::DecorationLocation);
            if (varying.name == "matrixValue")
            {
                EXPECT_EQ(location, 4u);
            }
            else if (varying.name == "colorValue")
            {
                EXPECT_EQ(location, 3u);
            }
            else
            {
                EXPECT_EQ(varying.name, "vertexOnly");
                EXPECT_EQ(location, 0u);
            }
        }
    }
    const auto outputs = fragment.get_shader_resources().stage_outputs;
    ASSERT_EQ(outputs.size(), 1u);
    EXPECT_TRUE(fragment.has_decoration(outputs[0].id, spv::DecorationLocation));
    EXPECT_EQ(fragment.get_decoration(outputs[0].id, spv::DecorationLocation), 0u);
    const auto inputs = vertex.get_shader_resources().stage_inputs;
    ASSERT_EQ(inputs.size(), 1u);
    EXPECT_EQ(vertex.get_decoration(inputs[0].id, spv::DecorationLocation), 0u);
}

TEST(ShaderCompilation, VulkanUsesVertexAndInstanceIndexBuiltins)
{
    Babylon::Plugins::ShaderCompiler compiler;
    const auto info = compiler.Compile(
        R"(#version 310 es
            precision highp float;
            void main() { gl_Position = vec4(float(gl_VertexID), float(gl_InstanceID), 0.0, 1.0); })",
        R"(#version 310 es
            precision highp float;
            layout(location = 0) out vec4 color;
            void main() { color = vec4(1.0); })");
    auto vertex = ReadVulkanShader(info.VertexBytes);
    bool vertexIndex{};
    bool instanceIndex{};
    for (const auto& input : vertex.get_shader_resources().builtin_inputs)
    {
        EXPECT_NE(input.builtin, spv::BuiltInVertexId);
        EXPECT_NE(input.builtin, spv::BuiltInInstanceId);
        vertexIndex |= input.builtin == spv::BuiltInVertexIndex;
        instanceIndex |= input.builtin == spv::BuiltInInstanceIndex;
    }
    EXPECT_TRUE(vertexIndex);
    EXPECT_TRUE(instanceIndex);
}
#endif
