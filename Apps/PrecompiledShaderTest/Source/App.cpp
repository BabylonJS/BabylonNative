#include "App.h"

#include <Babylon/AppRuntime.h>
#include <Babylon/ScriptLoader.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <Babylon/Plugins/ShaderCache.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/Window.h>

#include <cmath>
#include <cstdlib>
#include <fstream>
#include <future>
#include <iostream>

namespace
{
    void CatchAndLogError(Napi::Promise jsPromise)
    {
        auto jsOnRejected = Napi::Function::New(jsPromise.Env(), [](const Napi::CallbackInfo& info) {
            auto console = info.Env().Global().Get("console").As<Napi::Object>();
            console.Get("error").As<Napi::Function>().Call(console, {info[0]});
            std::exit(1);
        });

        jsPromise.Get("catch").As<Napi::Function>().Call(jsPromise, {jsOnRejected});
    }

    // Compare two images pixel-by-pixel.
    // Returns true when the images match within the given tolerance.
    bool CompareImages(
        const ImageData& rendered,
        const ImageData& reference,
        uint32_t perChannelThreshold = 25,
        double maxErrorPercent = 2.5)
    {
        if (rendered.Width != reference.Width || rendered.Height != reference.Height)
        {
            std::cerr << "Image dimensions do not match: "
                      << rendered.Width << "x" << rendered.Height << " vs "
                      << reference.Width << "x" << reference.Height << std::endl;
            return false;
        }

        const size_t pixelCount = static_cast<size_t>(rendered.Width) * rendered.Height;
        const size_t byteCount = pixelCount * 4;

        if (rendered.Pixels.size() != byteCount || reference.Pixels.size() != byteCount)
        {
            std::cerr << "Unexpected pixel buffer size." << std::endl;
            return false;
        }

        size_t diffCount = 0;
        for (size_t i = 0; i < byteCount; i += 4)
        {
            // Compare R, G, B channels (skip alpha).
            if (std::abs(static_cast<int>(rendered.Pixels[i])     - static_cast<int>(reference.Pixels[i]))     >= static_cast<int>(perChannelThreshold) ||
                std::abs(static_cast<int>(rendered.Pixels[i + 1]) - static_cast<int>(reference.Pixels[i + 1])) >= static_cast<int>(perChannelThreshold) ||
                std::abs(static_cast<int>(rendered.Pixels[i + 2]) - static_cast<int>(reference.Pixels[i + 2])) >= static_cast<int>(perChannelThreshold))
            {
                ++diffCount;
            }
        }

        const double errorPercent = (static_cast<double>(diffCount) * 100.0) / static_cast<double>(pixelCount);
        std::cout << "Image comparison: " << diffCount << "/" << pixelCount
                  << " pixels differ (" << errorPercent << "%)" << std::endl;

        return errorPercent <= maxErrorPercent;
    }

    bool LoadShaderCache(const std::filesystem::path& cachePath)
    {
        std::ifstream cacheFile(cachePath, std::ios::binary);
        if (!cacheFile.is_open())
        {
            std::cerr << "Failed to open shader cache file: " << cachePath.string() << std::endl;
            return false;
        }

        const uint32_t count = Babylon::Plugins::ShaderCache::Load(cacheFile);
        std::cout << "Loaded " << count << " shader(s) from cache: " << cachePath.string() << std::endl;
        return count > 0;
    }
}

