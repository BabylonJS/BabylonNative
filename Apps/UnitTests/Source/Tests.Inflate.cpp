#include <gtest/gtest.h>


#include <Babylon/AppRuntime.h>
#include <Babylon/Polyfills/XMLHttpRequest.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/Window.h>
#include <Babylon/Polyfills/DecompressionStream.h>
#include <Babylon/Plugins/NativeZip.h>
#include <Babylon/ScriptLoader.h>
#include <iostream>
#include <future>

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

namespace
{
    void RunInflateScript(const std::string& url)
    {
        // Change this to true to wait for the JavaScript debugger to attach (only applies to V8)
        constexpr const bool waitForDebugger = false;

        std::promise<int32_t> exitCodePromise;

        Babylon::AppRuntime::Options options{};

        options.UnhandledExceptionHandler = [&exitCodePromise](const Napi::Error& error) {
            std::cerr << "[Uncaught Error] " << Napi::GetErrorString(error) << std::endl;
            std::cerr.flush();

            exitCodePromise.set_exception(std::make_exception_ptr(std::exception{}));
        };

        if (waitForDebugger)
        {
            std::cout << "Waiting for debugger..." << std::endl;
            options.WaitForDebugger = true;
        }

        Babylon::AppRuntime runtime{options};

        runtime.Dispatch([&exitCodePromise](Napi::Env env) {

            Babylon::Plugins::NativeZip::Initialize(env);
            Babylon::Polyfills::XMLHttpRequest::Initialize(env);
            Babylon::Polyfills::DecompressionStream::Initialize(env);
            Babylon::Polyfills::Console::Initialize(env, [](const char* message, Babylon::Polyfills::Console::LogLevel logLevel) {
                std::cout << "[" << EnumToString(logLevel) << "] " << message << std::endl;
                std::cout.flush();
            });
            Babylon::Polyfills::Window::Initialize(env);

            auto setExitCodeCallback = Napi::Function::New(
                env, [&exitCodePromise](const Napi::CallbackInfo& info) {
                    Napi::Env env = info.Env();
                    exitCodePromise.set_value(info[0].As<Napi::Number>().Int32Value());
                },
                "setExitCode");
            env.Global().Set("setExitCode", setExitCodeCallback);
        });

        Babylon::ScriptLoader loader{runtime};
        loader.Eval("location = { href: '' };", ""); // Required for Mocha.js as we do not have a location in Babylon Native
        loader.LoadScript(url);

        auto exitCode{exitCodePromise.get_future().get()};
        EXPECT_EQ(exitCode, 0);
    }
}
/*
TEST(Inflate, zip)
{
    RunInflateScript("app:///Assets/tests.inflate.zip.js");
}
*/
TEST(Inflate, DecompressionStream)
{
    RunInflateScript("app:///Assets/tests.inflate.stream.js");
}
