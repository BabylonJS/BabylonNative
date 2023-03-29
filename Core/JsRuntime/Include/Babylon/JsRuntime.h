#pragma once

#include <napi/env.h>

#include <functional>
#include <mutex>
#include <vector>

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

        JsRuntime(const JsRuntime&) = delete;
        JsRuntime& operator=(const JsRuntime&) = delete;

        // Any JavaScript errors that occur will bubble up as a Napi::Error C++ exception.
        // JsRuntime expects the provided dispatch function to handle this exception,
        // such as with a try/catch and logging the exception message.
        using DispatchFunctionT = std::function<void(std::function<void(Napi::Env)>)>;

        // Creates the JsRuntime object owned by the JavaScript environment.
        // Note: It is the contract of JsRuntime that its dispatch function must be usable
        // at the moment of construction. JsRuntime cannot be built with dispatch function
        // that captures a reference to a not-yet-completed object that will be completed
        // later -- an instance of an inheriting type, for example. The dispatch function
        // must be safely callable as soon as it is passed to the JsRuntime constructor.
        static JsRuntime& CreateForJavaScript(Napi::Env, DispatchFunctionT);

        // Gets the JsRuntime from the given N-API environment.
        static JsRuntime& GetFromJavaScript(Napi::Env);

        // Notifies the JsRuntime that the JavaScript environment will begin shutting down.
        // Calling this function will signal callbacks registered with RegisterDisposing.
        static void NotifyDisposing(JsRuntime&);

        // Registers a callback for when the JavaScript environment will begin shutting down.
        static void RegisterDisposing(JsRuntime&, std::function<void()>);

        // Dispatches work onto the JavaScript thread and provides access to the N-API
        // environment.
        void Dispatch(std::function<void(Napi::Env)>);

    private:
        JsRuntime(Napi::Env, DispatchFunctionT);

        std::mutex m_mutex{};
        DispatchFunctionT m_dispatchFunction{};
        std::vector<std::function<void()>> m_disposingCallbacks{};
    };
}
