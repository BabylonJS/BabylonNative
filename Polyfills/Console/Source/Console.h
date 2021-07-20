#pragma once

#include <Babylon/Polyfills/Console.h>

namespace Babylon::Polyfills::Internal
{
    class Console final : public Napi::ObjectWrap<Console>
    {
    public:
        static inline constexpr const char* JS_INSTANCE_NAME{"console"};

        using ParentT = Napi::ObjectWrap<Console>;

        static void CreateInstance(Napi::Env env, Babylon::Polyfills::Console::CallbackT callback);

        explicit Console(const Napi::CallbackInfo& info);

    private:
        void Log(const Napi::CallbackInfo& info);
        void Warn(const Napi::CallbackInfo& info);
        void Error(const Napi::CallbackInfo& info);
        void InvokeCallback(const Napi::CallbackInfo& info, Babylon::Polyfills::Console::LogLevel logLevel) const;
        void InvokeEngineCallback(const std::string functionName, const Napi::CallbackInfo& info);

        std::unique_ptr<Napi::ObjectReference> m_engineConsole{};
        Babylon::Polyfills::Console::CallbackT m_callback{};
    };
}
