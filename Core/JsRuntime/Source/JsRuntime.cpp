#include "JsRuntime.h"

namespace Babylon
{
    namespace
    {
        static constexpr auto JS_RUNTIME_NAME = "runtime";
        static constexpr auto JS_WINDOW_NAME = "window";
    }

    JsRuntime::JsRuntime(Napi::Env env, DispatchFunctionT dispatchFunction)
        : m_dispatchFunction{std::move(dispatchFunction)}
    {
        auto global = env.Global();

        if (global.Get(JS_WINDOW_NAME).IsUndefined())
        {
            global.Set(JS_WINDOW_NAME, global);
        }

        auto jsNative = Napi::Object::New(env);
        env.Global().Set(NativeObject::JS_NATIVE_NAME, jsNative);

        Napi::Value jsRuntime = Napi::External<JsRuntime>::New(env, this, [](Napi::Env, JsRuntime* runtime) { delete runtime; });
        jsNative.Set(JS_RUNTIME_NAME, jsRuntime);
    }

    JsRuntime& JsRuntime::CreateForJavaScript(Napi::Env env, DispatchFunctionT dispatchFunction)
    {
        auto* runtime = new JsRuntime(env, std::move(dispatchFunction));
        return *runtime;
    }

    JsRuntime& JsRuntime::GetFromJavaScript(Napi::Env env)
    {
        return *NativeObject::GetFromJavaScript(env)
                    .As<Napi::Object>()
                    .Get(JS_RUNTIME_NAME)
                    .As<Napi::External<JsRuntime>>()
                    .Data();
    }

    void JsRuntime::Dispatch(std::function<void(Napi::Env)> function)
    {
        std::scoped_lock lock{m_mutex};
        m_dispatchFunction([function = std::move(function)](Napi::Env env) {
            function(env);

            // The environment will be in a pending exceptional state if
            // Napi::Error::ThrowAsJavaScriptException is invoked within the
            // previous function. Throw and clear the pending exception here to
            // bubble up the exception to the the dispatcher.
            if (env.IsExceptionPending())
            {
                throw env.GetAndClearPendingException();
            }
        });
    }
}
