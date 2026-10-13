#include <gtest/gtest.h>
#include <gsl/span>

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
#include <cstring>
#include <cstdlib>
#include <future>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#if defined(BABYLON_NATIVE_GRAPHICS_API_D3D11)
#include <d3d11shader.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#endif

#if defined(BABYLON_NATIVE_GRAPHICS_API_VULKAN)
#include <Babylon/Plugins/ShaderCompiler.h>
#include <spirv_cross.hpp>
#endif

using namespace std::chrono_literals;

extern Babylon::Graphics::Configuration g_deviceConfig;

#if defined(BABYLON_NATIVE_GRAPHICS_API_D3D11) || defined(BABYLON_NATIVE_GRAPHICS_API_VULKAN)
#include <Babylon/Graphics/BgfxShaderInfo.h>
#include <string_view>

namespace
{
    gsl::span<const uint8_t> ReadFixtureShaderCode(const std::vector<uint8_t>& bytes)
    {
        // bgfx v13: 20-byte header, uint16 uniform count, uniform table, then uint32 code length.
        // The only uniform these fixtures may carry is the injected render target transform.
        constexpr size_t countOffset = 20;
        if (bytes.size() < countOffset + 2 || bytes[3] != 13)
        {
            throw std::runtime_error{"Expected a bgfx v13 shader"};
        }
        uint16_t uniformCount{};
        std::memcpy(&uniformCount, bytes.data() + countOffset, sizeof(uniformCount));
        size_t offset = countOffset + sizeof(uniformCount);
        for (uint16_t uniform = 0; uniform < uniformCount; ++uniform)
        {
            if (offset >= bytes.size())
            {
                throw std::runtime_error{"Invalid shader uniform table"};
            }
            const size_t nameSize = bytes[offset];
            if (offset + 1 + nameSize > bytes.size() ||
                std::string_view{reinterpret_cast<const char*>(bytes.data() + offset + 1), nameSize} != Babylon::Graphics::RENDER_TARGET_TRANSFORM_UNIFORM_NAME)
            {
                throw std::runtime_error{"Expected only the render target transform uniform"};
            }
            // name, type, count, register index, register count, texture component/dimension/format.
            offset += 1 + nameSize + 1 + 1 + 2 + 2 + 1 + 1 + 2;
        }
        const size_t codeOffset = offset + sizeof(uint32_t);
        if (bytes.size() < codeOffset)
        {
            throw std::runtime_error{"Invalid shader byte length"};
        }
        uint32_t codeSize{};
        std::memcpy(&codeSize, bytes.data() + offset, sizeof(codeSize));
        if (codeSize > bytes.size() - codeOffset)
        {
            throw std::runtime_error{"Invalid shader byte length"};
        }
        return {bytes.data() + codeOffset, codeSize};
    }
}
#endif

