#include <Babylon/AppRuntime.h>
#include <gtest/gtest.h>
#include <Babylon/ScriptLoader.h>
#include <Babylon/Polyfills/AbortController.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/Performance.h>
#include <Babylon/Polyfills/Scheduling.h>
#include <Babylon/Polyfills/URL.h>
#include <Babylon/Polyfills/WebSocket.h>
#include <Babylon/Polyfills/XMLHttpRequest.h>
#include <Babylon/Polyfills/Fetch.h>
#include <Babylon/Polyfills/Blob.h>
#include <Babylon/Polyfills/File.h>
#include <Babylon/Polyfills/TextDecoder.h>
#include <Babylon/Polyfills/TextEncoder.h>
#include <cstdint>
#include <future>
#include <iostream>
#include <string>

namespace
{
    const char* EnumToString(Babylon::Polyfills::Console::LogLevel logLevel)
    {
        switch (logLevel)
        {
            case Babylon::Polyfills::Console::LogLevel::Log:
                return "log";
            case Babylon::Polyfills::Console::LogLevel::Warn:
                return "warn";
            case Babylon::Polyfills::Console::LogLevel::Error:
                return "error";
        }

        return "unknown";
    }
}

TEST(JavaScript, All)
{
    // Change this to true to wait for the JavaScript debugger to attach (only applies to V8)
    constexpr const bool waitForDebugger = false;

    std::promise<int32_t> exitCodePromise;

    Babylon::AppRuntime::Options options{};

    options.UnhandledExceptionHandler = [&exitCodePromise](const Napi::Error& error) {
        std::cerr << "[Uncaught Error] " << Napi::GetErrorString(error) << std::endl;
        std::cerr.flush();

        exitCodePromise.set_value(-1);
    };

    if (waitForDebugger)
    {
        std::cout << "Waiting for debugger..." << std::endl;
        options.WaitForDebugger = true;
    }

    Babylon::AppRuntime runtime{options};

    runtime.Dispatch([&exitCodePromise](Napi::Env env) mutable {
        Babylon::Polyfills::Console::Initialize(env, [env](const char* message, Babylon::Polyfills::Console::LogLevel logLevel) {
            std::cout << "[" << EnumToString(logLevel) << "] " << message;
            if (logLevel == Babylon::Polyfills::Console::LogLevel::Error)
            {
                std::string stack = Babylon::Polyfills::Console::CaptureCurrentJsStack(env);
                if (!stack.empty())
                {
                    std::cout << std::endl << stack;
                }
            }
            std::cout << std::endl;
            std::cout.flush();
        });

        Babylon::Polyfills::AbortController::Initialize(env);
        Babylon::Polyfills::Performance::Initialize(env);
        Babylon::Polyfills::Scheduling::Initialize(env);
        Babylon::Polyfills::URL::Initialize(env);
        Babylon::Polyfills::WebSocket::Initialize(env);
        Babylon::Polyfills::XMLHttpRequest::Initialize(env);
        Babylon::Polyfills::Fetch::Initialize(env);
        Babylon::Polyfills::Blob::Initialize(env);
        Babylon::Polyfills::File::Initialize(env);
        Babylon::Polyfills::TextDecoder::Initialize(env);
        Babylon::Polyfills::TextEncoder::Initialize(env);

        auto setExitCodeCallback = Napi::Function::New(
            env, [&exitCodePromise](const Napi::CallbackInfo& info) {
                Napi::Env env = info.Env();
                exitCodePromise.set_value(info[0].As<Napi::Number>().Int32Value());
            },
            "setExitCode");
        env.Global().Set("setExitCode", setExitCodeCallback);

        env.Global().Set("hostPlatform", Napi::Value::From(env, JSRUNTIMEHOST_PLATFORM));
        env.Global().Set("hostEngine", Napi::Value::From(env, NAPI_JAVASCRIPT_ENGINE));
    });

    Babylon::ScriptLoader loader{runtime};
    loader.Eval("location = { href: '' };", ""); // Required for Mocha.js as we do not have a location
    loader.LoadScript("app:///Assets/tests.js");

    auto exitCode{exitCodePromise.get_future().get()};

    EXPECT_EQ(exitCode, 0);
}
