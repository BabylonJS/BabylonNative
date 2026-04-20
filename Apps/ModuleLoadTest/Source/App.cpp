#include "App.h"

#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Plugins/NativeEncoding.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <Babylon/Polyfills/Blob.h>
#include <Babylon/Polyfills/Canvas.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/Window.h>
#include <Babylon/Polyfills/XMLHttpRequest.h>

#include <future>
#include <iostream>
#include <optional>

namespace ModuleLoadTest
{
    ModuleSnapshot RunBoot(const Babylon::Graphics::Configuration& config)
    {
        // Bring up the real graphics device. This triggers the backend
        // (D3D11/D3D12/Metal/OpenGL) and GPU ICD DLLs to load.
        Babylon::Graphics::Device device{config};
        device.StartRenderingCurrentFrame();

        std::optional<Babylon::Polyfills::Canvas> nativeCanvas;

        Babylon::AppRuntime::Options options{};
        options.UnhandledExceptionHandler = [](const Napi::Error& error) {
            std::cerr << "[Uncaught Error] " << Napi::GetErrorString(error) << std::endl;
            std::quick_exit(1);
        };

        Babylon::AppRuntime runtime{options};

        std::promise<void> initDone;

        runtime.Dispatch([&device, &nativeCanvas, &initDone](Napi::Env env) {
            device.AddToJavaScript(env);

            Babylon::Polyfills::XMLHttpRequest::Initialize(env);
            Babylon::Polyfills::Console::Initialize(env, [](const char* message, Babylon::Polyfills::Console::LogLevel) {
                std::cout << message << std::endl;
            });
            Babylon::Polyfills::Window::Initialize(env);
            Babylon::Polyfills::Blob::Initialize(env);
            nativeCanvas.emplace(Babylon::Polyfills::Canvas::Initialize(env));
            Babylon::Plugins::NativeEngine::Initialize(env);
            Babylon::Plugins::NativeEncoding::Initialize(env);

            initDone.set_value();
        });

        initDone.get_future().get();

        device.FinishRenderingCurrentFrame();

        // Snapshot immediately after the first frame completes. At this point
        // the graphics backend and every polyfill/plugin has been initialized,
        // but no user JS has executed.
        return CaptureSnapshot();
    }
}