#if defined(HAS_SHADER_COMPILER) && (defined(BABYLON_NATIVE_GRAPHICS_API_D3D11) || defined(BABYLON_NATIVE_GRAPHICS_API_D3D12))
TEST(ShaderCompilation, SamplersReportStageAndDimension)
{
    // bgfx keys D3D sampler dimensions by regIndex and binds multisampled depth as
    // multisampled unless the sampler at that stage reports a non-multisampled dimension.
    Babylon::Plugins::ShaderCompiler compiler;
    const auto shader = compiler.Compile(
        "in vec3 position; void main() { gl_Position = vec4(position, 1.0); }",
        R"(
            precision highp float;
            uniform sampler2D first;
            uniform samplerCube cube;
            uniform sampler2D depth;
            out vec4 color;
            void main() {
                color = texture(first, vec2(0.5)) + texture(cube, vec3(1.0)) + texture(depth, vec2(0.5));
            }
        )");

    struct Sampler
    {
        uint16_t RegIndex;
        uint8_t TexDimension;
    };
    std::map<std::string, Sampler> samplers;
    const auto& bytes = shader.FragmentBytes;
    ASSERT_GE(bytes.size(), 22u);
    EXPECT_EQ(bytes[3], 13);
    size_t offset = 20;
    const auto read = [&](auto& value) {
        ASSERT_LE(offset + sizeof(value), bytes.size());
        std::memcpy(&value, bytes.data() + offset, sizeof(value));
        offset += sizeof(value);
    };
    uint16_t count{};
    read(count);
    for (uint16_t i = 0; i < count; ++i)
    {
        uint8_t nameSize{};
        read(nameSize);
        ASSERT_LE(offset + nameSize, bytes.size());
        const std::string name{reinterpret_cast<const char*>(bytes.data() + offset), nameSize};
        offset += nameSize;
        uint8_t type{}, num{}, texComponent{}, texDimension{};
        uint16_t regIndex{}, regCount{}, texFormat{};
        read(type);
        read(num);
        read(regIndex);
        read(regCount);
        read(texComponent);
        read(texDimension);
        read(texFormat);
        if ((type & 0x20) != 0) // BGFX_UNIFORM_SAMPLERBIT
        {
            samplers[name] = {regIndex, texDimension};
        }
    }

    ASSERT_EQ(samplers.size(), 3u);
    for (const auto& [name, sampler] : samplers)
    {
        SCOPED_TRACE(name);
        ASSERT_TRUE(shader.UniformStages.contains(name));
        EXPECT_EQ(sampler.RegIndex, shader.UniformStages.at(name));
    }
    EXPECT_NE(shader.UniformStages.at("depth"), 0);
    EXPECT_EQ(samplers.at("first").TexDimension, 0x02);
    EXPECT_EQ(samplers.at("cube").TexDimension, 0x04);
    EXPECT_EQ(samplers.at("depth").TexDimension, 0x02);
}
#endif

#if defined(HAS_SHADER_COMPILER) && defined(BABYLON_NATIVE_GRAPHICS_API_D3D11)
TEST(ShaderCompilation, InterfaceBlocksHaveMatchingD3D11Semantics)
{
#if !defined(HAS_SHADER_INTERFACE_BLOCKS)
    GTEST_SKIP() << "Interface blocks require BABYLON_NATIVE_DISABLE_WEBMIN";
#else
    Babylon::Plugins::ShaderCompiler compiler;
    const auto shader = compiler.Compile(R"(
        #extension GL_EXT_shader_io_blocks : require
        precision highp float;
        in vec3 position;
        out Payload { vec3 tint; } vertexData;
        void main() {
            gl_Position = vec4(position, 1.0);
            vertexData.tint = position;
        }
    )", R"(
        #extension GL_EXT_shader_io_blocks : require
        precision highp float;
        in Payload { vec3 tint; } fragmentData;
        out vec4 color;
        void main() { color = vec4(fragmentData.tint, 1.0); }
    )");
    const auto readSignature = [](const std::vector<uint8_t>& bytes, bool vertex) {
        const auto code = ReadFixtureShaderCode(bytes);
        Microsoft::WRL::ComPtr<ID3D11ShaderReflection> reflection;
        if (FAILED(D3DReflect(code.data(), code.size(), IID_PPV_ARGS(&reflection))))
        {
            throw std::runtime_error{"Could not reflect DXBC shader"};
        }
        D3D11_SHADER_DESC description{};
        if (FAILED(reflection->GetDesc(&description)))
        {
            throw std::runtime_error{"Could not read DXBC shader description"};
        }
        std::map<unsigned, unsigned> signature;
        const auto count = vertex ? description.OutputParameters : description.InputParameters;
        for (UINT i = 0; i < count; ++i)
        {
            D3D11_SIGNATURE_PARAMETER_DESC parameter{};
            const auto result = vertex ? reflection->GetOutputParameterDesc(i, &parameter) : reflection->GetInputParameterDesc(i, &parameter);
            if (FAILED(result))
            {
                throw std::runtime_error{"Could not read DXBC signature parameter"};
            }
            if (std::strcmp(parameter.SemanticName, "TEXCOORD") == 0)
            {
                EXPECT_TRUE(signature.emplace(parameter.SemanticIndex, parameter.Mask).second);
            }
        }
        return signature;
    };
    const auto outputs = readSignature(shader.VertexBytes, true);
    const auto inputs = readSignature(shader.FragmentBytes, false);
    ASSERT_EQ(outputs.size(), 1u);
    ASSERT_EQ(inputs.size(), 1u);
    EXPECT_EQ(outputs, inputs);
#endif
}
#endif

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

