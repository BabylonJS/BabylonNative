#pragma once

#include <napi/env.h>

#include <functional>
#include <mutex>

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
        JsRuntime(DispatchFunctionT&&);
        JsRuntime(const JsRuntime&) = delete;

        void AddJavaScriptReference(Napi::Env env, bool doesJavaScriptOwnJsRuntime);

    private:
        DispatchFunctionT m_dispatchFunction{};
        std::mutex m_mutex{};
    };
}