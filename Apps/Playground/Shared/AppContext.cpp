#include "AppContext.h"

#include <Babylon/AppRuntime.h>
#include <Babylon/DebugTrace.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/PerfTrace.h>
#include <Babylon/ScriptLoader.h>

#include <Babylon/Plugins/NativeCamera.h>
#include <Babylon/Plugins/NativeCapture.h>
#include <Babylon/Plugins/NativeEncoding.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <Babylon/Plugins/NativeInput.h>
#include <Babylon/Plugins/NativeOptimizations.h>
#include <Babylon/Plugins/NativeTracing.h>
#include <Babylon/Plugins/ShaderCache.h>
#include <Babylon/Plugins/TestUtils.h>

#include <Babylon/Polyfills/Blob.h>
#include <Babylon/Polyfills/Canvas.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/Window.h>
#include <Babylon/Polyfills/XMLHttpRequest.h>

#include <iostream>
#include <sstream>

namespace
{
    const char* GetLogLevelString(Babylon::Polyfills::Console::LogLevel logLevel)
    {
        switch (logLevel)
        {
            case Babylon::Polyfills::Console::LogLevel::Log:
                return "Log";
            case Babylon::Polyfills::Console::LogLevel::Warn:
                return "Warn";
            case Babylon::Polyfills::Console::LogLevel::Error:
                return "Error";
            default:
                return "";
        }
    }
}

AppContext::AppContext(
    Babylon::Graphics::WindowT window,
    size_t width,
    size_t height,
    DebugLogCallback debugLog,
    AdditionalInitCallback additionalInit)
{
    Babylon::DebugTrace::EnableDebugTrace(true);
    Babylon::DebugTrace::SetTraceOutput(debugLog);
    Babylon::PerfTrace::SetLevel(Babylon::PerfTrace::Level::Mark);

    Babylon::Graphics::Configuration graphicsConfig{};
    graphicsConfig.Window = window;
    graphicsConfig.Width = width;
    graphicsConfig.Height = height;
    graphicsConfig.MSAASamples = 4;

    m_device.emplace(graphicsConfig);
    m_deviceUpdate.emplace(m_device->GetUpdate("update"));

    Babylon::Plugins::ShaderCache::Enable();

    m_device->StartRenderingCurrentFrame();
    m_deviceUpdate->Start();

    Babylon::AppRuntime::Options options{};

    options.EnableDebugger = true;

    options.UnhandledExceptionHandler = [debugLog](const Napi::Error& error) {
        std::ostringstream ss{};
        ss << "[Uncaught Error] " << Napi::GetErrorString(error);

        debugLog(ss.str().data());

        std::quick_exit(1);
    };

    m_runtime.emplace(options);

    m_runtime->Dispatch([this, window, debugLog, additionalInit = std::move(additionalInit)](Napi::Env env) {
        m_device->AddToJavaScript(env);

        Babylon::Polyfills::Blob::Initialize(env);

        Babylon::Polyfills::Console::Initialize(env, [debugLog](const char* message, Babylon::Polyfills::Console::LogLevel logLevel) {
            std::ostringstream ss{};
            ss << "[" << GetLogLevelString(logLevel) << "] " << message;
            debugLog(ss.str().data());
        });

        Babylon::Polyfills::Window::Initialize(env);

        Babylon::Polyfills::XMLHttpRequest::Initialize(env);

        m_canvas.emplace(Babylon::Polyfills::Canvas::Initialize(env));

        Babylon::Plugins::NativeTracing::Initialize(env);

        Babylon::Plugins::NativeEncoding::Initialize(env);

        Babylon::Plugins::NativeEngine::Initialize(env);

        Babylon::Plugins::NativeOptimizations::Initialize(env);

        Babylon::Plugins::NativeCapture::Initialize(env);

        Babylon::Plugins::NativeCamera::Initialize(env);

        m_input = &Babylon::Plugins::NativeInput::CreateForJavaScript(env);

        Babylon::Plugins::TestUtils::Initialize(env, window);

        if (additionalInit)
        {
            additionalInit(env);
        }
    });

    m_scriptLoader.emplace(*m_runtime);
    m_scriptLoader->LoadScript("app:///Scripts/ammo.js");
    // Commenting out recast.js for now because v8jsi is incompatible with asm.js.
    // m_scriptLoader->LoadScript("app:///Scripts/recast.js");
    m_scriptLoader->LoadScript("app:///Scripts/babylon.max.js");
    m_scriptLoader->LoadScript("app:///Scripts/babylonjs.loaders.js");
    m_scriptLoader->LoadScript("app:///Scripts/babylonjs.materials.js");
    m_scriptLoader->LoadScript("app:///Scripts/babylon.gui.js");
    m_scriptLoader->LoadScript("app:///Scripts/meshwriter.min.js");
    m_scriptLoader->LoadScript("app:///Scripts/babylonjs.serializers.js");
}

AppContext::~AppContext()
{
    if (m_device)
    {
        m_deviceUpdate->Finish();
        m_device->FinishRenderingCurrentFrame();
    }

    Babylon::Plugins::ShaderCache::Disable();

    m_scriptLoader.reset();
    m_canvas.reset();
    m_input = {};
    m_runtime.reset();
    m_deviceUpdate.reset();
    m_device.reset();
}