TEST(ShaderCompilation, UniformNamesDoNotCollideWithIntrinsicsOrBgfx)
{
    Babylon::Plugins::ShaderCompiler compiler{};
    const auto shader = compiler.Compile(R"(#version 310 es
        in vec2 position;
        uniform mat4 u_view;
        void main() { gl_Position = u_view * vec4(position, 0.0, 1.0); }
    )", R"(#version 310 es
        precision highp float;
        #define SIZE_NAME textureSize
        uniform vec2 SIZE_NAME;
        uniform mat4 u_view;
        uniform float bnUserUniform_textureSize;
        uniform highp sampler2D inputSampler;
        out vec4 color;
        void main() {
            color = vec4(SIZE_NAME / vec2(textureSize(inputSampler, 0)),
                u_view[0][0] + bnUserUniform_textureSize, 1.0);
        }
    )");
    ASSERT_EQ(shader.UniformNames.size(), 2u);
    EXPECT_EQ(shader.UniformNames.at("bnUserUniform_textureSize1"), "textureSize");
    EXPECT_EQ(shader.UniformNames.at("bnUserUniform_u_view"), "u_view");
    EXPECT_FALSE(shader.FragmentBytes.empty());
}

TEST(ShaderCompilation, UniformNamesDoNotCollideWithRenderTargetTransform)
{
    Babylon::Plugins::ShaderCompiler compiler{};
    const auto shader = compiler.Compile(R"(#version 310 es
        in vec2 position;
        uniform vec4 bnRenderTargetTransform;
        void main() { gl_Position = vec4(position, 0.0, 1.0) * bnRenderTargetTransform; }
    )", R"(#version 310 es
        precision highp float;
        uniform vec4 bnRenderTargetTransform;
        out vec4 color;
        void main() { color = vec4(gl_FragCoord.y) * bnRenderTargetTransform; }
    )");
    ASSERT_EQ(shader.UniformNames.size(), 1u);
    EXPECT_EQ(shader.UniformNames.at("bnUserUniform_bnRenderTargetTransform"), "bnRenderTargetTransform");
    EXPECT_FALSE(shader.VertexBytes.empty());
    EXPECT_FALSE(shader.FragmentBytes.empty());
}

