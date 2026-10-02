#include "Scheduling.h"

namespace
{
    constexpr auto JS_SET_TIMEOUT_NAME = "setTimeout";
    constexpr auto JS_CLEAR_TIMEOUT_NAME = "clearTimeout";
    constexpr auto JS_SET_INTERVAL_NAME = "setInterval";
    constexpr auto JS_CLEAR_INTERVAL_NAME = "clearInterval";

    Napi::Value SetTimeout(const Napi::CallbackInfo& info, Babylon::Polyfills::Internal::TimeoutDispatcher& timeoutDispatcher, bool repeat)
    {
        auto function =
            info[0].IsFunction()
                ? std::make_shared<Napi::FunctionReference>(Napi::Persistent(info[0].As<Napi::Function>()))
                : std::shared_ptr<Napi::FunctionReference>{};

        auto delay = std::chrono::milliseconds{info[1].ToNumber().Int32Value()};

        return Napi::Value::From(info.Env(), timeoutDispatcher.Dispatch(function, delay, repeat));
    }

    void ClearTimeout(const Napi::CallbackInfo& info, Babylon::Polyfills::Internal::TimeoutDispatcher& timeoutDispatcher)
    {
        const auto arg = info[0];
        if (arg.IsNumber())
        {
            auto timeoutId = arg.As<Napi::Number>().Int32Value();
            timeoutDispatcher.Clear(timeoutId);
        }
    }
}

namespace Babylon::Polyfills::Scheduling
{
    void BABYLON_API Initialize(Napi::Env env)
    {
        auto global = env.Global();
        auto timeoutDispatcher = std::make_shared<Internal::TimeoutDispatcher>(JsRuntime::GetFromJavaScript(env));

        if (global.Get(JS_SET_TIMEOUT_NAME).IsUndefined() && global.Get(JS_CLEAR_TIMEOUT_NAME).IsUndefined())
        {
            global.Set(JS_SET_TIMEOUT_NAME,
                Napi::Function::New(
                    env, [timeoutDispatcher](const Napi::CallbackInfo& info) {
                        return SetTimeout(info, *timeoutDispatcher, false);
                    },
                    JS_SET_TIMEOUT_NAME));

            global.Set(JS_CLEAR_TIMEOUT_NAME,
                Napi::Function::New(
                    env, [timeoutDispatcher](const Napi::CallbackInfo& info) {
                        ClearTimeout(info, *timeoutDispatcher);
                    },
                    JS_CLEAR_TIMEOUT_NAME));
        }

        if (global.Get(JS_SET_INTERVAL_NAME).IsUndefined() && global.Get(JS_CLEAR_INTERVAL_NAME).IsUndefined())
        {
            global.Set(JS_SET_INTERVAL_NAME,
                Napi::Function::New(
                    env, [timeoutDispatcher](const Napi::CallbackInfo& info) {
                        return SetTimeout(info, *timeoutDispatcher, true);
                    },
                    JS_SET_INTERVAL_NAME));

            global.Set(JS_CLEAR_INTERVAL_NAME,
                Napi::Function::New(
                    env, [timeoutDispatcher](const Napi::CallbackInfo& info) {
                        ClearTimeout(info, *timeoutDispatcher);
                    },
                    JS_CLEAR_INTERVAL_NAME));
        }
    }
}
