#include <gtest/gtest.h>
#include <gsl/util>

#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Graphics/DeviceContext.h>
#include <Babylon/Graphics/Texture.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/Window.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <Babylon/Plugins/ShaderCache.h>
#include <Babylon/Plugins/ExternalTexture.h>
#include <Babylon/ScriptLoader.h>
#include <napi/pointer.h>

#include "Helpers.h"

#include <atomic>
#include <array>
#include <chrono>
#include <cstdlib>
#include <future>
#include <functional>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

extern Babylon::Graphics::Configuration g_deviceConfig;

// Shader-visible coordinates use GL's bottom-left origin on every backend.
namespace
{
    class TestCompletion
    {
    public:
        std::future<void> GetFuture()
        {
            return m_promise.get_future();
        }

        void Complete(std::exception_ptr error = {})
        {
            if (!m_completed.exchange(true))
            {
                if (error)
                {
                    m_promise.set_exception(std::move(error));
                }
                else
                {
                    m_promise.set_value();
                }
            }
        }

    private:
        std::promise<void> m_promise;
        std::atomic<bool> m_completed{};
    };

    std::string GetTestErrorString(const Napi::Error& error)
    {
        const auto message = error.Message();
        const auto stack = Napi::GetErrorString(error);
        return stack.find(message) == std::string::npos ? message + "\n" + stack : stack;
    }

    std::exception_ptr CaptureTestException()
    {
        try
        {
            throw;
        }
        catch (const Napi::Error& error)
        {
            // Do not retain a JS-backed exception beyond the runtime's lifetime.
            return std::make_exception_ptr(std::runtime_error{GetTestErrorString(error)});
        }
        catch (...)
        {
            return std::current_exception();
        }
    }

    void WaitForTestCompletion(std::future<void>& future, std::chrono::milliseconds timeout, const char* timeoutMessage)
    {
        if (future.wait_for(timeout) != std::future_status::ready)
        {
            throw std::runtime_error{timeoutMessage};
        }
        future.get();
    }

    // Renders a full-screen quad into a width x height render target, returning RGBA8
    // pixels in memory order. The fragment shader may declare `uniform vec2 targetSize`
    // and, when withInputTexture is set, `uniform sampler2D inputSampler` bound to a
    // texture whose row y holds makeRow(y).
    std::vector<uint8_t> RenderFullScreenQuad(
        uint32_t width,
        uint32_t height,
        const std::string& vertexShader,
        const std::string& fragmentShader,
        bool withInputTexture,
        const std::string& setupScript = {},
        std::chrono::milliseconds renderTimeout = std::chrono::seconds{30},
        const std::function<void(Napi::Env)>& nativeSetup = {})
    {
        // Use clip-space geometry and bottom-left-origin UVs.
        Babylon::Graphics::Device device{g_deviceConfig};
        Babylon::Graphics::TextureT outputTexture{};
        const auto releaseOutput = gsl::finally([&outputTexture] {
            if (outputTexture)
            {
                Helpers::DestroyTexture(outputTexture);
            }
        });
        bool frameOpen{};
        const auto finishFrame = gsl::finally([&device, &frameOpen] {
            if (frameOpen)
            {
                device.FinishRenderingCurrentFrame();
            }
        });
        device.StartRenderingCurrentFrame();
        frameOpen = true;

        outputTexture = Helpers::CreateTexture(
            device.GetPlatformInfo().Device, width, height, 1, true);
        Babylon::Plugins::ExternalTexture outputExternalTexture{outputTexture};

        auto startupDone = std::make_shared<TestCompletion>();
        auto renderDone = std::make_shared<TestCompletion>();
        auto startupFuture = startupDone->GetFuture();
        auto renderFuture = renderDone->GetFuture();
        Babylon::AppRuntime::Options options{};
        options.UnhandledExceptionHandler = [startupDone, renderDone](const Napi::Error& error) {
            std::cerr << "[Uncaught Error] " << GetTestErrorString(error) << std::endl;
            std::cerr.flush();
            auto exception = std::make_exception_ptr(std::runtime_error{GetTestErrorString(error)});
            startupDone->Complete(exception);
            renderDone->Complete(exception);
        };

        Babylon::AppRuntime runtime{options};
        // Late callbacks must be able to acquire a frame scope while runtime teardown joins them.
        const auto reopenFrame = gsl::finally([&device, &frameOpen] {
            if (!frameOpen)
            {
                device.StartRenderingCurrentFrame();
                frameOpen = true;
            }
        });
        runtime.Dispatch([&device, &nativeSetup](Napi::Env env) {
            env.Global().Set("globalThis", env.Global());
            device.AddToJavaScript(env);
            if (nativeSetup)
            {
                nativeSetup(env);
            }

            Babylon::Polyfills::Console::Initialize(env, [](const char* message, auto) {
                std::cout << message << std::endl;
            });
            Babylon::Polyfills::Window::Initialize(env);
            Babylon::Plugins::NativeEngine::Initialize(env);
        });

        Babylon::ScriptLoader loader{runtime};
        loader.LoadScript("app:///Assets/babylon.max.js");

        const std::string script = R"(
            (function () {
                var vertexShader = VERTEX_SHADER_SOURCE;

                var fragmentShader = FRAGMENT_SHADER_SOURCE;

                globalThis.startup = function (outputNativeTexture, width, height) {
                    var engine = new BABYLON.NativeEngine();
                    engine.getCaps().parallelShaderCompile = null;
                    var scene = new BABYLON.Scene(engine);
                    scene.autoClear = true;
                    scene.clearColor = new BABYLON.Color4(0, 0, 0, 1);

                    var outputTexture = new BABYLON.RenderTargetTexture(
                        "output",
                        { width: width, height: height },
                        scene,
                        {
                            colorAttachment: engine.wrapNativeTexture(outputNativeTexture),
                            generateDepthBuffer: true,
                            generateStencilBuffer: false
                        });

                    var camera = new BABYLON.FreeCamera("camera", new BABYLON.Vector3(0, 0, -1), scene);
                    camera.setTarget(BABYLON.Vector3.Zero());
                    camera.mode = BABYLON.Camera.ORTHOGRAPHIC_CAMERA;
                    camera.orthoTop = 1;
                    camera.orthoBottom = -1;
                    camera.orthoLeft = -1;
                    camera.orthoRight = 1;
                    camera.outputRenderTarget = outputTexture;

                    // Cover the target without a projection matrix.
                    var quad = new BABYLON.Mesh("quad", scene);
                    var vertexData = new BABYLON.VertexData();
                    vertexData.positions = [
                        -1, -1, 0,
                         1, -1, 0,
                         1,  1, 0,
                        -1,  1, 0
                    ];
                    vertexData.uvs = [
                        0, 0,
                        1, 0,
                        1, 1,
                        0, 1
                    ];
                    vertexData.indices = [0, 1, 2, 0, 2, 3];
                    vertexData.applyToMesh(quad);
                    quad.alwaysSelectAsActiveMesh = true;

                    var material = new BABYLON.ShaderMaterial(
                        "fragCoordShader",
                        scene,
                        { vertexSource: vertexShader, fragmentSource: fragmentShader },
                        {
                            attributes: vertexShader.indexOf("attribute vec2 uv") !== -1
                                ? ["position", "uv"]
                                : ["position"],
                            uniforms: ["targetSize"],
                            samplers: WITH_INPUT_TEXTURE ? ["inputSampler"] : []
                        });
                    material.onError = function (_effect, errors) {
                        console.error("ShaderMaterial compilation error: " + errors);
                    };
                    material.backFaceCulling = false;
                    material.depthFunction = BABYLON.Constants.ALWAYS;
                    material.setVector2("targetSize", new BABYLON.Vector2(width, height));

                    if (WITH_INPUT_TEXTURE) {
                        // A red ramp detects mirrors; row-index bits in blue detect off-by-one errors.
                        var data = new Uint8Array(width * height * 4);
                        for (var y = 0; y < height; ++y) {
                            for (var x = 0; x < width; ++x) {
                                var i = (y * width + x) * 4;
                                data[i] = 255 - y * 4;
                                data[i + 1] = 0;
                                data[i + 2] = y * 4;
                                data[i + 3] = 255;
                            }
                        }
                        var raw = engine.createRawTexture(
                            data,
                            width,
                            height,
                            BABYLON.Constants.TEXTUREFORMAT_RGBA,
                            false /* generateMipMaps */,
                            false /* invertY */,
                            BABYLON.Constants.TEXTURE_NEAREST_SAMPLINGMODE);
                        var wrapper = new BABYLON.Texture(null, scene);
                        wrapper._texture = raw;
                        wrapper.wrapU = BABYLON.Constants.TEXTURE_CLAMP_ADDRESSMODE;
                        wrapper.wrapV = BABYLON.Constants.TEXTURE_CLAMP_ADDRESSMODE;
                        material.setTexture("inputSampler", wrapper);
                    }

                    quad.material = material;
                    SETUP_SCRIPT
                    globalThis.__scene = scene;
                };

                globalThis.render = function () {
                    var scene = globalThis.__scene;
                    var preparation = Promise.resolve().then(function () {
                        return globalThis.__prepare ? globalThis.__prepare() : undefined;
                    });
                    return preparation.then(function () {
                        return scene.whenReadyAsync();
                    }).then(function () {
                        scene.render();
                    });
                };
            })();
        )";

        // Inject the caller's shaders as JS string literals.
        const auto toJsStringLiteral = [](const std::string& source) {
            std::string result = "\"";
            for (char c : source)
            {
                if (c == '\n')
                {
                    result += "\\n";
                }
                else if (c == '"')
                {
                    result += "\\\"";
                }
                else if (c == '\\')
                {
                    result += "\\\\";
                }
                else
                {
                    result += c;
                }
            }
            result += "\"";
            return result;
        };