#if defined(BABYLON_NATIVE_GRAPHICS_API_D3D11)
TEST(ShaderCompilation, UnsupportedSamplerArrayReportsError)
{
    Babylon::Plugins::ShaderCompiler compiler{};
    EXPECT_THROW(compiler.Compile(R"(
        precision highp float;
        in vec3 position;
        void main() { gl_Position = vec4(position, 1.0); }
    )", R"(
        precision highp float;
        uniform sampler2D sources[2];
        out vec4 color;
        void main() { color = texture(sources[0], vec2(0.5)) + texture(sources[1], vec2(0.5)); }
    )"), std::runtime_error);
}
#endif

TEST(ShaderCompilation, UnsignedShaderArgumentsAcceptIntegerConstants)
{
    Babylon::Plugins::ShaderCompiler compiler{};
    EXPECT_NO_THROW(compiler.Compile(R"(
        in vec2 position;
        void main() { gl_Position = vec4(position, 0.0, 1.0); }
    )", R"(
        precision highp float;
        precision highp int;
        #define NUM_SAMPLES 4
        out vec4 color;
        float sampleValue(uint index, uint count) { return float(index) / float(count); }
        void main() {
            float sum = 0.0;
            for (uint index = 0u; index < NUM_SAMPLES; ++index) {
                sum += sampleValue(index, NUM_SAMPLES);
            }
            color = vec4(sum);
        }
    )"));
}

TEST(ShaderCompilation, SamplerCoordinatesAcceptConditionalExpressions)
{
    Babylon::Plugins::ShaderCompiler compiler{};
    auto shader = compiler.Compile(
        R"(
            in vec2 position;
            void main() { gl_Position = vec4(position, 0.0, 1.0); }
        )",
        R"(
            precision highp float;
            uniform highp sampler3D volume;
            uniform highp sampler2DShadow shadow;
            uniform highp sampler2DArrayShadow shadowArray;
            uniform vec4 coordinate;
            layout(location = 0) out vec4 fragColor;
            void main()
            {
                vec3 alternate = coordinate.zyx;
                fragColor = texture(volume, coordinate.x > 0.0 ? coordinate.xyz : alternate);
                fragColor += textureLod(volume, coordinate.x > 0.0 ? alternate : coordinate.xyz, 0.0);
                fragColor += textureGrad(volume, coordinate.x > 0.0 ? coordinate.xyz : alternate, vec3(0.01), vec3(0.02));
                fragColor += texelFetch(volume, coordinate.x > 0.0 ? ivec3(coordinate.xyz) : ivec3(0), coordinate.x > 0.0 ? 0 : 1);
                fragColor += vec4(texture(shadow, coordinate.x > 0.0 ? alternate : coordinate.xyz));
                fragColor += vec4(texture(shadowArray, coordinate.x > 0.0 ? coordinate : coordinate.wzyx));
            }
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
        const auto code = ReadFixtureShaderCode(bytes);
        if (code.size() % sizeof(uint32_t) != 0)
        {
            throw std::runtime_error{"Invalid SPIR-V byte length"};
        }
        std::vector<uint32_t> words(code.size() / sizeof(uint32_t));
        std::memcpy(words.data(), code.data(), code.size());
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

TEST(ShaderCompilation, VulkanLinksBlocksWithDifferentInstanceNames)
{
#if defined(GLSLANG_WEB)
    GTEST_SKIP() << "Interface blocks require BABYLON_NATIVE_DISABLE_WEBMIN";
#else
    Babylon::Plugins::ShaderCompiler compiler;
    for (const std::string fragmentLocation : {"", "layout(location = 4) "})
    {
        SCOPED_TRACE(fragmentLocation);
        const auto info = compiler.Compile(
            R"(#version 310 es
                #extension GL_EXT_shader_io_blocks : require
                precision highp float;
                in vec3 position;
                out Payload { mat2 transform; vec3 tint; } vertexData;
                out vec2 extraValue;
                void main() {
                    gl_Position = vec4(position, 1.0);
                    vertexData.transform = mat2(1.0);
                    vertexData.tint = position;
                    extraValue = position.xy;
                })",
            "#version 310 es\n#extension GL_EXT_shader_io_blocks : require\n"
            "precision highp float;\nin vec2 extraValue;\n" +
                fragmentLocation + R"(in Payload { mat2 transform; vec3 tint; } fragmentData;
                layout(location = 0) out vec4 color;
                void main() { color = vec4(fragmentData.transform * extraValue, fragmentData.tint.z, 1.0); })");
        auto vertex = ReadVulkanShader(info.VertexBytes);
        auto fragment = ReadVulkanShader(info.FragmentBytes);
        std::map<std::string, unsigned> vertexLocations;
        std::map<std::string, unsigned> fragmentLocations;
        for (const auto* stage : {&vertex, &fragment})
        {
            const auto resources = stage->get_shader_resources();
            const auto& varyings = stage == &vertex ? resources.stage_outputs : resources.stage_inputs;
            ASSERT_EQ(varyings.size(), 2u);
            auto& locations = stage == &vertex ? vertexLocations : fragmentLocations;
            for (const auto& varying : varyings)
            {
                ASSERT_TRUE(stage->has_decoration(varying.id, spv::DecorationLocation));
                locations[varying.name] = stage->get_decoration(varying.id, spv::DecorationLocation);
            }
        }
        ASSERT_EQ(vertexLocations.count("Payload"), 1u);
        ASSERT_EQ(vertexLocations.count("extraValue"), 1u);
        EXPECT_EQ(vertexLocations, fragmentLocations);
        const auto blockLocation = vertexLocations.at("Payload");
        const auto extraLocation = vertexLocations.at("extraValue");
        EXPECT_TRUE(extraLocation < blockLocation || extraLocation >= blockLocation + 3);
        if (!fragmentLocation.empty())
        {
            EXPECT_EQ(blockLocation, 4u);
        }
    }
#endif
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
