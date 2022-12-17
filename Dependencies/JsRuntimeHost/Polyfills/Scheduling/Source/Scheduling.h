#pragma once

#include "TimeoutDispatcher.h"

#include <Babylon/JsRuntime.h>

#include <optional>

namespace Babylon::Polyfills::Internal
{
    class Scheduling : public Napi::ObjectWrap<Scheduling>
    {
    public:
        static void Initialize(Napi::Env env);

        Scheduling(const Napi::CallbackInfo& info);

    private:
        JsRuntime& m_runtime;
        std::optional<TimeoutDispatcher> m_timeoutDispatcher;

        static Napi::Value SetTimeout(const Napi::CallbackInfo& info);
        static void ClearTimeout(const Napi::CallbackInfo& info);
    };
}
