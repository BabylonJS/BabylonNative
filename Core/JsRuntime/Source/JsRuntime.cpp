#include "JsRuntime.h"

namespace Babylon
{
    namespace
    {
        static constexpr auto JS_RUNTIME_NAME = "runtime";
        static constexpr auto JS_WINDOW_NAME = "window";
    }

    JsRuntime::JsRuntime(DispatchFunctionT&& dispatchFunction)
        : m_dispatchFunction{ dispatchFunction }
    {
    }

    void JsRuntime::Initialize(Napi::Env env, DispatchFunctionT dispatchFunction)
    {
        JsRuntime* runtime = new JsRuntime(std::move(dispatchFunction));
        runtime->AddJavaScriptReference(env, true);
    }

    JsRuntime& JsRuntime::GetFromJavaScript(Napi::Env env)
    {
        return *env.Global()
            .Get(JS_NATIVE_NAME)
            .As<Napi::Object>()
            .Get(JS_RUNTIME_NAME)
            .As<Napi::External<JsRuntime>>()
            .Data();
    }

    void JsRuntime::Dispatch(std::function<void(Napi::Env)> function)
    {
        m_dispatchFunction(std::move(function));
    }

    void JsRuntime::AddJavaScriptReference(Napi::Env env, bool doesJavaScriptOwnJsRuntime)
    {
        auto global = env.Global();

        if (global.Get(JS_WINDOW_NAME).IsUndefined())
        {
            global.Set(JS_WINDOW_NAME, global);
        }

        auto jsNative = Napi::Object::New(env);
        global.Set(JS_NATIVE_NAME, jsNative);

        Napi::Value jsRuntime;
        if (doesJavaScriptOwnJsRuntime)
        {
            jsRuntime = Napi::External<JsRuntime>::New(env, this, [](Napi::Env, JsRuntime* jsRuntime) { delete jsRuntime; });
        }
        else
        {
            jsRuntime = Napi::External<JsRuntime>::New(env, this);
        }
        jsNative.Set(JS_RUNTIME_NAME, jsRuntime);
    }
}