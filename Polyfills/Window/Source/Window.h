#pragma once

#include "TimeoutDispatcher.h"

#include <Babylon/JsRuntime.h>

#include <optional>

namespace Babylon::Polyfills::Internal
{
    using TimeoutId = int32_t;

    class Window : public Napi::ObjectWrap<Window>
    {
        static constexpr auto JS_WINDOW_NAME = "window";

    public:
        static void Initialize(Napi::Env env);
        static Window& GetFromJavaScript(Napi::Env);

        Window(const Napi::CallbackInfo& info);
    private:
        JsRuntime& m_runtime;
        std::optional<TimeoutDispatcher> m_timeoutDispatcher;

        static Napi::Value SetTimeout(const Napi::CallbackInfo& info);
        static void ClearTimeout(const Napi::CallbackInfo& info);
        static Napi::Value DecodeBase64(const Napi::CallbackInfo& info);
        static void AddEventListener(const Napi::CallbackInfo& info);
        static void RemoveEventListener(const Napi::CallbackInfo& info);
        static Napi::Value GetDevicePixelRatio(const Napi::CallbackInfo& info);
    };
}