        const auto replaceToken = [](std::string& text, const std::string& token, const std::string& value) {
            for (size_t pos = text.find(token); pos != std::string::npos; pos = text.find(token, pos))
            {
                text.replace(pos, token.size(), value);
                pos += value.size();
            }
        };

        std::string finalScript = script;
        replaceToken(finalScript, "VERTEX_SHADER_SOURCE", toJsStringLiteral(vertexShader));
        replaceToken(finalScript, "FRAGMENT_SHADER_SOURCE", toJsStringLiteral(fragmentShader));
        replaceToken(finalScript, "WITH_INPUT_TEXTURE", withInputTexture ? "true" : "false");
        replaceToken(finalScript, "SETUP_SCRIPT", setupScript);

        loader.Eval(finalScript, "frag_coord_orientation_test.js");

        loader.Dispatch([&outputExternalTexture, startupDone, width, height](Napi::Env env) {
            try
            {
                auto jsOutput = outputExternalTexture.CreateForJavaScript(env);
                env.Global().Get("startup").As<Napi::Function>().Call({
                    jsOutput,
                    Napi::Number::New(env, width),
                    Napi::Number::New(env, height),
                });
                startupDone->Complete();
            }
            catch (...)
            {
                startupDone->Complete(CaptureTestException());
            }
        });
        WaitForTestCompletion(startupFuture, std::chrono::seconds{30}, "quad setup dispatch timed out");

        device.FinishRenderingCurrentFrame();
        frameOpen = false;
        device.StartRenderingCurrentFrame();
        frameOpen = true;

        loader.Dispatch([renderDone](Napi::Env env) {
            try
            {
                auto jsPromise = env.Global().Get("render").As<Napi::Function>().Call({}).As<Napi::Promise>();

                auto jsOnFulfilled = Napi::Function::New(env, [renderDone](const Napi::CallbackInfo&) {
                    renderDone->Complete();
                });
                auto jsOnRejected = Napi::Function::New(env, [renderDone](const Napi::CallbackInfo& info) {
                    try
                    {
                        const auto reason = info[0];
                        const bool isError = reason.IsObject() &&
                                             reason.As<Napi::Object>().InstanceOf(info.Env().Global().Get("Error").As<Napi::Function>());
                        const auto message = isError
                                                 ? GetTestErrorString(reason.As<Napi::Error>())
                                                 : reason.ToString().Utf8Value();
                        renderDone->Complete(std::make_exception_ptr(std::runtime_error{message}));
                    }
                    catch (...)
                    {
                        renderDone->Complete(CaptureTestException());
                    }
                });

                jsPromise.Get("then").As<Napi::Function>().Call(jsPromise, {jsOnFulfilled, jsOnRejected});
            }
            catch (...)
            {
                renderDone->Complete(CaptureTestException());
            }
        });

        const auto deadline = std::chrono::steady_clock::now() + renderTimeout;
        while (renderFuture.wait_for(std::chrono::milliseconds{16}) != std::future_status::ready)
        {
            if (std::chrono::steady_clock::now() >= deadline)
            {
                throw std::runtime_error{"quad preparation/render timed out"};
            }
            device.FinishRenderingCurrentFrame();
            frameOpen = false;
            device.StartRenderingCurrentFrame();
            frameOpen = true;
        }
        renderFuture.get();

        device.FinishRenderingCurrentFrame();
        frameOpen = false;

        return Helpers::ReadPixels(device.GetPlatformInfo(), outputTexture, width, height);
    }

    std::vector<uint8_t> RenderVolumeQuad(const std::string& fragmentShader)
    {
        const auto initializeVolume = [](Napi::Env env) {
            env.Global().Set("initializeTestVolume", Napi::Function::New(env, [](const Napi::CallbackInfo& info) {
                auto& context = Babylon::Graphics::DeviceContext::GetFromJavaScript(info.Env());
                auto scope = context.AcquireFrameCompletionScope();
                auto* texture = info[0].As<Napi::Pointer<Babylon::Graphics::Texture>>().Get();
                texture->Create3D(2, 2, 2, false, bgfx::TextureFormat::RGBA8, BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
                const std::array<uint8_t, 32> pixels{
                    64, 128, 192, 255, 64, 128, 192, 255, 64, 128, 192, 255, 64, 128, 192, 255,
                    64, 128, 192, 255, 64, 128, 192, 255, 64, 128, 192, 255, 64, 128, 192, 255};
                bgfx::updateTexture3D(texture->Handle(), 0, 0, 0, 0, 2, 2, 2, bgfx::copy(pixels.data(), static_cast<uint32_t>(pixels.size())));
            }));
        };
        return RenderFullScreenQuad(2, 2,
            "attribute vec3 position; void main() { gl_Position = vec4(position, 1.0); }",
            fragmentShader, false, R"(
                var raw = BABYLON.RawTexture.CreateRGBATexture(new Uint8Array([0, 0, 0, 255]),
                    1, 1, scene, false, false, BABYLON.Texture.NEAREST_SAMPLINGMODE);
                initializeTestVolume(raw.getInternalTexture()._hardwareTexture.underlyingResource);
                raw.getInternalTexture().is3D = true;
                material.setTexture("volume", raw);
            )", std::chrono::seconds{30}, initializeVolume);
    }
}

TEST(ShaderCompilation, UniformNamesSurviveNativeBindingAndCache)
{
#if defined(SKIP_EXTERNAL_TEXTURE_TESTS) || defined(SKIP_RENDER_TESTS)
    GTEST_SKIP();
#else
    Babylon::Plugins::ShaderCache::Enable();
    Babylon::Plugins::ShaderCache::Clear();
    const auto disableCache = gsl::finally([] {
        Babylon::Plugins::ShaderCache::Clear();
        Babylon::Plugins::ShaderCache::Disable();
    });
    const auto render = [] {
        return RenderFullScreenQuad(4, 4,
            "attribute vec3 position; void main() { gl_Position = vec4(position, 1.0); }",
            R"(
                precision highp float;
                uniform highp sampler2D inputSampler;
                uniform vec2 textureSize;
                uniform mat4 u_view;
                uniform float bnUserUniform_textureSize;
                uniform float bnSamplerState_inputSampler;
                void main() {
                    gl_FragColor = vec4(textureSize.x / float(textureSize(inputSampler, 0).x),
                        u_view[0][0], bnUserUniform_textureSize * bnSamplerState_inputSampler,
                        texture2D(inputSampler, vec2(0.5)).a);
                }
            )", true, R"(
                material.setVector2("textureSize", new BABYLON.Vector2(1, 2));
                material.setMatrix("u_view", BABYLON.Matrix.Scaling(0.5, 0.5, 0.5));
                material.setFloat("bnUserUniform_textureSize", 0.75);
                material.setFloat("bnSamplerState_inputSampler", 1.0);
            )");
    };
    const auto original = render();
    ASSERT_EQ(original.size(), 4u * 4u * 4u);
    for (size_t offset = 0; offset < original.size(); offset += 4)
    {
        EXPECT_NEAR(original[offset], 64, 1);
        EXPECT_NEAR(original[offset + 1], 128, 1);
        EXPECT_NEAR(original[offset + 2], 191, 1);
        EXPECT_EQ(original[offset + 3], 255);
    }
    std::stringstream stream{std::ios::in | std::ios::out | std::ios::binary};
    const auto count = Babylon::Plugins::ShaderCache::Save(stream);
    ASSERT_GT(count, 0u);
    Babylon::Plugins::ShaderCache::Clear();
    EXPECT_EQ(Babylon::Plugins::ShaderCache::Load(stream), count);
    EXPECT_EQ(render(), original);
    std::stringstream after;
    EXPECT_EQ(Babylon::Plugins::ShaderCache::Save(after), count);
#endif
}

TEST(ShaderCompilation, FragCoordSetupAndPreparationFailuresPropagate)
{
#if defined(SKIP_EXTERNAL_TEXTURE_TESTS) || defined(SKIP_RENDER_TESTS)
    GTEST_SKIP();
#else
    const std::string vertexShader =
        "precision highp float;\n"
        "attribute vec3 position;\n"
        "void main(void) { gl_Position = vec4(position, 1.0); }\n";
    const std::string fragmentShader =
        "precision highp float;\n"
        "void main(void) { gl_FragColor = vec4(1.0); }\n";
    struct FailureCase
    {
        std::string SetupScript;
        std::string ExpectedError;
        std::chrono::milliseconds Timeout{std::chrono::seconds{30}};
    };
    const std::vector<FailureCase> cases{
        {"throw new Error('setup hook failure');", "setup hook failure"},
        {"globalThis.__prepare = function () { throw new Error('synchronous preparation failure'); };", "synchronous preparation failure"},
        {"globalThis.__prepare = function () { return Promise.reject(new Error('asynchronous preparation failure')); };", "asynchronous preparation failure"},
        {"globalThis.__prepare = function () { return Promise.reject('non-Error rejection'); };", "non-Error rejection"},
        {"globalThis.__prepare = function () { return new Promise(function () {}); };", "quad preparation/render timed out", std::chrono::milliseconds{100}},
        {"globalThis.__prepare = function () { return new Promise(function (resolve) { setTimeout(resolve, 300); }); };", "quad preparation/render timed out", std::chrono::milliseconds{100}},
    };
    for (const auto& [setupScript, expectedError, timeout] : cases)
    {
        SCOPED_TRACE(setupScript);
        try
        {
            RenderFullScreenQuad(1, 1, vertexShader, fragmentShader, false, setupScript, timeout);
            FAIL() << "expected setup/preparation failure";
        }
        catch (const std::runtime_error& error)
        {
            EXPECT_NE(std::string{error.what()}.find(expectedError), std::string::npos) << error.what();
        }
    }
#endif
}

