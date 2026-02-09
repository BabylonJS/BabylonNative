#include "AppContext.h"

#include <Babylon/AppRuntime.h>
#include <Babylon/DebugTrace.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/ScriptLoader.h>

#include <Babylon/Plugins/NativeWebGPU.h>

#include <Babylon/Polyfills/Blob.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/URL.h>
#include <Babylon/Polyfills/Window.h>
#include <Babylon/Polyfills/XMLHttpRequest.h>

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

    Babylon::Graphics::Configuration graphicsConfig{};
    graphicsConfig.Window = window;
    graphicsConfig.Width = width;
    graphicsConfig.Height = height;
    graphicsConfig.MSAASamples = 4;

    m_device.emplace(graphicsConfig);
    m_deviceUpdate.emplace(m_device->GetUpdate("update"));

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

    m_runtime->Dispatch([this, debugLog, additionalInit = std::move(additionalInit)](Napi::Env env) {
        m_device->AddToJavaScript(env);

        Babylon::Polyfills::Blob::Initialize(env);

        Babylon::Polyfills::Console::Initialize(env, [debugLog](const char* message, Babylon::Polyfills::Console::LogLevel logLevel) {
            std::ostringstream ss{};
            ss << "[" << GetLogLevelString(logLevel) << "] " << message;
            debugLog(ss.str().data());
        });

        Babylon::Polyfills::Window::Initialize(env);
        Babylon::Polyfills::URL::Initialize(env);
        Babylon::Polyfills::XMLHttpRequest::Initialize(env);

        Babylon::Plugins::NativeWebGPU::Initialize(env);

        if (additionalInit)
        {
            additionalInit(env);
        }
    });

    m_scriptLoader.emplace(*m_runtime);
    m_scriptLoader->LoadScript("app:///Scripts/ammo.js");
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

    m_scriptLoader.reset();
    m_input = {};
    m_runtime.reset();
    m_deviceUpdate.reset();
    m_device.reset();
}