int RunApp(
    const std::filesystem::path& executablePath,
    Babylon::Graphics::Configuration config,
    Babylon::Plugins::ExternalTexture externalTexture,
    std::function<void(const std::filesystem::path&)> saveTexture,
    std::function<ImageData(const std::filesystem::path&)> loadImage)
{
    // Enable and load the pre-compiled shader cache.
    Babylon::Plugins::ShaderCache::Enable();
    auto cachePath = executablePath / "shaders.bin";
    if (!LoadShaderCache(cachePath))
    {
        std::cerr << "No shaders loaded from cache. Aborting." << std::endl;
        return EXIT_FAILURE;
    }

    // Create the Babylon Native graphics device and update.
    auto device = Babylon::Graphics::Device(config);

    // Start rendering a frame to unblock the JavaScript from queuing graphics
    // commands.
    device.StartRenderingCurrentFrame();

    // Create a Babylon Native application runtime which hosts a JavaScript
    // engine on a new thread.
    Babylon::AppRuntime runtime{{.UnhandledExceptionHandler = [](const Napi::Error& e) {
        Babylon::AppRuntime::DefaultUnhandledExceptionHandler(e);
        std::quick_exit(1);
    }}};

    runtime.Dispatch([&device](Napi::Env env) {
        env.Global().Set("globalThis", env.Global());

        // Add the Babylon Native graphics device to the JavaScript environment.
        device.AddToJavaScript(env);

        // Initialize polyfills.
        Babylon::Polyfills::Console::Initialize(env, [](const char* message, auto) {
            std::cout << message << std::endl;
        });
        Babylon::Polyfills::Window::Initialize(env);
        Babylon::Plugins::NativeEngine::Initialize(env);
    });

    // Load scripts.
    Babylon::ScriptLoader loader{runtime};
    loader.LoadScript("app:///index.js");

    std::promise<void> addToContext{};
    std::promise<void> startup{};

    // Create an external texture for the render target texture and pass it to
    // the `startup` JavaScript function.
    loader.Dispatch([externalTexture = std::move(externalTexture), &addToContext, &startup](Napi::Env env) {
        auto jsPromise = externalTexture.AddToContextAsync(env);
        addToContext.set_value();

        auto jsOnFulfilled = Napi::Function::New(env, [&startup](const Napi::CallbackInfo& info) {
            auto nativeTexture = info[0];
            info.Env().Global().Get("startup").As<Napi::Function>().Call(
                {
                    nativeTexture,
                    Napi::Value::From(info.Env(), WIDTH),
                    Napi::Value::From(info.Env(), HEIGHT),
                });
            startup.set_value();
        });

        jsPromise = jsPromise.Get("then").As<Napi::Function>().Call(jsPromise, {jsOnFulfilled}).As<Napi::Promise>();
        CatchAndLogError(jsPromise);
    });

    // Wait for `AddToContextAsync` to be called.
    addToContext.get_future().wait();

    // Render a frame so that `AddToContextAsync` will complete.
    device.FinishRenderingCurrentFrame();

    // Reopen the gate so JS can continue running (startup may issue bgfx commands).
    device.StartRenderingCurrentFrame();

    // Wait for `startup` to finish.
    startup.get_future().wait();

    // Close the frame opened above.
    device.FinishRenderingCurrentFrame();

    // Start a new frame for rendering the scene.
    device.StartRenderingCurrentFrame();

    std::promise<void> renderScene{};

    // Call `renderSceneAsync` in JavaScript.
    loader.Dispatch([&renderScene](Napi::Env env) {
        std::cout << "Rendering scene" << std::endl;

        auto jsPromise = env.Global().Get("renderSceneAsync").As<Napi::Function>().Call({}).As<Napi::Promise>();

        auto jsOnFulfilled = Napi::Function::New(env, [&renderScene](const Napi::CallbackInfo&) {
            renderScene.set_value();
        });

        jsPromise = jsPromise.Get("then").As<Napi::Function>().Call(jsPromise, {jsOnFulfilled}).As<Napi::Promise>();
        CatchAndLogError(jsPromise);
    });

    // Wait for the scene to render.
    renderScene.get_future().wait();

    // Finish the frame.
    device.FinishRenderingCurrentFrame();

    // Save the rendered output as a PNG.
    auto filePath = executablePath / "output.png";
    std::cout << "Writing " << filePath.filename().string() << std::endl;
    saveTexture(filePath);

    // Compare the rendered output with the reference image.
    auto referencePath = executablePath / "reference.png";
    if (!std::filesystem::exists(referencePath))
    {
        std::cerr << "ERROR: Reference image not found: " << referencePath.string() << std::endl;
        Babylon::Plugins::ShaderCache::Disable();
        return EXIT_FAILURE;
    }

    std::cout << "Comparing with " << referencePath.filename().string() << std::endl;
    auto renderedImage = loadImage(filePath);
    auto referenceImage = loadImage(referencePath);

    if (!CompareImages(renderedImage, referenceImage))
    {
        std::cerr << "ERROR: Rendered image does not match reference image." << std::endl;
        Babylon::Plugins::ShaderCache::Disable();
        return EXIT_FAILURE;
    }

    Babylon::Plugins::ShaderCache::Disable();
    return EXIT_SUCCESS;
}
