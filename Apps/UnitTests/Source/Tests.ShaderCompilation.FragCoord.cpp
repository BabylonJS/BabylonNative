#include <gtest/gtest.h>

#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/Window.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <Babylon/Plugins/ExternalTexture.h>
#include <Babylon/ScriptLoader.h>

#include "Helpers.h"

#include <chrono>
#include <cstdlib>
#include <future>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

extern Babylon::Graphics::Configuration g_deviceConfig;

// These tests pin down the orientation of gl_FragCoord.y.
//
// Babylon Native's shader model is "shader-visible coordinates are GL-logical,
// converted to physical at each sampler access". D3D, Metal and Vulkan rasterize
// with a top-left origin while GL uses bottom-left, and Babylon Native does not
// flip geometry, so gl_FragCoord.y arrives mirrored from the hardware and has to
// be corrected by the shader compiler (FragCoordYFlipTraverser).
//
// Both tests render a full-screen quad into a render target and read the result
// back. Helpers::ReadPixels returns rows in memory order, so row 0 is the top of
// the image on every backend.
namespace
{
    // Renders a full-screen quad into a width x height render target using the
    // supplied fragment shader, and returns the RGBA8 pixels in memory order.
    //
    // The fragment shader may declare a `uniform vec2 targetSize` (set to the
    // render target dimensions) and a `uniform sampler2D inputSampler` (bound to
    // a raw texture whose row y is filled with the RGBA value produced by
    // makeRow(y), when withInputTexture is true).
    std::vector<uint8_t> RenderFullScreenQuad(
        uint32_t width,
        uint32_t height,
        const std::string& vertexShader,
        const std::string& fragmentShader,
        bool withInputTexture)
    {
        // Clip-space quad, so no projection matrix is involved and the geometry
        // lines up with the render target exactly. uv follows the GL convention
        // of (0,0) at the bottom-left corner.
        Babylon::Graphics::Device device{g_deviceConfig};
        device.StartRenderingCurrentFrame();

        auto outputTexture = Helpers::CreateTexture(
            device.GetPlatformInfo().Device, width, height, 1, true);
        Babylon::Plugins::ExternalTexture outputExternalTexture{outputTexture};

        Babylon::AppRuntime::Options options{};
        options.UnhandledExceptionHandler = [](const Napi::Error& error) {
            std::cerr << "[Uncaught Error] " << Napi::GetErrorString(error) << std::endl;
            std::cerr.flush();
        };

        Babylon::AppRuntime runtime{options};
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
                    delete engine.getCaps().parallelShaderCompile;
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

                    // Two triangles in clip space covering the whole target. The
                    // vertex shader passes position straight through, so no
                    // projection matrix is involved and the quad lines up exactly
                    // with the render target regardless of camera conventions.
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
                        // Row y is filled with a monotonically decreasing red ramp so
                        // that a vertical mirror is unambiguous. Blue encodes the low
                        // bits of the row index to catch off-by-one errors.
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
                    globalThis.__scene = scene;
                };