TEST(ShaderCompilation, MultiRowMorphTextureMatchesVertexAttributes)
{
#if defined(SKIP_EXTERNAL_TEXTURE_TESTS) || defined(SKIP_RENDER_TESTS)
    GTEST_SKIP();
#else
    const std::string vertexShader = R"(
        attribute vec3 position;
        #include<morphTargetsVertexGlobalDeclaration>
        #include<morphTargetsVertexDeclaration>[0..maxSimultaneousMorphTargets]
        void main()
        {
            vec3 positionUpdated = position;
            #include<morphTargetsVertexGlobal>
            #include<morphTargetsVertex>[0..maxSimultaneousMorphTargets]
            gl_Position = vec4(positionUpdated, 1.0);
        }
    )";
    const std::string fragmentShader =
        "precision highp float;\n"
        "void main() { gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0); }\n";
    std::vector<uint8_t> expected;
    for (bool useTexture : {false, true})
    {
        const std::string setup = "var useTexture = " + std::string{useTexture ? "true" : "false"} + R"(;
            var caps = engine.getCaps();
            var maxTextureSize = caps.maxTextureSize;
            caps.maxTextureSize = 2;
            try {
                var manager = new BABYLON.MorphTargetManager(scene);
                manager.useTextureToStoreTargets = useTexture;
                manager.areUpdatesFrozen = true;
                var first = new BABYLON.MorphTarget("first", 0.25, scene);
                first.setPositions(new Float32Array([
                    -0.75, -0.875, 0, 0.875, -0.75, 0,
                     0.5,   0.75, 0,  -0.5, 0.875, 0
                ]));
                var second = new BABYLON.MorphTarget("second", 0.5, scene);
                second.setPositions(new Float32Array([
                    -0.875, -0.5, 0,   0.5, -0.875, 0,
                     0.875,  0.5, 0, -0.75,   0.75, 0
                ]));
                manager.addTarget(first);
                manager.addTarget(second);
                manager.areUpdatesFrozen = false;
                quad.morphTargetManager = manager;
                if (useTexture && (!manager.isUsingTextureForTargets ||
                    manager._textureWidth !== 2 || manager._textureHeight !== 2)) {
                    throw new Error("Expected a two-layer, multi-row morph texture");
                }
            } finally {
                caps.maxTextureSize = maxTextureSize;
            }
        )";
        auto pixels = RenderFullScreenQuad(32, 32, vertexShader, fragmentShader, false, setup);
        if (useTexture)
        {
            EXPECT_EQ(pixels, expected);
        }
        else
        {
            expected = std::move(pixels);
            ASSERT_EQ(expected.size(), 32u * 32u * 4u);
            EXPECT_EQ(expected[0], 0);
            EXPECT_EQ(expected[(16 * 32 + 16) * 4], 255);
        }
    }
#endif
}

TEST(ShaderCompilation, NearestSamplerBoundariesPreserveLinearFilteringAndWrapModes)
{
#if defined(SKIP_EXTERNAL_TEXTURE_TESTS) || defined(SKIP_RENDER_TESTS) || defined(SKIP_FLIPPED_SAMPLING_TESTS)
    GTEST_SKIP();
#else
    const std::array<std::array<uint8_t, 7>, 3> point{{
        {32, 32, 64, 128, 224, 224, 224},
        {224, 32, 64, 128, 224, 32, 64},
        {32, 32, 64, 128, 224, 224, 128},
    }};
    const std::array<std::array<uint8_t, 7>, 3> linear{{
        {32, 32, 48, 96, 176, 224, 224},
        {176, 128, 48, 96, 176, 128, 48},
        {48, 32, 48, 96, 176, 224, 176},
    }};
    for (int wrap = 0; wrap < 3; ++wrap)
    {
        for (const std::string sample : {"texture2D(source, uv)", "textureLod(source, uv, 0.0)",
                 "textureGrad(source, uv, vec2(0.0), vec2(0.0))"})
        {
            SCOPED_TRACE(std::to_string(wrap) + ": " + sample);
            const auto pixels = RenderFullScreenQuad(7, 1,
                "attribute vec3 position; void main() { gl_Position = vec4(position, 1.0); }",
                R"(
                    precision highp float;
                    uniform highp sampler2D pointSampler, linearSampler;
                    int calls = 0;
                    vec2 nextCoordinate() {
                        calls++;
                        return vec2(0.5, (floor(gl_FragCoord.x) - 1.0) / 4.0);
                    }
                    vec4 innerSample(sampler2D source, vec2 uv) { return )" + sample + R"(; }
                    vec4 outerSample(sampler2D source, vec2 uv) { return innerSample(source, uv); }
                    void main() {
                        vec2 uv = vec2(0.5, (floor(gl_FragCoord.x) - 1.0) / 4.0);
                        vec4 point = outerSample(pointSampler, nextCoordinate());
                        vec4 linear = outerSample(linearSampler, uv);
                        gl_FragColor = vec4(point.r, linear.r, float(calls) * 0.25, 1.0);
                    }
                )", false,
                "var wrap = " + std::to_string(wrap) + R"(;
                    var data = new Uint8Array([32,0,0,255, 64,0,0,255, 128,0,0,255, 224,0,0,255]);
                    for (var i = 0; i < 2; ++i) {
                        var texture = BABYLON.RawTexture.CreateRGBATexture(data, 1, 4, scene, false, false,
                            i ? BABYLON.Texture.BILINEAR_SAMPLINGMODE : BABYLON.Texture.NEAREST_SAMPLINGMODE);
                        texture.wrapV = wrap;
                        material.setTexture(i ? "linearSampler" : "pointSampler", texture);
                    }
                )");
            ASSERT_EQ(pixels.size(), 28u);
            for (size_t index = 0; index < 7; ++index)
            {
                EXPECT_EQ(pixels[index * 4], point[wrap][index]);
                EXPECT_NEAR(pixels[index * 4 + 1], linear[wrap][index], 1);
                EXPECT_NEAR(pixels[index * 4 + 2], 64, 1);
                EXPECT_EQ(pixels[index * 4 + 3], 255);
            }
        }
    }
#endif
}

TEST(ShaderCompilation, VolumeCoordinateSideEffectsExecuteOnce)
{
#if defined(SKIP_EXTERNAL_TEXTURE_TESTS) || defined(SKIP_RENDER_TESTS)
    GTEST_SKIP();
#else
    for (const std::string coordinate : {"nextCoordinate()", "targetSize.x > 0.0 ? nextCoordinate() : vec3(0.0)"})
    {
        SCOPED_TRACE(coordinate);
        const std::string fragmentShader = R"(
            precision highp float;
            uniform highp sampler3D volume;
            uniform vec2 targetSize;
            int calls = 0;
            vec3 nextCoordinate() { calls++; return vec3(0.25, 0.75, 0.25); }
            void main()
            {
                vec4 sampled = texture(volume, )" + coordinate + R"();
                gl_FragColor = vec4(float(calls) * 0.25, sampled.g, sampled.b, 1.0);
            }
        )";
        const auto pixels = RenderVolumeQuad(fragmentShader);
        ASSERT_EQ(pixels.size(), 16u);
        for (size_t i = 0; i < pixels.size(); i += 4)
        {
            EXPECT_NEAR(pixels[i], 64, 1);
            EXPECT_EQ(pixels[i + 1], 128);
            EXPECT_EQ(pixels[i + 2], 192);
            EXPECT_EQ(pixels[i + 3], 255);
        }
    }
#endif
}

TEST(ShaderCompilation, RawArraySamplingPreservesRowsAndLayers)
{
#if defined(SKIP_EXTERNAL_TEXTURE_TESTS) || defined(SKIP_RENDER_TESTS) || defined(SKIP_FLIPPED_SAMPLING_TESTS)
    GTEST_SKIP();
#else
    for (const std::string sample : {
             "texture(arrayTexture, (vec3(coord) + vec3(0.5, 0.5, 0.0)) / vec3(2.0, 2.0, 1.0))",
             "texelFetch(arrayTexture, coord, 0)",
             "textureGrad(arrayTexture, (vec3(coord) + vec3(0.5, 0.5, 0.0)) / vec3(2.0, 2.0, 1.0), vec2(0.0), vec2(0.0))"})
    {
        SCOPED_TRACE(sample);
        const std::string fragmentShader = R"(
            precision highp float;
            uniform highp sampler2DArray arrayTexture;
            void main() {
                ivec3 coord = ivec3(int(gl_FragCoord.x) % 2, int(gl_FragCoord.y), int(gl_FragCoord.x) / 2);
                gl_FragColor = )" + sample + R"(;
            }
        )";
        const auto pixels = RenderFullScreenQuad(4, 2,
            "attribute vec3 position; void main() { gl_Position = vec4(position, 1.0); }",
            fragmentShader, false, R"(
                var data = new Uint8Array(32);
                for (var layer = 0; layer < 2; ++layer) {
                    for (var y = 0; y < 2; ++y) {
                        for (var x = 0; x < 2; ++x) {
                            data.set([64 + layer * 128, 64 + y * 128, 32 + x * 64, 255],
                                ((layer * 2 + y) * 2 + x) * 4);
                        }
                    }
                }
                var original = Array.from(data).join(",");
                var array = BABYLON.RawTexture2DArray.CreateRGBATexture(
                    data, 2, 2, 2, scene, false, false, BABYLON.Texture.NEAREST_SAMPLINGMODE);
                if (Array.from(data).join(",") !== original) {
                    throw new Error("Array upload modified its source");
                }
                material.setTexture("arrayTexture", array);
            )");
        ASSERT_EQ(pixels.size(), 32u);
        for (uint32_t row = 0; row < 2; ++row)
        {
            for (uint32_t x = 0; x < 4; ++x)
            {
                const auto offset = (row * 4 + x) * 4;
                EXPECT_EQ(pixels[offset], 64 + (x / 2) * 128);
                EXPECT_EQ(pixels[offset + 1], 64 + (1 - row) * 128);
                EXPECT_EQ(pixels[offset + 2], 32 + (x % 2) * 64);
                EXPECT_EQ(pixels[offset + 3], 255);
            }
        }
    }
