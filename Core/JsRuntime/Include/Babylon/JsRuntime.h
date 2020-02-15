#pragma once

#include <napi/env.h>

#include <functional>

namespace Babylon
{
    class JsRuntime
    {
    public:
        static constexpr auto JS_NATIVE_NAME = "_native";
        using DispatchFunctionT = std::function<void(std::function<void(Napi::Env)>)>;

        static void Initialize(Napi::Env, DispatchFunctionT);
        static JsRuntime& GetFromJavaScript(Napi::Env);
        void Dispatch(std::function<void(Napi::Env)>);

    protected:
        void AddJavaScriptReference(Napi::Env env, bool doesJavaScriptOwnJsRuntime);
        JsRuntime(DispatchFunctionT&&);

    private:
        DispatchFunctionT m_dispatchFunction{};
    };
}