                globalThis.render = function () {
                    var scene = globalThis.__scene;
                    return scene.whenReadyAsync().then(function () {
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

        loader.Eval(finalScript, "frag_coord_orientation_test.js");

        std::promise<void> startupDone;
        loader.Dispatch([&outputExternalTexture, &startupDone, width, height](Napi::Env env) {
            auto jsOutput = outputExternalTexture.CreateForJavaScript(env);
            env.Global().Get("startup").As<Napi::Function>().Call({
                jsOutput,
                Napi::Number::New(env, width),
                Napi::Number::New(env, height),
            });
            startupDone.set_value();
        });
        startupDone.get_future().wait();

        device.FinishRenderingCurrentFrame();
        device.StartRenderingCurrentFrame();

        std::promise<void> renderDone;
        loader.Dispatch([&renderDone](Napi::Env env) {
            auto jsPromise = env.Global().Get("render").As<Napi::Function>().Call({}).As<Napi::Promise>();

            auto jsOnFulfilled = Napi::Function::New(env, [&renderDone](const Napi::CallbackInfo&) {
                renderDone.set_value();
            });
            auto jsOnRejected = Napi::Function::New(env, [&renderDone](const Napi::CallbackInfo& info) {
                renderDone.set_exception(std::make_exception_ptr(
                    std::runtime_error{Napi::GetErrorString(info[0].As<Napi::Error>())}));
            });

            jsPromise.Get("then").As<Napi::Function>().Call(jsPromise, {jsOnFulfilled, jsOnRejected});
        });

        auto renderFuture = renderDone.get_future();
        EXPECT_EQ(renderFuture.wait_for(std::chrono::seconds(30)), std::future_status::ready)
            << "render timed out";
        EXPECT_NO_THROW(renderFuture.get()) << "render rejected";

        device.FinishRenderingCurrentFrame();

        auto pixels = Helpers::ReadPixels(device.GetPlatformInfo(), outputTexture, width, height);
        Helpers::DestroyTexture(outputTexture);
        return pixels;
    }
}

// gl_FragCoord.y must follow the GL convention of increasing upwards, so the top
// row of the image (row 0 in memory) has to hold the largest value. Without the
// gl_FragCoord correction the ramp comes out upside down on D3D/Metal/Vulkan.
TEST(ShaderCompilation, FragCoordYIncreasesUpwards)
{
#if defined(SKIP_EXTERNAL_TEXTURE_TESTS) || defined(SKIP_RENDER_TESTS)
    GTEST_SKIP();
#else
    constexpr uint32_t WIDTH = 8;
    constexpr uint32_t HEIGHT = 64;

    const std::string vertexShader =
        "precision highp float;\n"
        "attribute vec3 position;\n"
        "void main(void) { gl_Position = vec4(position, 1.0); }\n";

    const std::string fragmentShader =
        "precision highp float;\n"
        "uniform vec2 targetSize;\n"
        "void main(void) {\n"
        "    gl_FragColor = vec4(gl_FragCoord.y / targetSize.y, 0.0, 0.0, 1.0);\n"
        "}\n";

    auto pixels = RenderFullScreenQuad(WIDTH, HEIGHT, vertexShader, fragmentShader, false);
    ASSERT_EQ(pixels.size(), static_cast<size_t>(WIDTH) * HEIGHT * 4);

    const auto red = [&pixels](uint32_t row) {
        return static_cast<int>(pixels[static_cast<size_t>(row) * WIDTH * 4]);
    };

    std::cout << "row 0 red=" << red(0)
              << ", row " << (HEIGHT / 2) << " red=" << red(HEIGHT / 2)
              << ", row " << (HEIGHT - 1) << " red=" << red(HEIGHT - 1) << std::endl;

    // The ramp must run bright at the top to dark at the bottom.
    EXPECT_GT(red(0), 200) << "top row should hold the largest gl_FragCoord.y";
    EXPECT_LT(red(HEIGHT - 1), 55) << "bottom row should hold the smallest gl_FragCoord.y";

    // Monotonicity is checked instead of exact values so the test stays valid
    // under any monotonic transfer function the backend may apply.
    for (uint32_t row = 1; row < HEIGHT; ++row)
    {
        ASSERT_LE(red(row), red(row - 1))
            << "gl_FragCoord.y ramp is not monotonically decreasing at row " << row;
    }
#endif
}

// Indexing a screen-sized texture with gl_FragCoord must give the same image as
// indexing it with the interpolated UVs of a full-screen quad. This is the
// pattern used by order-independent transparency, TAA and screen space
// curvature, and it only holds if the gl_FragCoord correction and
// FlipSamplerCoordinatesTraverser compose to a no-op.
//
// Comparing the two addressing modes against each other rather than against the
// source pixels keeps the test independent of how createRawTexture lays its data
// out in memory.
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

    // Guard against a vacuous pass: the source must actually vary down the image,
    // otherwise a vertical mirror would be undetectable.
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