#endif
}

TEST(ShaderCompilation, CompressedArraySamplingPreservesPartialRowsAndLayers)
{
#if defined(SKIP_EXTERNAL_TEXTURE_TESTS) || defined(SKIP_RENDER_TESTS) || defined(SKIP_FLIPPED_SAMPLING_TESTS)
    GTEST_SKIP();
#else
    for (const std::string sample : {
             "texture(arrayTexture, vec3(0.5, gl_FragCoord.y / 6.0, floor(gl_FragCoord.x)))",
             "texelFetch(arrayTexture, ivec3(1, int(gl_FragCoord.y), int(gl_FragCoord.x)), 0)"})
    {
        SCOPED_TRACE(sample);
        const auto pixels = RenderFullScreenQuad(2, 6,
            "attribute vec3 position; void main() { gl_Position = vec4(position, 1.0); }",
            "precision highp float; uniform highp sampler2DArray arrayTexture; void main() { gl_FragColor = " + sample + "; }",
            false, R"(
                var data = new Uint8Array([
                    0x00, 0xf8, 0x00, 0xf8, 0, 0, 0, 0,
                    0xe0, 0x07, 0xe0, 0x07, 0, 0, 0, 0,
                    0x1f, 0x00, 0x1f, 0x00, 0, 0, 0, 0,
                    0xff, 0xff, 0xff, 0xff, 0, 0, 0, 0
                ]);
                var original = Array.from(data).join(",");
                var array = new BABYLON.RawTexture2DArray(
                    data, 3, 6, 2, BABYLON.Constants.TEXTUREFORMAT_COMPRESSED_RGB_S3TC_DXT1,
                    scene, false, false, BABYLON.Texture.NEAREST_SAMPLINGMODE);
                if (Array.from(data).join(",") !== original) {
                    throw new Error("Compressed array upload modified its source");
                }
                material.setTexture("arrayTexture", array);
            )");
        ASSERT_EQ(pixels.size(), 48u);
        for (uint32_t row = 0; row < 6; ++row)
        {
            const auto offset = row * 8;
            EXPECT_EQ(pixels[offset], row < 2 ? 0 : 255);
            EXPECT_EQ(pixels[offset + 1], row < 2 ? 255 : 0);
            EXPECT_EQ(pixels[offset + 2], 0);
            EXPECT_EQ(pixels[offset + 3], 255);
            EXPECT_EQ(pixels[offset + 4], row < 2 ? 255 : 0);
            EXPECT_EQ(pixels[offset + 5], row < 2 ? 255 : 0);
            EXPECT_EQ(pixels[offset + 6], 255);
            EXPECT_EQ(pixels[offset + 7], 255);
        }
    }
#endif
}

TEST(ShaderCompilation, IntegerVolumeCoordinatesExecuteOnce)
{
#if defined(SKIP_EXTERNAL_TEXTURE_TESTS) || defined(SKIP_RENDER_TESTS)
    GTEST_SKIP();
#else
    for (const std::string coordinate : {"nextCoordinate()", "targetSize.x > 0.0 ? nextCoordinate() : ivec3(0)"})
    {
        SCOPED_TRACE(coordinate);
        const auto pixels = RenderVolumeQuad(R"(
            precision highp float;
            precision highp int;
            uniform highp sampler3D volume;
            uniform vec2 targetSize;
            int calls = 0;
            int lodCalls = 0;
            ivec3 nextCoordinate() { calls++; return ivec3(0, 1, 0); }
            int nextLod() { lodCalls++; return 0; }
            void main()
            {
                vec4 sampled = texelFetch(volume, )" + coordinate + R"(, nextLod());
                gl_FragColor = vec4(float(calls) * 0.25, float(lodCalls) * 0.25, sampled.b, 1.0);
            }
        )");
        ASSERT_EQ(pixels.size(), 16u);
        for (size_t i = 0; i < pixels.size(); i += 4)
        {
            EXPECT_NEAR(pixels[i], 64, 1);
            EXPECT_NEAR(pixels[i + 1], 64, 1);
            EXPECT_EQ(pixels[i + 2], 192);
            EXPECT_EQ(pixels[i + 3], 255);
        }
    }
#endif
}

TEST(ShaderCompilation, RawVolumeExplicitGradientsPreserveRows)
{
#if defined(SKIP_EXTERNAL_TEXTURE_TESTS) || defined(SKIP_RENDER_TESTS) || !defined(HAS_NATIVE_IMAGE_LOADING)
    GTEST_SKIP();
#else
    const auto pixels = RenderFullScreenQuad(2, 2,
        "attribute vec3 position; void main() { gl_Position = vec4(position, 1.0); }",
        R"(
            precision highp float;
            uniform highp sampler3D volume;
            void main()
            {
                gl_FragColor = textureGrad(volume, vec3(0.25, 0.25, 0.75),
                    vec3(0.01, 0.03, 0.0), vec3(0.02, 0.04, 0.0));
            }
        )", false, R"(
            var raw = BABYLON.RawTexture.CreateRGBATexture(new Uint8Array([0, 0, 0, 255]),
                1, 1, scene, false, false, BABYLON.Texture.NEAREST_SAMPLINGMODE);
            var data = new Uint8Array([
                64, 32, 16, 255, 128, 32, 16, 255, 64, 224, 16, 255, 128, 224, 16, 255,
                64, 32, 192, 255, 128, 32, 192, 255, 64, 224, 192, 255, 128, 224, 192, 255]);
            engine._engine.loadRawTexture3D(raw.getInternalTexture()._hardwareTexture.underlyingResource,
                data, 2, 2, 2, testVolumeFormat, false, false);
            if (data[1] !== 32 || data[9] !== 224) throw new Error("Volume upload mutated source rows");
            raw.getInternalTexture().is3D = true;
            material.setTexture("volume", raw);
        )", std::chrono::seconds{30}, [](Napi::Env env) {
            env.Global().Set("testVolumeFormat", Napi::Number::New(env, bgfx::TextureFormat::RGBA8));
        });
    ASSERT_EQ(pixels.size(), 16u);
    for (size_t i = 0; i < pixels.size(); i += 4)
    {
        EXPECT_EQ(pixels[i], 64);
        EXPECT_EQ(pixels[i + 1], 32);
        EXPECT_EQ(pixels[i + 2], 192);
        EXPECT_EQ(pixels[i + 3], 255);
    }
#endif
}

TEST(NativeEngineReadback, FloatTexturesUsePinnedJavaScriptByteContract)
{
#if defined(SKIP_EXTERNAL_TEXTURE_TESTS) || defined(SKIP_RENDER_TESTS)
    GTEST_SKIP();
#else
    const auto pixels = RenderFullScreenQuad(1, 1,
        "attribute vec3 position; void main() { gl_Position = vec4(position, 1.0); }",
        "precision highp float; void main() { gl_FragColor = vec4(1.0); }", false, R"(
            globalThis.__prepare = function () {
                var texture = BABYLON.RawTexture.CreateRGBATexture(
                    new Float32Array([0.25, 0.5, 0.75, 1.0]), 1, 1, scene, false, false,
                    BABYLON.Texture.NEAREST_SAMPLINGMODE, BABYLON.Constants.TEXTURETYPE_FLOAT);
                var handle = texture.getInternalTexture()._hardwareTexture.underlyingResource;
                return engine._engine.readTexture(handle, 0, 0, 0, 1, 1, null, 0, 0, -1).then(function (buffer) {
                    if (buffer.byteLength !== 4) throw new Error("Legacy readTexture must return four RGBA8 bytes");
                    var bytes = new Uint8Array(buffer);
                    [64, 128, 191, 255].forEach(function (value, index) {
                        if (Math.abs(bytes[index] - value) > 1) throw new Error("Incorrect legacy readback");
                    });
                    var destination = new Uint8Array(12);
                    destination.fill(91);
                    return engine._engine.readTexture(handle, 0, 0, 0, 1, 1, destination.buffer, 4, 4, -1).then(function () {
                        for (var i = 0; i < destination.length; ++i) {
                            if (destination[i] !== (i >= 4 && i < 8 ? bytes[i - 4] : 91)) {
                                throw new Error("Legacy readback changed values outside its view");
                            }
                        }
                    });
                });
            };
        )");
    EXPECT_EQ(pixels, (std::vector<uint8_t>{255, 255, 255, 255}));
#endif
}

TEST(NativeEngineViewport, MatchesClipSpaceRegion)
{
#if defined(SKIP_EXTERNAL_TEXTURE_TESTS) || defined(SKIP_RENDER_TESTS)
    GTEST_SKIP();
#else
    const std::string fragmentShader =
        "precision highp float;\n"
        "void main(void) { gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0); }\n";
    const auto expected = RenderFullScreenQuad(8, 8,
        "attribute vec3 position;\n"
        "void main(void) { gl_Position = vec4(position.x * 0.5, position.y * 0.5 - 0.25, 0.0, 1.0); }\n",
        fragmentShader, false);
    const auto actual = RenderFullScreenQuad(8, 8,
        "attribute vec3 position;\n"
        "void main(void) { gl_Position = vec4(position, 1.0); }\n",
        fragmentShader, false, "camera.viewport = new BABYLON.Viewport(0.25, 0.125, 0.5, 0.5);");
    EXPECT_EQ(actual, expected);
#endif
}

