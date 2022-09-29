#pragma once

#include <Babylon/JsRuntimeScheduler.h>

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
        static void SetTimeout(const Napi::CallbackInfo& info);
        static Napi::Value DecodeBase64(const Napi::CallbackInfo& info);
        static void AddEventListener(const Napi::CallbackInfo& info);
        static void RemoveEventListener(const Napi::CallbackInfo& info);
        static Napi::Value GetDevicePixelRatio(const Napi::CallbackInfo& info);

        void RecursiveWaitOrCall(Napi::Reference<Napi::Value> thisRef, Napi::FunctionReference function, std::chrono::system_clock::time_point whenToRun);

        arcana::cancellation_source m_cancelSource;

        // Put this last so that it gets destructed first.
        JsRuntimeScheduler m_runtimeScheduler;
    };
}
