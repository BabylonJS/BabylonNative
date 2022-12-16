#include "Window.h"

namespace Babylon::Polyfills::Internal
{
    namespace
    {
        constexpr auto JS_CLASS_NAME = "Window";
        constexpr auto JS_SET_TIMEOUT_NAME = "setTimeout";
        constexpr auto JS_CLEAR_TIMEOUT_NAME = "clearTimeout";
    }

    void Window::Initialize(Napi::Env env)
    {
        Napi::HandleScope scope{env};

        Napi::Function constructor = DefineClass(
            env,
            JS_CLASS_NAME,
            {});

        auto global = env.Global();
        auto jsNative = JsRuntime::NativeObject::GetFromJavaScript(env);
        auto jsWindow = constructor.New({});

        jsNative.Set(JS_WINDOW_NAME, jsWindow);

        if (global.Get(JS_SET_TIMEOUT_NAME).IsUndefined() && global.Get(JS_CLEAR_TIMEOUT_NAME).IsUndefined())
        {
            global.Set(JS_SET_TIMEOUT_NAME, Napi::Function::New(env, &Window::SetTimeout, JS_SET_TIMEOUT_NAME, Window::Unwrap(jsWindow)));
            global.Set(JS_CLEAR_TIMEOUT_NAME, Napi::Function::New(env, &Window::ClearTimeout, JS_CLEAR_TIMEOUT_NAME, Window::Unwrap(jsWindow)));
        }
    }

    Window& Window::GetFromJavaScript(Napi::Env env)
    {
        return *Window::Unwrap(JsRuntime::NativeObject::GetFromJavaScript(env).Get(JS_WINDOW_NAME).As<Napi::Object>());
    }

    Window::Window(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<Window>{info}
        , m_runtime{JsRuntime::GetFromJavaScript(info.Env())}
        , m_timeoutDispatcher{m_runtime}
    {
    }

    Napi::Value Window::SetTimeout(const Napi::CallbackInfo& info)
    {
        auto& window = *static_cast<Window*>(info.Data());
        auto function = info[0].IsFunction()
            ? std::make_shared<Napi::FunctionReference>(Napi::Persistent(info[0].As<Napi::Function>()))
            : std::shared_ptr<Napi::FunctionReference>{};

        auto delay = std::chrono::milliseconds{info[1].ToNumber().Int32Value()};

        return Napi::Value::From(info.Env(), window.m_timeoutDispatcher->Dispatch(function, delay));
    }

    void Window::ClearTimeout(const Napi::CallbackInfo& info)
    {
        const auto arg = info[0];
        if (arg.IsNumber())
        {
            auto timeoutId = arg.As<Napi::Number>().Int32Value();
            auto& window = *static_cast<Window*>(info.Data());
            window.m_timeoutDispatcher->Clear(timeoutId);
        }
    }
}

namespace Babylon::Polyfills::Window
{
    void Initialize(Napi::Env env)
    {
        Internal::Window::Initialize(env);
    }
}