TEST(NativeEngineClear, PreservesTextureBindings)
{
#if defined(SKIP_EXTERNAL_TEXTURE_TESTS) || defined(SKIP_RENDER_TESTS)
    GTEST_SKIP();
#else
    const std::string vertexShader =
        "precision highp float;\n"
        "attribute vec3 position;\n"
        "attribute vec2 uv;\n"
        "varying vec2 vUV;\n"
        "void main(void) { vUV = uv; gl_Position = vec4(position, 1.0); }\n";
    const std::string fragmentShader =
        "precision highp float;\n"
        "uniform sampler2D inputSampler;\n"
        "uniform sampler2D otherSampler;\n"
        "varying vec2 vUV;\n"
        "void main(void) { gl_FragColor = mix(texture2D(inputSampler, vUV), texture2D(otherSampler, vUV), 0.5); }\n";
    for (int clearMode = 0; clearMode < 4; ++clearMode)
    {
        SCOPED_TRACE(::testing::Message() << "clearMode=" << clearMode);
        const std::string setupScript = R"(
            var first = BABYLON.RawTexture.CreateRGBATexture(new Uint8Array([32, 64, 96, 255]), 1, 1, scene, false, false, 1);
            var second = BABYLON.RawTexture.CreateRGBATexture(new Uint8Array([128, 160, 192, 255]), 1, 1, scene, false, false, 1);
            material.setTexture("inputSampler", first);
            material.setTexture("otherSampler", second);
            material.onBindObservable.add(function () {
                var clearMode = )" + std::to_string(clearMode) + R"(;
                if (clearMode === 3) engine.enableScissor(0, 0, 1, height);
                engine.clear(new BABYLON.Color4(0, 0, 0, 1), (clearMode & 1) !== 0, (clearMode & 2) !== 0, false);
                if (clearMode === 3) engine.disableScissor();
            });
        )";
        const auto pixels = RenderFullScreenQuad(2, 2, vertexShader, fragmentShader, false, setupScript);
        ASSERT_EQ(pixels.size(), 16u);
        for (size_t offset = 0; offset < pixels.size(); offset += 4)
        {
            EXPECT_NEAR(pixels[offset], 80, 1);
            EXPECT_NEAR(pixels[offset + 1], 112, 1);
            EXPECT_NEAR(pixels[offset + 2], 144, 1);
            EXPECT_EQ(pixels[offset + 3], 255);
        }
    }
#endif
}

TEST(NativeEngineClear, ProceduralTextureRetainsBothInputs)
{
#if defined(SKIP_EXTERNAL_TEXTURE_TESTS) || defined(SKIP_RENDER_TESTS)
    GTEST_SKIP();
#else
    const std::string vertexShader =
        "precision highp float;\n"
        "attribute vec3 position;\n"
        "attribute vec2 uv;\n"
        "varying vec2 vUV;\n"
        "void main(void) { vUV = uv; gl_Position = vec4(position, 1.0); }\n";
    const std::string fragmentShader =
        "precision highp float;\n"
        "uniform sampler2D inputSampler;\n"
        "varying vec2 vUV;\n"
        "void main(void) { gl_FragColor = texture2D(inputSampler, vUV); }\n";
    const std::string setupScript = R"(
        var first = BABYLON.RawTexture.CreateRGBATexture(new Uint8Array([32, 64, 96, 255]), 1, 1, scene, false, false, 1);
        var second = BABYLON.RawTexture.CreateRGBATexture(new Uint8Array([128, 160, 192, 255]), 1, 1, scene, false, false, 1);
        var procedural = new BABYLON.ProceduralTexture("sampled", 2, {
            fragmentSource: "precision highp float; varying vec2 vUV; uniform sampler2D first; uniform sampler2D second;" +
                "void main(void) { gl_FragColor = mix(texture2D(first, vUV), texture2D(second, vUV), 0.5); }"
        }, scene, null, false);
        procedural.setTexture("first", first);
        procedural.setTexture("second", second);
        material.setTexture("inputSampler", procedural);
    )";
    const auto pixels = RenderFullScreenQuad(2, 2, vertexShader, fragmentShader, false, setupScript);
    ASSERT_EQ(pixels.size(), 16u);
    for (size_t offset = 0; offset < pixels.size(); offset += 4)
    {
        EXPECT_NEAR(pixels[offset], 80, 1);
        EXPECT_NEAR(pixels[offset + 1], 112, 1);
        EXPECT_NEAR(pixels[offset + 2], 144, 1);
        EXPECT_EQ(pixels[offset + 3], 255);
    }
#endif
}



TEST(NativeEngineInstanceData, QueuedDrawRetainsDataBeforeUpdate)
{
#if defined(SKIP_EXTERNAL_TEXTURE_TESTS) || defined(SKIP_RENDER_TESTS)
    GTEST_SKIP();
#else
    const std::string vertexShader =
        "precision highp float;\n"
        "attribute vec3 position;\n"
        "#include<instancesDeclaration>\n"
        "varying float vValue;\n"
        "void main(void) {\n"
        "#include<instancesVertex>\n"
        "vValue = finalWorld[3].x; gl_Position = vec4(position, 1.0); }\n";
    const std::string fragmentShader =
        "precision highp float;\n"
        "varying float vValue;\n"
        "void main(void) { gl_FragColor = vec4(vValue, 0.0, 0.0, 1.0); }\n";
    const std::string setupScript = R"(
        material.options.uniforms.push("world");
        var matrices = new Float32Array(BABYLON.Matrix.Translation(0.25, 0, 0).m);
        quad.thinInstanceSetBuffer("matrix", matrices, 16, false);
        globalThis.__prepare = function () {
            return material.forceCompilationAsync(quad, { useInstances: true });
        };
        quad.onAfterRenderObservable.add(function () {
            matrices[12] = 0.75;
            quad.thinInstanceBufferUpdated("matrix");
        });
    )";
    const auto pixels = RenderFullScreenQuad(2, 1, vertexShader, fragmentShader, false, setupScript);
    ASSERT_EQ(pixels.size(), 8u);
    for (size_t offset = 0; offset < pixels.size(); offset += 4)
    {
        EXPECT_NEAR(pixels[offset], 64, 1);
        EXPECT_EQ(pixels[offset + 1], 0);
        EXPECT_EQ(pixels[offset + 2], 0);
        EXPECT_EQ(pixels[offset + 3], 255);
    }
#endif
}

TEST(NativeEngineInstanceData, DynamicVertexBufferUpdateWithEmptyStreamDoesNotWaitForFrame)
{
    Babylon::Graphics::Device device{g_deviceConfig};
    bool frameOpen{};
    const auto finishFrame = gsl::finally([&device, &frameOpen] {
        if (frameOpen)
        {
            device.FinishRenderingCurrentFrame();
        }
    });
    device.StartRenderingCurrentFrame();
    frameOpen = true;

    Babylon::AppRuntime runtime{};
    const auto reopenFrame = gsl::finally([&device, &frameOpen] {
        if (!frameOpen)
        {
            device.StartRenderingCurrentFrame();
            frameOpen = true;
        }
    });
    runtime.Dispatch([&device](Napi::Env env) {
        env.Global().Set("globalThis", env.Global());
        device.AddToJavaScript(env);
        Babylon::Polyfills::Console::Initialize(env, [](const char* message, auto) {
            std::cout << message << std::endl;
        });
        Babylon::Polyfills::Window::Initialize(env);
        Babylon::Plugins::NativeEngine::Initialize(env);
    });

    Babylon::ScriptLoader loader{runtime};
    loader.LoadScript("app:///Assets/babylon.max.js");

    auto setupDone = std::make_shared<std::promise<void>>();
    auto setupFuture = setupDone->get_future();
    loader.Dispatch([setupDone](Napi::Env env) {
        try
        {
            auto nativeEngine = env.Global().Get("BABYLON").As<Napi::Object>().Get("NativeEngine").As<Napi::Function>();
            env.Global().Set("__engine", nativeEngine.New({}));
            auto engine = env.Global().Get("__engine").As<Napi::Object>();
            auto data = Napi::Float32Array::New(env, 3);
            engine.Set("__buffer", engine.Get("createDynamicVertexBuffer").As<Napi::Function>().Call(engine, {data}));
            setupDone->set_value();
        }
        catch (...)
        {
            setupDone->set_exception(CaptureTestException());
        }
    });

    if (setupFuture.wait_for(std::chrono::seconds{30}) != std::future_status::ready)
    {
        FAIL() << "dynamic vertex-buffer setup dispatch timed out";
    }
    try
    {
        setupFuture.get();
    }
    catch (const std::exception& exception)
    {
        FAIL() << "dynamic vertex-buffer setup failed: " << exception.what();
    }
    catch (...)
    {
        FAIL() << "dynamic vertex-buffer setup failed with a non-standard exception";
    }

    device.FinishRenderingCurrentFrame();
    frameOpen = false;

    auto updateStarted = std::make_shared<std::promise<void>>();
    auto updateDone = std::make_shared<std::promise<void>>();
    auto updateStartedFuture = updateStarted->get_future();
    auto updateFuture = updateDone->get_future();
    loader.Dispatch([updateStarted, updateDone](Napi::Env env) {
        updateStarted->set_value();
        try
        {
            auto engine = env.Global().Get("__engine").As<Napi::Object>();
            auto data = Napi::Float32Array::New(env, 3);
            engine.Get("updateDynamicVertexBuffer").As<Napi::Function>().Call(engine, {engine.Get("__buffer"), data});
            updateDone->set_value();
        }
        catch (...)
        {
            updateDone->set_exception(CaptureTestException());
        }
    });

    const auto updateStartedStatus = updateStartedFuture.wait_for(std::chrono::seconds{30});
    const auto updateStatus = updateStartedStatus == std::future_status::ready
                                  ? updateFuture.wait_for(std::chrono::milliseconds{250})
                                  : std::future_status::timeout;

    // Keep the gate open through runtime teardown, including when the callback starts late.
    device.StartRenderingCurrentFrame();
    frameOpen = true;
    const auto updateCompletionStatus = updateFuture.wait_for(std::chrono::seconds{30});

    ASSERT_EQ(updateCompletionStatus, std::future_status::ready)
        << "dynamic vertex-buffer update did not complete after the next frame started";
    ASSERT_NO_THROW(updateFuture.get());
    EXPECT_EQ(updateStartedStatus, std::future_status::ready)
        << "dynamic vertex-buffer update dispatch timed out";
    EXPECT_EQ(updateStatus, std::future_status::ready)
        << "an update with no queued commands must not wait for the next frame";
}

