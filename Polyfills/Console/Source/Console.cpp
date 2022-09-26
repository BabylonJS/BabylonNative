#include <Babylon/Polyfills/Console.h>

#include <array>
#include <functional>
#include <sstream>

namespace
{
    constexpr const char* JS_INSTANCE_NAME{"console"};

    void Call(Napi::Function func, const Napi::CallbackInfo& info)
    {
        static std::array<Napi::Value, 6> staticArgs{};
        const size_t argc = info.Length();

        if (info.Length() < std::size(staticArgs))
        {
            for (size_t i = 0; i < argc; ++i)
            {
                staticArgs[i] = info[i];
            }

            func.Call(argc, staticArgs.data());
        }
        else
        {
            std::vector<Napi::Value> args(argc);
            for (size_t i = 0; i < argc; ++i)
            {
                args[i] = info[i];
            }

            func.Call(argc, args.data());
        }
    }

    void InvokeCallback(Babylon::Polyfills::Console::CallbackT callback, const Napi::CallbackInfo& info, Babylon::Polyfills::Console::LogLevel logLevel)
    {
        std::stringstream ss{};
        for (size_t i = 0; i < info.Length(); i++)
        {
            if (i > 0)
            {
                ss << " ";
            }
            ss << info[i].ToString().Utf8Value().c_str();
        }
        ss << std::endl;

        callback(ss.str().c_str(), logLevel);
    }
}

namespace Babylon::Polyfills::Console
{
    void Initialize(Napi::Env env, CallbackT callback)
    {
        Napi::HandleScope scope{env};

        auto console = env.Global().Get(JS_INSTANCE_NAME).As<Napi::Object>();
        if (console.IsUndefined())
        {
            console = Napi::Object::New(env);

            console.Set("log", Napi::Function::New(env, [callback](const Napi::CallbackInfo& info)
            {
                InvokeCallback(callback, info, LogLevel::Log);
            }, "log"));

            console.Set("warn", Napi::Function::New(env, [callback](const Napi::CallbackInfo& info)
            {
                InvokeCallback(callback, info, LogLevel::Warn);
            }, "warn"));

            console.Set("error", Napi::Function::New(env, [callback](const Napi::CallbackInfo& info)
            {
                InvokeCallback(callback, info, LogLevel::Error);
            }, "error"));

            env.Global().Set(JS_INSTANCE_NAME, console);
        }
        else
        {
            auto existingLog = console.Get("log").As<Napi::Function>();
            console.Set("log", Napi::Function::New(env, [existingLog = Napi::Persistent(existingLog), callback](const Napi::CallbackInfo& info)
            {
                InvokeCallback(callback, info, LogLevel::Log);
                Call(existingLog.Value(), info);
            }, "log"));

            auto existingWarn = console.Get("warn").As<Napi::Function>();
            console.Set("warn", Napi::Function::New(env, [existingWarn = Napi::Persistent(existingWarn), callback](const Napi::CallbackInfo& info)
            {
                InvokeCallback(callback, info, LogLevel::Warn);
                Call(existingWarn.Value(), info);
            }, "warn"));

            auto existingError = console.Get("error").As<Napi::Function>();
            console.Set("error", Napi::Function::New(env, [existingError = Napi::Persistent(existingError), callback](const Napi::CallbackInfo& info)
            {
                InvokeCallback(callback, info, LogLevel::Error);
                Call(existingError.Value(), info);
            }, "error"));
        }
    }
}
