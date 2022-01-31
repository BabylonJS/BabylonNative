#include "Babylon/JsConsoleLogger.h"

namespace Babylon
{
    void LogMethod(Napi::Env env, const char* methodName, const char* message)
    {
        auto console = env.Global().Get("console");

        if (console.IsObject())
        {
            auto consoleLog{console.ToObject().Get(methodName)};

            if (consoleLog.IsFunction())
            {
                auto consoleLogFunction = consoleLog.As<Napi::Function>();
                auto messageStr = Napi::String::New(env, message);
                consoleLogFunction.Call(console, {messageStr});
            }
        }
    }

    void JsConsoleLogger::LogInfo(Napi::Env env, const char* message)
    {
        LogMethod(env, "log", message);
    }

    void JsConsoleLogger::LogWarn(Napi::Env env, const char* message)
    {
        LogMethod(env, "warn", message);
    }

    void JsConsoleLogger::LogError(Napi::Env env, const char* message) 
    {
        LogMethod(env, "error", message);
    }
}