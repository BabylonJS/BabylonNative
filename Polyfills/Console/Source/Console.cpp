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
    void Console::CreateInstance(Napi::Env env, Babylon::Polyfills::Console::CallbackT callback)
    {
        Napi::HandleScope scope{env};

        Napi::Function func = ParentT::DefineClass(
            env,
            "Console",
            {
                ParentT::InstanceMethod("log", &Console::Log),
                ParentT::InstanceMethod("warn", &Console::Warn),
                ParentT::InstanceMethod("error", &Console::Error),
            });

        auto console = func.New({});
        Console::Unwrap(console)->m_callback = std::move(callback);
        const auto existingConsole = env.Global().Get(JS_INSTANCE_NAME);
        if (!existingConsole.IsUndefined())
        {
            Console::Unwrap(console)->m_engineConsole = std::make_unique<Napi::ObjectReference>(Napi::Persistent(existingConsole.As<Napi::Object>()));
        }
        env.Global().Set(JS_INSTANCE_NAME, console);
    }

    Console::Console(const Napi::CallbackInfo& info)
        : ParentT{info}
        , m_callback{}
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

    void Console::InvokeCallback(const Napi::CallbackInfo& info, Babylon::Polyfills::Console::LogLevel logLevel) const
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
        m_callback(ss.str().c_str(), logLevel);
    }

    void Console::InvokeEngineCallback(const std::string functionName, const Napi::CallbackInfo& info)
    {
        if (m_engineConsole != nullptr)
        {
            const auto engineConsoleFunc = m_engineConsole->Value().Get(functionName).As<Napi::Function>();
            if (!engineConsoleFunc.IsUndefined()) {
                const auto args = GetCallbackInfoArgs(info);
                engineConsoleFunc.As<Napi::Function>().Call(m_engineConsole->Value(), args.size(), args.data());
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
