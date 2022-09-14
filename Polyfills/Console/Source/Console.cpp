#include "Console.h"

#include <functional>
#include <sstream>

namespace
{
    std::vector<Napi::Value> GetCallbackInfoArgs(const Napi::CallbackInfo& info)
    {
        auto args = std::vector<Napi::Value>();
        for (unsigned int index = 0; index < info.Length(); index++)
            args.push_back(info[index]);
        return args;
    }
}

namespace Babylon::Polyfills::Internal
{
    static constexpr auto JS_CONSTRUCTOR_NAME = "console";

    void Console::CreateInstance(Napi::Env env, Babylon::Polyfills::Console::CallbackT callback)
    {
        Napi::HandleScope scope{env};

        Napi::Function func = ParentT::DefineClass(
            env,
            JS_CONSTRUCTOR_NAME,
            {
                ParentT::StaticMethod("log", &Console::Log),
                ParentT::StaticMethod("warn", &Console::Warn),
                ParentT::StaticMethod("error", &Console::Error),
            });

        const auto existingConsole = env.Global().Get(JS_INSTANCE_NAME);
        if (!existingConsole.IsUndefined())
        {
            s_engineConsole = std::make_unique<Napi::ObjectReference>(Napi::Persistent(existingConsole.As<Napi::Object>()));
        }
        env.Global().Set(JS_CONSTRUCTOR_NAME, func);
        s_callback = callback;
    }

    Console::Console(const Napi::CallbackInfo& info)
        : ParentT{info}
    {
    }

    void Console::Log(const Napi::CallbackInfo& info)
    {
        InvokeEngineCallback("log", info);
        InvokeCallback(info, Babylon::Polyfills::Console::LogLevel::Log);
    }

    void Console::Warn(const Napi::CallbackInfo& info)
    {
        InvokeEngineCallback("warn", info);
        InvokeCallback(info, Babylon::Polyfills::Console::LogLevel::Warn);
    }

    void Console::Error(const Napi::CallbackInfo& info)
    {
        InvokeEngineCallback("error", info);
        InvokeCallback(info, Babylon::Polyfills::Console::LogLevel::Error);
    }

    void Console::InvokeCallback(const Napi::CallbackInfo& info, Babylon::Polyfills::Console::LogLevel logLevel)
    {
        std::stringstream ss{};
        for (unsigned int index = 0; index < info.Length(); index++)
        {
            if (index > 0)
            {
                ss << " ";
            }
            ss << info[index].ToString().Utf8Value().c_str();
        }
        ss << std::endl;
        s_callback(ss.str().c_str(), logLevel);
    }

    void Console::InvokeEngineCallback(const std::string functionName, const Napi::CallbackInfo& info)
    {
        if (s_engineConsole != nullptr)
        {
            const auto engineConsoleFunc = s_engineConsole->Value().Get(functionName).As<Napi::Function>();
            if (!engineConsoleFunc.IsUndefined()) {
                const auto args = GetCallbackInfoArgs(info);
                engineConsoleFunc.As<Napi::Function>().Call(s_engineConsole->Value(), args.size(), args.data());
            }
        }
    }
}

namespace Babylon::Polyfills::Console
{
    void Initialize(Napi::Env env, CallbackT callback)
    {
        Internal::Console::CreateInstance(env, std::move(callback));
    }
}
