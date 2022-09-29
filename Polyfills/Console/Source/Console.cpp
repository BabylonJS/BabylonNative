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

    void AddMethod(Napi::Object& console, const char* functionName, Babylon::Polyfills::Console::LogLevel logLevel, Babylon::Polyfills::Console::CallbackT callback)
    {
        auto existingFunction = std::make_shared<Napi::FunctionReference>(Napi::Persistent(console.Get(functionName).As<Napi::Function>()));
        console.Set(functionName, Napi::Function::New(console.Env(), [callback, existingFunction = std::move(existingFunction), logLevel](const Napi::CallbackInfo& info)
        {
            InvokeCallback(callback, info, logLevel);

            if (!existingFunction->Value().IsUndefined())
            {
                Call(existingFunction->Value(), info);
            }
        }, functionName));
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
            env.Global().Set(JS_INSTANCE_NAME, console);
        }

        AddMethod(console, "log", LogLevel::Log, callback);
        AddMethod(console, "warn", LogLevel::Warn, callback);
        AddMethod(console, "error", LogLevel::Error, callback);
    }
}
