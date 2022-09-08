#pragma once

#include <Babylon/Polyfills/Console.h>

namespace Babylon::Polyfills::Internal
{
    class Console final : public Napi::ObjectWrap<Console>
    {
    public:
        using ParentT = Napi::ObjectWrap<Console>;

        static void CreateInstance(Napi::Env env, Babylon::Polyfills::Console::CallbackT callback);

        explicit Console(const Napi::CallbackInfo& info);

    private:
        static void Log(const Napi::CallbackInfo& info);
        static void Warn(const Napi::CallbackInfo& info);
        static void Error(const Napi::CallbackInfo& info);
        static void InvokeCallback(const Napi::CallbackInfo& info, Babylon::Polyfills::Console::LogLevel logLevel);
        static void InvokeEngineCallback(const std::string functionName, const Napi::CallbackInfo& info);

        static inline std::unique_ptr<Napi::ObjectReference> s_engineConsole{};
        static inline Babylon::Polyfills::Console::CallbackT s_callback{};
    };
}
