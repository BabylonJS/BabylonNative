#pragma once

#include <napi/env.h>

#include <functional>

namespace Babylon
{
    class JsRuntime
    {
        static constexpr auto JS_RUNTIME_NAME = "runtime";
        static constexpr auto JS_WINDOW_NAME = "window";

    public:
        static constexpr auto JS_NATIVE_NAME = "_native";
        using DispatchFunctionT = std::function<void(std::function<void(Napi::Env)>)>;

        static void Initialize(Napi::Env env, DispatchFunctionT dispatchFunction)
        {
            JsRuntime* runtime = new JsRuntime(std::move(dispatchFunction));
            runtime->AddJavaScriptReference(env, true);
        }

        static JsRuntime& GetFromJavaScript(Napi::Env env)
        {
            return *env.Global()
                .Get(JS_NATIVE_NAME)
                .As<Napi::Object>()
                .Get(JS_RUNTIME_NAME)
                .As<Napi::External<JsRuntime>>()
                .Data();
        }

        void Dispatch(std::function<void(Napi::Env)> function)
        {
            m_dispatchFunction(std::move(function));
        }

    protected:
        void AddJavaScriptReference(Napi::Env env, bool doesJavaScriptOwnJsRuntime)
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

        JsRuntime(DispatchFunctionT&& dispatchFunction)
            : m_dispatchFunction{dispatchFunction}
        {
        }

    private:
        DispatchFunctionT m_dispatchFunction{};
    };
}