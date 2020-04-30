#include "AppLifeCycle.h"

#include <Babylon/JsRuntime.h>

namespace Babylon
{
    namespace
    {
        static constexpr auto JS_APP_LIFE_CYCLE_NAME = "appLifeCycle";
    }

    AppLifeCycle::AppLifeCycle(Napi::Env env)
    {
        Napi::Value jsAppLifeCycle = Napi::External<AppLifeCycle>::New(env, this, [](Napi::Env, AppLifeCycle* appLifeCycle) { delete appLifeCycle; });

        auto jsNative = env.Global().Get(Babylon::JsRuntime::JS_NATIVE_NAME).As<Napi::Object>();
        jsNative.Set(JS_APP_LIFE_CYCLE_NAME, jsAppLifeCycle);
    }

    AppLifeCycle& AppLifeCycle::CreateForJavaScript(Napi::Env env)
    {
        auto* appLifeCycle = new AppLifeCycle(env);
        return *appLifeCycle;
    }

    AppLifeCycle& AppLifeCycle::GetFromJavaScript(Napi::Env env)
    {
        return *env.Global()
                    .Get(Babylon::JsRuntime::JS_NATIVE_NAME)
                    .As<Napi::Object>()
                    .Get(JS_APP_LIFE_CYCLE_NAME)
                    .As<Napi::External<AppLifeCycle>>()
                    .Data();
    }

    AppLifeCycle::TicketT AppLifeCycle::AddOnSuspendCallback(CallbackT callback)
    {
        return m_suspendCallbacks.insert(std::move(callback));
    }

    AppLifeCycle::TicketT AppLifeCycle::AddOnResumeCallback(CallbackT callback)
    {
        return m_resumeCallbacks.insert(std::move(callback));
    }

    void AppLifeCycle::Suspend()
    {
        m_suspendCallbacks.apply_to_all([](CallbackT& callback) {
            callback();
        });
    }

    void AppLifeCycle::Resume()
    {
        m_resumeCallbacks.apply_to_all([](CallbackT& callback) {
            callback();
        });
    }
}