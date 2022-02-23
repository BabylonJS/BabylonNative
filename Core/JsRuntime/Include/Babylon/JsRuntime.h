#pragma once

#include <napi/env.h>

#include <functional>
#include <mutex>

namespace Babylon
{
    class JsRuntime
    {
    public:
        class NativeObject
        {
            friend class JsRuntime;
            static constexpr auto JS_NATIVE_NAME = "_native";
        
        public:
            static Napi::Object GetFromJavaScript(Napi::Env env)
            {
                return env.Global().Get(JS_NATIVE_NAME).As<Napi::Object>();
            }
        };

        struct InternalState;
        friend struct InternalState;

        using DispatchFunctionT = std::function<void(std::function<void(Napi::Env)>)>;

        // Note: It is the contract of JsRuntime that its dispatch function must be usable
        // at the moment of construction. JsRuntime cannot be built with dispatch function
        // that captures a reference to a not-yet-completed object that will be completed
        // later -- an instance of an inheriting type, for example. The dispatch function
        // must be safely callable as soon as it is passed to the JsRuntime constructor.
        static JsRuntime& CreateForJavaScript(Napi::Env, DispatchFunctionT, std::function<void(const Napi::Error&)>);
        static JsRuntime& GetFromJavaScript(Napi::Env);
        void Dispatch(std::function<void(Napi::Env)>);

    protected:
        JsRuntime(const JsRuntime&) = delete;
        JsRuntime(JsRuntime&&) = delete;

    private:
        JsRuntime(Napi::Env, DispatchFunctionT, std::function<void(const Napi::Error&)> unhandledExceptionHandler);

        DispatchFunctionT m_dispatchFunction{};
        std::function<void(const Napi::Error&)> m_unhandledExceptionHandler{};
        std::mutex m_mutex{};
    };
}
