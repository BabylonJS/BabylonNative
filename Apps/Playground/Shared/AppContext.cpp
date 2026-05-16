#include "AppContext.h"
#include "Diagnostics.h"

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

#include <Babylon/Polyfills/AbortController.h>
#include <Babylon/Polyfills/Blob.h>
#include <Babylon/Polyfills/Canvas.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/File.h>
#include <Babylon/Polyfills/Performance.h>
#include <Babylon/Polyfills/TextDecoder.h>
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
    AdditionalInitCallback additionalInit,
    PlaygroundOptions playgroundOptions)
{
    Babylon::DebugTrace::EnableDebugTrace(playgroundOptions.DebugTrace.value_or(true));
    Babylon::DebugTrace::SetTraceOutput(debugLog);

    Babylon::PerfTrace::Level perfLevel{Babylon::PerfTrace::Level::Mark};
    if (playgroundOptions.PerfTrace.has_value())
    {
        const auto& v = *playgroundOptions.PerfTrace;
        if (v == "None" || v == "none")
        {
            perfLevel = Babylon::PerfTrace::Level::None;
        }
        else if (v == "Log" || v == "log" || v == "Detail" || v == "detail")
        {
            perfLevel = Babylon::PerfTrace::Level::Log;
        }
    }
    Babylon::PerfTrace::SetLevel(perfLevel);

    Babylon::Graphics::Configuration graphicsConfig{};
    graphicsConfig.Window = window;
    graphicsConfig.Width = width;
    graphicsConfig.Height = height;
    graphicsConfig.MSAASamples = 4;

    m_device.emplace(graphicsConfig);
    m_deviceUpdate.emplace(m_device->GetUpdate("update"));

    // Mirror bgfx trace output (BgfxCallback::traceVargs) to debugLog so it
    // reaches stdout in headless mode, not just OutputDebugString.
    m_device->SetDiagnosticOutput(debugLog);

    Babylon::Plugins::ShaderCache::Enable();

    m_device->StartRenderingCurrentFrame();
    m_deviceUpdate->Start();

    Babylon::AppRuntime::Options options{};

    options.EnableDebugger = true;

    options.UnhandledExceptionHandler = [debugLog](const Napi::Error& error) {
        // Bx-style banner with native + JS callstack, then keep the legacy
        // "[Uncaught Error] ..." one-liner for log scrapers.
        const std::string js = Napi::GetErrorString(error);

        Diagnostics::DumpFailure(
            "UNCAUGHT JS ERROR",
            nullptr,
            0,
            0,
            "%s",
            js.c_str());

        std::ostringstream ss{};
        ss << "[Uncaught Error] " << js;
        debugLog(ss.str().data());

        Diagnostics::SetExitCode(1);
        Diagnostics::PrintFinishLine();
        std::quick_exit(1);
    };

    m_runtime.emplace(options);

    m_runtime->Dispatch([this, window, debugLog, additionalInit = std::move(additionalInit), playgroundOptions = std::move(playgroundOptions)](Napi::Env env) {
        m_device->AddToJavaScript(env);

        {
            auto js = Napi::Object::New(env);
            js.Set("listTests",          Napi::Boolean::New(env, playgroundOptions.ListTests));
            js.Set("headless",           Napi::Boolean::New(env, playgroundOptions.Headless));
            js.Set("breakOnFail",        Napi::Boolean::New(env, playgroundOptions.BreakOnFail));
            js.Set("generateReferences", Napi::Boolean::New(env, playgroundOptions.GenerateReferences));
            js.Set("runOnce",            Napi::Boolean::New(env, playgroundOptions.RunOnce));
            js.Set("includeExcluded",    Napi::Boolean::New(env, playgroundOptions.IncludeExcluded));
            if (playgroundOptions.SaveResults.has_value())
            {
                js.Set("saveResults", Napi::Boolean::New(env, *playgroundOptions.SaveResults));
            }
            if (playgroundOptions.CaptureFrame.has_value())
            {
                js.Set("captureFrame", Napi::Number::New(env, *playgroundOptions.CaptureFrame));
            }

            auto filters = Napi::Array::New(env, playgroundOptions.TestFilters.size());
            for (uint32_t i = 0; i < playgroundOptions.TestFilters.size(); ++i)
            {
                filters[i] = Napi::String::New(env, playgroundOptions.TestFilters[i]);
            }
            js.Set("testFilters", filters);

            auto indices = Napi::Array::New(env, playgroundOptions.TestIndices.size());
            for (uint32_t i = 0; i < playgroundOptions.TestIndices.size(); ++i)
            {
                indices[i] = Napi::Number::New(env, playgroundOptions.TestIndices[i]);
            }
            js.Set("testIndices", indices);

            env.Global().Set("_playgroundOptions", js);
        }

        Babylon::Polyfills::Blob::Initialize(env);
        Babylon::Polyfills::File::Initialize(env);

        Babylon::Polyfills::Console::Initialize(env, [env, debugLog](const char* message, Babylon::Polyfills::Console::LogLevel logLevel) {
            std::ostringstream ss{};
            ss << "[" << GetLogLevelString(logLevel) << "] " << message;
            debugLog(ss.str().data());

            // Promote console.error to a banner with JS + native callstack.
            // Babylon.js routes recoverable errors here.
            if (logLevel == Babylon::Polyfills::Console::LogLevel::Error)
            {
                auto jsStack = Babylon::Polyfills::Console::CaptureCurrentJsStack(env);
                Diagnostics::DumpFailure(
                    "JS CONSOLE ERROR",
                    nullptr,
                    0,
                    0,
                    "%s%s%s",
                    message != nullptr ? message : "(null)",
                    jsStack.empty() ? "" : "\nJS callstack:\n",
                    jsStack.c_str());
            }
        });

        Babylon::Polyfills::Performance::Initialize(env);

        Babylon::Polyfills::Window::Initialize(env);

        Babylon::Polyfills::AbortController::Initialize(env);

        Babylon::Polyfills::TextDecoder::Initialize(env);

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
    m_scriptLoader->LoadScript("app:///Scripts/dom_polyfill.js");
    m_scriptLoader->LoadScript("app:///Scripts/fetch_polyfill.js");
    m_scriptLoader->LoadScript("app:///Scripts/ammo.js");
    // Commenting out recast.js for now because v8jsi is incompatible with asm.js.
    // m_scriptLoader->LoadScript("app:///Scripts/recast.js");
    m_scriptLoader->LoadScript("app:///Scripts/babylon.max.js");
    m_scriptLoader->LoadScript("app:///Scripts/cube_texture_polyfill.js");
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