// Compare channels to test shader coordinates independently of backend readback row order.
TEST(ShaderCompilation, FragCoordYMatchesInterpolatedUV)
{
#if defined(SKIP_EXTERNAL_TEXTURE_TESTS) || defined(SKIP_RENDER_TESTS)
    GTEST_SKIP();
#else
    constexpr uint32_t WIDTH = 8;
    constexpr uint32_t HEIGHT = 64;

    const std::string vertexShader =
        "precision highp float;\n"
        "attribute vec3 position;\n"
        "attribute vec2 uv;\n"
        "varying vec2 vUV;\n"
        "void main(void) { vUV = uv; gl_Position = vec4(position, 1.0); }\n";

    const std::string fragmentShader =
        "precision highp float;\n"
        "uniform vec2 targetSize;\n"
        "varying vec2 vUV;\n"
        "void main(void) {\n"
        "    gl_FragColor = vec4(gl_FragCoord.y / targetSize.y, vUV.y, 0.0, 1.0);\n"
        "}\n";

    auto pixels = RenderFullScreenQuad(WIDTH, HEIGHT, vertexShader, fragmentShader, false);
    ASSERT_EQ(pixels.size(), static_cast<size_t>(WIDTH) * HEIGHT * 4);

    const auto texel = [&pixels](uint32_t row) {
        const size_t offset = static_cast<size_t>(row) * WIDTH * 4;
        return std::make_pair(static_cast<int>(pixels[offset]), static_cast<int>(pixels[offset + 1]));
    };

    const auto first = texel(0);
    const auto middle = texel(HEIGHT / 2);
    const auto last = texel(HEIGHT - 1);
    std::cout << "row 0 fragCoord=" << first.first << " uv=" << first.second
              << ", row " << (HEIGHT / 2) << " fragCoord=" << middle.first << " uv=" << middle.second
              << ", row " << (HEIGHT - 1) << " fragCoord=" << last.first << " uv=" << last.second
              << std::endl;

    // Reject a constant ramp that would pass the comparison vacuously.
    ASSERT_GT(std::abs(first.second - last.second), 200)
        << "vUV.y reference ramp did not vary across the target";

    // Allow interpolation/8-bit quantization, but not a vertical flip.
    for (uint32_t row = 0; row < HEIGHT; ++row)
    {
        const auto values = texel(row);
        ASSERT_LE(std::abs(values.first - values.second), 6)
            << "gl_FragCoord.y disagrees with the interpolated vUV.y at row " << row
            << " (gl_FragCoord=" << values.first << ", vUV=" << values.second << ")";
    }
#endif
}

// Direct returns parent the symbol on TIntermBranch rather than an expression node.
TEST(ShaderCompilation, FragCoordDirectReturnMatchesInterpolatedUV)
{
#if defined(SKIP_EXTERNAL_TEXTURE_TESTS) || defined(SKIP_RENDER_TESTS)
    GTEST_SKIP();
#else
    constexpr uint32_t WIDTH = 8;
    constexpr uint32_t HEIGHT = 64;

    const std::string vertexShader =
        "precision highp float;\n"
        "attribute vec3 position;\n"
        "attribute vec2 uv;\n"
        "varying vec2 vUV;\n"
        "void main(void) { vUV = uv; gl_Position = vec4(position, 1.0); }\n";

    const std::string fragmentShader =
        "precision highp float;\n"
        "uniform vec2 targetSize;\n"
        "varying vec2 vUV;\n"
        "vec4 fragCoord() { return gl_FragCoord; }\n"
        "void main(void) {\n"
        "    gl_FragColor = vec4(fragCoord().y / targetSize.y, vUV.y, 0.0, 1.0);\n"
        "}\n";

    auto pixels = RenderFullScreenQuad(WIDTH, HEIGHT, vertexShader, fragmentShader, false);
    ASSERT_EQ(pixels.size(), static_cast<size_t>(WIDTH) * HEIGHT * 4);

    const auto texel = [&pixels](uint32_t row) {
        const size_t offset = static_cast<size_t>(row) * WIDTH * 4;
        return std::make_pair(static_cast<int>(pixels[offset]), static_cast<int>(pixels[offset + 1]));
    };

    const auto first = texel(0);
    const auto last = texel(HEIGHT - 1);
    ASSERT_GT(std::abs(first.second - last.second), 200)
        << "vUV.y reference ramp did not vary across the target";

    for (uint32_t row = 0; row < HEIGHT; ++row)
    {
        const auto values = texel(row);
        ASSERT_LE(std::abs(values.first - values.second), 6)
            << "gl_FragCoord.y disagrees with the interpolated vUV.y at row " << row
            << " after a direct return (gl_FragCoord=" << values.first << ", vUV=" << values.second << ")";
    }
#endif
}

// Compare fragment-coordinate and UV addressing without depending on upload row order.
TEST(ShaderCompilation, FragCoordAndUVAddressATextureIdentically)
{
#if defined(SKIP_EXTERNAL_TEXTURE_TESTS) || defined(SKIP_RENDER_TESTS)
    GTEST_SKIP();
#else
    constexpr uint32_t WIDTH = 8;
    constexpr uint32_t HEIGHT = 64;

    const std::string vertexShader =
        "precision highp float;\n"
        "attribute vec3 position;\n"
        "attribute vec2 uv;\n"
        "varying vec2 vUV;\n"
        "void main(void) {\n"
        "    vUV = uv;\n"
        "    gl_Position = vec4(position, 1.0);\n"
        "}\n";

    const std::string uvShader =
        "precision highp float;\n"
        "varying vec2 vUV;\n"
        "uniform vec2 targetSize;\n"
        "uniform sampler2D inputSampler;\n"
        "void main(void) {\n"
        "    gl_FragColor = texture2D(inputSampler, vUV);\n"
        "}\n";

    const std::string fragCoordShader =
        "precision highp float;\n"
        "varying vec2 vUV;\n"
        "uniform vec2 targetSize;\n"
        "uniform sampler2D inputSampler;\n"
        "void main(void) {\n"
        "    gl_FragColor = texture2D(inputSampler, gl_FragCoord.xy / targetSize);\n"
        "}\n";

    auto uvPixels = RenderFullScreenQuad(WIDTH, HEIGHT, vertexShader, uvShader, true);
    auto fragCoordPixels = RenderFullScreenQuad(WIDTH, HEIGHT, vertexShader, fragCoordShader, true);

    ASSERT_EQ(uvPixels.size(), static_cast<size_t>(WIDTH) * HEIGHT * 4);
    ASSERT_EQ(fragCoordPixels.size(), uvPixels.size());

    const auto red = [](const std::vector<uint8_t>& pixels, uint32_t row) {
        return static_cast<int>(pixels[static_cast<size_t>(row) * WIDTH * 4]);
    };

    // A constant source cannot detect a vertical mirror.
    ASSERT_GT(std::abs(red(uvPixels, 0) - red(uvPixels, HEIGHT - 1)), 200)
        << "the source texture must vary from top to bottom for this test to mean anything";

    std::cout << "uv       rows: " << red(uvPixels, 0) << " .. " << red(uvPixels, HEIGHT - 1) << std::endl;
    std::cout << "fragCoord rows: " << red(fragCoordPixels, 0) << " .. " << red(fragCoordPixels, HEIGHT - 1) << std::endl;

    for (uint32_t row = 0; row < HEIGHT; ++row)
    {
        ASSERT_EQ(red(fragCoordPixels, row), red(uvPixels, row))
            << "row " << row << " differs between gl_FragCoord and uv addressing";
    }
#endif
}

TEST(ShaderCompilation, InterfaceBlocksLinkDifferentInstanceNames)
{
#if defined(SKIP_EXTERNAL_TEXTURE_TESTS) || defined(SKIP_RENDER_TESTS) || !defined(HAS_SHADER_INTERFACE_BLOCKS)
    GTEST_SKIP();
#else
    const auto pixels = RenderFullScreenQuad(1, 1, R"(
        #extension GL_EXT_shader_io_blocks : require
        attribute vec3 position;
        out Payload { vec3 tint; } vertexData;
        void main() {
            gl_Position = vec4(position, 1.0);
            vertexData.tint = vec3(0.25, 0.5, 0.75);
        }
    )", R"(
        #extension GL_EXT_shader_io_blocks : require
        precision highp float;
        in Payload { vec3 tint; } fragmentData;
        void main() { gl_FragColor = vec4(fragmentData.tint, 1.0); }
    )", false);
    ASSERT_EQ(pixels.size(), 4u);
    EXPECT_NEAR(pixels[0], 64, 1);
    EXPECT_NEAR(pixels[1], 128, 1);
    EXPECT_NEAR(pixels[2], 191, 1);
    EXPECT_EQ(pixels[3], 255);
#endif
}

TEST(ShaderCompilation, InterfaceBlocksReserveAllMemberLocations)
{
#if defined(SKIP_EXTERNAL_TEXTURE_TESTS) || defined(SKIP_RENDER_TESTS) || !defined(HAS_SHADER_INTERFACE_BLOCKS)
    GTEST_SKIP();
#else
    const auto pixels = RenderFullScreenQuad(1, 1, R"(
        #extension GL_EXT_shader_io_blocks : require
        attribute vec3 position;
        out Payload { mat2 transform; vec3 tint; } aData;
        varying vec3 zExtra;
        void main() {
            gl_Position = vec4(position, 1.0);
            aData.transform = mat2(0.5);
            aData.tint = vec3(0.0, 0.0, 0.75);
            zExtra = vec3(0.5, 1.0, 0.0);
        }
    )", R"(
        #extension GL_EXT_shader_io_blocks : require
        precision highp float;
        in Payload { mat2 transform; vec3 tint; } aData;
        varying vec3 zExtra;
        void main() { gl_FragColor = vec4(aData.transform * zExtra.xy, aData.tint.z + zExtra.z, 1.0); }
    )", false);
    ASSERT_EQ(pixels.size(), 4u);
    EXPECT_NEAR(pixels[0], 64, 1);
    EXPECT_NEAR(pixels[1], 128, 1);
    EXPECT_NEAR(pixels[2], 191, 1);
    EXPECT_EQ(pixels[3], 255);
#endif
}

