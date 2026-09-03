#include <gtest/gtest.h>

#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/Window.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <Babylon/Plugins/ExternalTexture.h>
#include <Babylon/ScriptLoader.h>

#include "Helpers.h"

#include <cstdlib>
#include <future>
#include <iostream>
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
    // Renders a full-screen quad into a width x height render target, returning RGBA8
    // pixels in memory order. The fragment shader may declare `uniform vec2 targetSize`
    // and, when withInputTexture is set, `uniform sampler2D inputSampler` bound to a
    // texture whose row y holds makeRow(y).
    std::vector<uint8_t> RenderFullScreenQuad(
        uint32_t width,
        uint32_t height,
        const std::string& vertexShader,
        const std::string& fragmentShader,
        bool withInputTexture)
    {
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

        renderDone.get_future().get();

        device.FinishRenderingCurrentFrame();

        auto pixels = Helpers::ReadPixels(device.GetPlatformInfo(), outputTexture, width, height);
        Helpers::DestroyTexture(outputTexture);
        return pixels;
    }
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
