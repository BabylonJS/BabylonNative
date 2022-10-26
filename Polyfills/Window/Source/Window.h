#pragma once

#include <Babylon/JsRuntimeScheduler.h>
#include "TimeoutDispatcher.h"
#include <optional>

namespace Babylon::Polyfills::Internal
{
    class Window : public Napi::ObjectWrap<Window>
    {
        static constexpr auto JS_WINDOW_NAME = "window";

    public:
        static void Initialize(Napi::Env env);
        static Window& GetFromJavaScript(Napi::Env);

        Window(const Napi::CallbackInfo& info);
        ~Window();

    private:
        static Napi::Value SetTimeout(const Napi::CallbackInfo& info);
        static void ClearTimeout(const Napi::CallbackInfo& info);
        static Napi::Value DecodeBase64(const Napi::CallbackInfo& info);
        static void AddEventListener(const Napi::CallbackInfo& info);
        static void RemoveEventListener(const Napi::CallbackInfo& info);
        static Napi::Value GetDevicePixelRatio(const Napi::CallbackInfo& info);

        arcana::cancellation_source m_cancelSource;
        JsRuntimeScheduler m_runtimeScheduler;
        TimeoutDispatcher m_timeoutDispatcher;
    };
}
