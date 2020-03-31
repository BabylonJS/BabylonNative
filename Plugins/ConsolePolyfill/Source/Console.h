#pragma once

#include <Babylon/ConsolePolyfill.h>

namespace Babylon
{
    class Console final : public Napi::ObjectWrap<Console>
    {
    public:
        static inline constexpr char* JS_INSTANCE_NAME{"console"};

        using ParentT = Napi::ObjectWrap<Console>;

        static void CreateInstance(Napi::Env env, ConsolePolyfill::CallbackT callback);

        explicit Console(const Napi::CallbackInfo& info);

    private:
        void Log(const Napi::CallbackInfo& info);
        void Warn(const Napi::CallbackInfo& info);
        void Error(const Napi::CallbackInfo& info);
        void InvokeCallback(const Napi::CallbackInfo& info, ConsolePolyfill::LogLevel logLevel) const;

        ConsolePolyfill::CallbackT m_callback{};
    };
}
