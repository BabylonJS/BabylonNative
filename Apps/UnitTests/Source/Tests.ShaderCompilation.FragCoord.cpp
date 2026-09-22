#include <gtest/gtest.h>
#include <gsl/util>

#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/Window.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <Babylon/Plugins/ExternalTexture.h>
#include <Babylon/ScriptLoader.h>

#include "Helpers.h"

#include <atomic>
#include <chrono>
#include <cstdlib>
#include <future>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

extern Babylon::Graphics::Configuration g_deviceConfig;

// These tests pin down the orientation of gl_FragCoord.y.
//
// D3D, Metal and Vulkan rasterize with a top-left origin while GL uses bottom-left,
// and Babylon Native does not flip geometry, so gl_FragCoord.y arrives mirrored and
// is corrected by the shader compiler (FragCoordYFlipTraverser).
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
        std::chrono::milliseconds renderTimeout = std::chrono::seconds{30})
    {
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

                    // Clip-space quad passed straight through the vertex shader, so no
                    // projection matrix is involved and it lines up with the target exactly.
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
                        // Decreasing red ramp makes a vertical mirror unambiguous; blue
                        // encodes the low bits of the row index to catch off-by-one errors.
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

        WaitForTestCompletion(renderFuture, renderTimeout, "quad preparation/render timed out");

        device.FinishRenderingCurrentFrame();
        frameOpen = false;

        return Helpers::ReadPixels(device.GetPlatformInfo(), outputTexture, width, height);
    }
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

// gl_FragCoord.y must increase towards +Y in clip space, like the interpolated vUV.y
// the quad supplies. Without the correction the two ramps become mirror images.
//
// Two channels of a single render are compared rather than absolute row indices
// because Helpers::ReadPixels returns the bottom scanline first on OpenGL and the top
// first on D3D11; an absolute check would encode one backend's readback convention
// instead of the shading language rule under test.
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

    // Guard against a vacuous pass: the reference ramp must actually sweep the range.
    ASSERT_GT(std::abs(first.second - last.second), 200)
        << "vUV.y reference ramp did not vary across the target";

    // Both channels come from the same fragment invocation, so they must agree row by
    // row whichever end of the image the readback starts at. The tolerance absorbs
    // interpolation and 8-bit quantization only.
    for (uint32_t row = 0; row < HEIGHT; ++row)
    {
        const auto values = texel(row);
        ASSERT_LE(std::abs(values.first - values.second), 6)
            << "gl_FragCoord.y disagrees with the interpolated vUV.y at row " << row
            << " (gl_FragCoord=" << values.first << ", vUV=" << values.second << ")";
    }
#endif
}

// `return gl_FragCoord;` parents the symbol on TIntermBranch, which MakeReplacements
// must handle; without that the compiler throws "Cannot replace symbol".
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

// Indexing a screen-sized texture with gl_FragCoord must give the same image as
// indexing it with the interpolated UVs of a full-screen quad; this only holds if the
// gl_FragCoord correction and FlipSamplerCoordinatesTraverser compose to a no-op.
// The two addressing modes are compared against each other rather than against the
// source pixels so the test does not depend on createRawTexture's memory layout.
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

    // Guard against a vacuous pass: the source must actually vary down the image.
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