TEST(NativeEngineShadows, PointLightCubeOrientationMatchesAllShadowLookups)
{
#if defined(SKIP_EXTERNAL_TEXTURE_TESTS) || defined(SKIP_RENDER_TESTS)
    GTEST_SKIP();
#else
    constexpr uint32_t CELL_SIZE = 8;
    constexpr uint32_t FACE_COUNT = 6;
    constexpr uint32_t WIDTH = FACE_COUNT * 2 * CELL_SIZE;
    constexpr uint32_t HEIGHT = CELL_SIZE;

    const std::string vertexShader =
        "precision highp float;\n"
        "attribute vec3 position;\n"
        "attribute vec2 uv;\n"
        "varying vec2 vUV;\n"
        "void main(void) { vUV = uv; gl_Position = vec4(position, 1.0); }\n";

    struct ShadowCase
    {
        int Filter;
        const char* Name;
        bool BackFaceCulling;
        bool CullBackFaces;
        bool ForceBackFacesOnly;
        const char* Lookup;
    };
    const ShadowCase cases[] = {
        {0, "none/back-cull", true, true, false, "computeShadowCube(worldPos, vec3(0.0), shadowSampler, 0.0, vec2(0.1, 10.1))"},
        {0, "none/front-cull", true, false, false, "computeShadowCube(worldPos, vec3(0.0), shadowSampler, 0.0, vec2(0.1, 10.1))"},
        {0, "none/force-back-faces", true, true, true, "computeShadowCube(worldPos, vec3(0.0), shadowSampler, 0.0, vec2(0.1, 10.1))"},
        {1, "esm", true, true, false, "computeShadowWithESMCube(worldPos, vec3(0.0), shadowSampler, 0.0, 20.0, vec2(0.1, 10.1))"},
        {2, "poisson", true, true, false, "computeShadowWithPoissonSamplingCube(worldPos, vec3(0.0), shadowSampler, 1.0 / 64.0, 0.0, vec2(0.1, 10.1))"},
        {4, "close-esm", true, true, false, "computeShadowWithCloseESMCube(worldPos, vec3(0.0), shadowSampler, 0.0, 20.0, vec2(0.1, 10.1))"},
    };

    for (const auto& shadowCase : cases)
    {
        SCOPED_TRACE(shadowCase.Name);

        // Each pair addresses one cube face in PointLight.getShadowDirection order:
        // +X, -X, -Y, +Y, +Z, -Z. The first ray goes through an off-axis
        // caster; the second reflects that ray across the face's projection-Y
        // axis and must stay lit. The +/-Y faces use Z as their vertical axis.
        const std::string fragmentShader =
            "precision highp float;\n"
            "varying vec2 vUV;\n"
            "uniform samplerCube shadowSampler;\n"
            "#define SHADOWS\n"
            "#include<shadowsFragmentFunctions>\n"
            "void main(void) {\n"
            "    float cell = floor(min(vUV.x, 0.999999) * 12.0);\n"
            "    vec3 direction;\n"
            "    if (cell < 0.5) direction = vec3( 1.00,  0.31,  0.17);\n"
            "    else if (cell < 1.5) direction = vec3( 1.00, -0.31,  0.17);\n"
            "    else if (cell < 2.5) direction = vec3(-1.00, -0.27,  0.19);\n"
            "    else if (cell < 3.5) direction = vec3(-1.00,  0.27,  0.19);\n"
            "    else if (cell < 4.5) direction = vec3( 0.23, -1.00,  0.37);\n"
            "    else if (cell < 5.5) direction = vec3( 0.23, -1.00, -0.37);\n"
            "    else if (cell < 6.5) direction = vec3(-0.21,  1.00,  0.33);\n"
            "    else if (cell < 7.5) direction = vec3(-0.21,  1.00, -0.33);\n"
            "    else if (cell < 8.5) direction = vec3( 0.29,  0.35,  1.00);\n"
            "    else if (cell < 9.5) direction = vec3( 0.29, -0.35,  1.00);\n"
            "    else if (cell < 10.5) direction = vec3(-0.25, -0.33, -1.00);\n"
            "    else direction = vec3(-0.25,  0.33, -1.00);\n"
            "    vec3 worldPos = normalize(direction) * 6.0;\n"
            "    float visibility = " +
            std::string{shadowCase.Lookup} +
            ";\n"
            "    gl_FragColor = vec4(vec3(visibility), 1.0);\n"
            "}\n";

        const std::string setupScript =
            R"(
                var filter = )" +
            std::to_string(shadowCase.Filter) +
            R"(;
                var backFaceCulling = )" +
            std::string{shadowCase.BackFaceCulling ? "true" : "false"} +
            R"(;
                var cullBackFaces = )" +
            std::string{shadowCase.CullBackFaces ? "true" : "false"} +
            R"(;
                var forceBackFacesOnly = )" +
            std::string{shadowCase.ForceBackFacesOnly ? "true" : "false"} +
            R"(;

                camera.layerMask = 0x1;
                quad.layerMask = 0x1;
                quad.renderingGroupId = 1;

                var light = new BABYLON.PointLight("shadowLight", BABYLON.Vector3.Zero(), scene);
                light.shadowMinZ = 0.1;
                light.shadowMaxZ = 10.0;

                // Force the portable packed-RGBA shadow path so the lookup
                // shader has one format on every Native backend.
                var caps = engine.getCaps();
                var capNames = [
                    "textureHalfFloatRender",
                    "textureHalfFloatLinearFiltering",
                    "textureFloatRender",
                    "textureFloatLinearFiltering"
                ];
                var savedCaps = capNames.map(function (name) { return caps[name]; });
                var shadowGenerator;
                try {
                    capNames.forEach(function (name) { caps[name] = false; });
                    shadowGenerator = new BABYLON.ShadowGenerator(64, light);
                } finally {
                    capNames.forEach(function (name, index) { caps[name] = savedCaps[index]; });
                }
                shadowGenerator.filter = filter;
                shadowGenerator.bias = 0.0;
                shadowGenerator.depthScale = 20.0;
                shadowGenerator.forceBackFacesOnly = forceBackFacesOnly;

                var casterMaterial = new BABYLON.StandardMaterial("casterMaterial", scene);
                casterMaterial.disableLighting = true;
                casterMaterial.backFaceCulling = backFaceCulling;
                casterMaterial.cullBackFaces = cullBackFaces;

                var casterDirections = [
                    new BABYLON.Vector3( 1.00,  0.31,  0.17),
                    new BABYLON.Vector3(-1.00, -0.27,  0.19),
                    new BABYLON.Vector3( 0.23, -1.00,  0.37),
                    new BABYLON.Vector3(-0.21,  1.00,  0.33),
                    new BABYLON.Vector3( 0.29,  0.35,  1.00),
                    new BABYLON.Vector3(-0.25, -0.33, -1.00)
                ];
                casterDirections.forEach(function (direction, face) {
                    direction.normalize();
                    var caster = BABYLON.MeshBuilder.CreateBox(
                        "caster" + face,
                        { width: 0.8, height: 0.8, depth: 0.8 },
                        scene);
                    caster.position.copyFrom(direction.scale(3.0));
                    caster.material = casterMaterial;
                    caster.layerMask = 0x2;
                    shadowGenerator.addShadowCaster(caster, false);
                });

                material.options.samplers.push("shadowSampler");
                material.setTexture("shadowSampler", shadowGenerator.getShadowMapForRendering());
                globalThis.__prepare = function () {
                    return Promise.all([
                        shadowGenerator.forceCompilationAsync(),
                        material.forceCompilationAsync(quad)
                    ]);
                };
            )";

        const auto pixels = RenderFullScreenQuad(WIDTH, HEIGHT, vertexShader, fragmentShader, false, setupScript);
        ASSERT_EQ(pixels.size(), static_cast<size_t>(WIDTH) * HEIGHT * 4);

        const auto redAtCell = [&pixels](uint32_t cell) {
            const uint32_t x = cell * CELL_SIZE + CELL_SIZE / 2;
            const uint32_t y = HEIGHT / 2;
            return static_cast<int>(pixels[(static_cast<size_t>(y) * WIDTH + x) * 4]);
        };
        const char* faceNames[] = {"+X", "-X", "-Y", "+Y", "+Z", "-Z"};
        for (uint32_t face = 0; face < FACE_COUNT; ++face)
        {
            const int shadowed = redAtCell(face * 2);
            const int lit = redAtCell(face * 2 + 1);
            EXPECT_LT(shadowed, 64)
                << faceNames[face] << " off-axis caster ray was not shadowed (visibility=" << shadowed << ")";
            EXPECT_GT(lit, 192)
                << faceNames[face] << " projection-Y mirror ray was not lit (visibility=" << lit << ")";
        }
    }
#endif
}

TEST(NativeEngineTextureSampling, RawUploadsPreserveEarlierCameraDraws)
{
#if defined(SKIP_EXTERNAL_TEXTURE_TESTS) || defined(SKIP_RENDER_TESTS)
    GTEST_SKIP();
#else
    const std::string vertexShader =
        "precision highp float;\n"
        "attribute vec3 position;\n"
        "void main(void) { gl_Position = vec4(position, 1.0); }\n";
    const std::string fragmentShader =
        "precision highp float;\n"
        "uniform sampler2D inputSampler;\n"
        "void main(void) { gl_FragColor = textureLod(inputSampler, vec2(0.5), 1.0); }\n";
    for (const bool mipmaps : {false, true})
    {
        SCOPED_TRACE(mipmaps);
        const std::string setupScript = std::string{R"(
            var source = BABYLON.RawTexture.CreateRGBATexture(
                new Uint8Array(16), 2, 2, scene, )"} + (mipmaps ? "true" : "false") + R"(,
                false, BABYLON.Texture.TRILINEAR_SAMPLINGMODE);
            material.options.samplers.push("inputSampler");
            material.setTexture("inputSampler", source);
            scene.autoClear = false;
            scene.autoClearDepthAndStencil = false;
            camera.viewport = new BABYLON.Viewport(0, 0, 0.5, 1);
            var second = camera.clone("second");
            second.viewport = new BABYLON.Viewport(0.5, 0, 0.5, 1);
            second.outputRenderTarget = outputTexture;
            scene.activeCameras = [camera, second];
            scene.onBeforeCameraRenderObservable.add(function (active) {
                var color = active === camera ? [255, 0, 0, 255] : [0, 255, 0, 255];
                source.update(new Uint8Array(color.concat(color, color, color)));
            });
        )";
        const auto pixels = RenderFullScreenQuad(8, 4, vertexShader, fragmentShader, false, setupScript);
        ASSERT_EQ(pixels.size(), 8u * 4u * 4u);
        for (uint32_t y = 0; y < 4; ++y)
        {
            for (uint32_t x = 0; x < 8; ++x)
            {
                const auto offset = (y * 8 + x) * 4;
                EXPECT_EQ(pixels[offset], x < 4 ? 255 : 0);
                EXPECT_EQ(pixels[offset + 1], x < 4 ? 0 : 255);
                EXPECT_EQ(pixels[offset + 2], 0);
                EXPECT_EQ(pixels[offset + 3], 255);
            }
        }
    }
#endif
}

TEST(NativeEngineTextureSampling, NoMipSamplingPreservesFiltersAndModeChanges)
{
#if defined(SKIP_EXTERNAL_TEXTURE_TESTS) || defined(SKIP_RENDER_TESTS)
    GTEST_SKIP();
#else
    const std::string vertexShader =
        "precision highp float;\n"
        "attribute vec3 position;\n"
        "attribute vec2 uv;\n"
        "varying vec2 vUV;\n"
        "void main(void) { vUV = uv; gl_Position = vec4(position, 1.0); }\n";
    struct SamplingCase
    {
        int Mode;
        int Anisotropy;
        int Red;
        int Green;
        int MagnifiedRed;
    };
    const SamplingCase cases[] = {
        {1, 1, 0, 0, 255},   // Nearest, no mips.
        {2, 1, 128, 0, 191}, // Linear, no mips.
        {2, 4, 255, 0, 255}, // Anisotropic, constant-color base mip.
        {3, 1, 0, 255, 191}, // Linear mip filtering restored.
        {4, 1, 0, 255, 255}, // Point mip filtering restored.
    };
    for (const auto& sample : cases)
    {
        SCOPED_TRACE(::testing::Message() << "mode=" << sample.Mode << ", anisotropy=" << sample.Anisotropy);
        const std::string setupScript = R"(
            var requestedMode = )" + std::to_string(sample.Mode) + R"(;
            var anisotropy = )" + std::to_string(sample.Anisotropy) + R"(;
            // Distinct lower mips separate mip selection from spatial filtering.
            var sampled;
            for (var mip = 0, size = 8; size >= 1; mip++, size /= 2) {
                var pixels = new Uint8Array(size * size * 4);
                var offset = 0;
                for (var y = 0; y < size; y++) {
                    for (var x = 0; x < size; x++) {
                        pixels[offset++] = mip === 0 && (anisotropy > 1 || x % 2 === 0) ? 255 : 0;
                        pixels[offset++] = mip === 0 ? 0 : 255;
                        pixels[offset++] = 0;
                        pixels[offset++] = 255;
                    }
                }
                if (mip === 0) {
                    sampled = BABYLON.RawTexture.CreateRGBATexture(pixels, size, size, scene, true, false, 1);
                } else {
                    engine.updateTextureData(sampled.getInternalTexture(), pixels, 0, 0, size, size, 0, mip);
                }
            }
            sampled.anisotropicFilteringLevel = anisotropy;
            sampled.updateSamplingMode(4);
            sampled.updateSamplingMode(requestedMode);
            material.setTexture("inputSampler", sampled);
        )";
        for (const bool magnify : {false, true})
        {
            SCOPED_TRACE(::testing::Message() << "magnify=" << magnify);
            const std::string fragmentShader =
                "precision highp float;\n"
                "uniform sampler2D inputSampler;\n"
                "varying vec2 vUV;\n"
                "void main(void) { gl_FragColor = texture2D(inputSampler, vUV * " +
                std::string{magnify ? "0.125" : "0.5"} + " + 0.25); }\n";
            const auto pixels = RenderFullScreenQuad(2, 2, vertexShader, fragmentShader, true, setupScript);
            ASSERT_EQ(pixels.size(), 16u);
            for (size_t offset = 0; offset < pixels.size(); offset += 4)
            {
                EXPECT_NEAR(pixels[offset], magnify ? sample.MagnifiedRed : sample.Red, 1);
                EXPECT_NEAR(pixels[offset + 1], magnify ? 0 : sample.Green, 1);
                EXPECT_EQ(pixels[offset + 2], 0);
                EXPECT_EQ(pixels[offset + 3], 255);
            }
        }
    }
#endif
}

TEST(ShaderCompilation, PbrRoughnessSquareInNestedLoops)
{
#if defined(SKIP_EXTERNAL_TEXTURE_TESTS) || defined(SKIP_RENDER_TESTS)
    GTEST_SKIP();
#else
    const std::string vertexShader =
        "precision highp float;\n"
        "attribute vec3 position;\n"
        "void main(void) { gl_Position = vec4(position, 1.0); }\n";

    // Use the production include: FXC can replace roughness squared with
    // roughness when the saturated value is used inside nested dynamic loops.
    const std::string fragmentShader =
        "precision highp float;\n"
        "uniform vec2 targetSize;\n"
        "#include<helperFunctions>\n"
        "#include<pbrHelperFunctions>\n"
        "void main(void) {\n"
        "    vec3 result = vec3(0.0);\n"
        "    for (int i = 0; i < int(targetSize.x); ++i) {\n"
        "        for (int j = 0; j < int(targetSize.y); ++j) {\n"
        "            float roughness = clamp(1.0 / targetSize.x, 0.0, 1.0);\n"
        "            result = vec3(roughness, convertRoughnessToAverageSlope(roughness), sqrt(roughness));\n"
        "        }\n"
        "    }\n"
        "    gl_FragColor = vec4(result, 1.0);\n"
        "}\n";

    auto pixels = RenderFullScreenQuad(2, 1, vertexShader, fragmentShader, false);
    ASSERT_EQ(pixels.size(), 8u);
    for (size_t offset = 0; offset < pixels.size(); offset += 4)
    {
        EXPECT_NEAR(pixels[offset], 128, 1);
        EXPECT_NEAR(pixels[offset + 1], 64, 1) << "roughness 0.5 must produce alphaG 0.2505, not 0.5005";
        EXPECT_NEAR(pixels[offset + 2], 180, 1);
        EXPECT_EQ(pixels[offset + 3], 255);
    }
#endif
}

TEST(ShaderCompilation, SaturatedArithmeticInNestedLoops)
{
#if defined(SKIP_EXTERNAL_TEXTURE_TESTS) || defined(SKIP_RENDER_TESTS)
    GTEST_SKIP();
#else
    const std::string vertexShader =
        "precision highp float;\n"
        "attribute vec3 position;\n"
        "void main(void) { gl_Position = vec4(position, 1.0); }\n";
    const std::string arithmetic =
        "float value = clamp(1.0 / targetSize.x, 0.0, 1.0);\n"
        "float squared = value * value;\n"
        "float fifth = squared * squared * value;\n"
        "float attenuation = clamp(1.0 - value, 0.0, 1.0);\n"
        "attenuation *= attenuation;\n"
        "result = vec3(squared, fifth, attenuation);\n";
    const std::vector<std::pair<std::string, std::string>> loops{
        {"for (int i = 0; i < int(targetSize.x); ++i) { for (int j = 0; j < int(targetSize.y); ++j) {\n", "}}\n"},
        {"int i = 0; while (i++ < int(targetSize.x)) { int j = 0; while (j++ < int(targetSize.y)) {\n", "}}\n"},
        {"int i = 0; do { int j = 0; do {\n", "} while (++j < int(targetSize.y)); } while (++i < int(targetSize.x));\n"},
    };
    for (const auto& loop : loops)
    {
        SCOPED_TRACE(loop.first);
        const std::string fragmentShader =
            "precision highp float;\n"
            "uniform vec2 targetSize;\n"
            "void main(void) {\n"
            "vec3 result = vec3(0.0);\n" +
            loop.first + arithmetic + loop.second +
            "gl_FragColor = vec4(result, 1.0);\n"
            "}\n";
        auto pixels = RenderFullScreenQuad(2, 1, vertexShader, fragmentShader, false);
        ASSERT_EQ(pixels.size(), 8u);
        for (size_t offset = 0; offset < pixels.size(); offset += 4)
        {
            EXPECT_NEAR(pixels[offset], 64, 1);
            EXPECT_NEAR(pixels[offset + 1], 8, 1);
            EXPECT_NEAR(pixels[offset + 2], 64, 1);
            EXPECT_EQ(pixels[offset + 3], 255);
        }
    }
#endif
}
