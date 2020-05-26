#pragma once

#include <Babylon/Polyfills/Script.h>
#include <Babylon/ScriptLoader.h>
#include <arcana/threading/cancellation.h>
#include <Babylon/JsRuntimeScheduler.h>

namespace Babylon::Polyfills::Internal
{
    class Script final : public Napi::ObjectWrap<Script>
    {
    public:
        static inline constexpr const char* JS_INSTANCE_NAME{ "NativeScript" };
        using DispatchFunctionT = std::function<void(std::function<void(Napi::Env)>)>;

        using ParentT = Napi::ObjectWrap<Script>;

        static void CreateInstance(Napi::Env env, DispatchFunctionT dispatchFunction);

        explicit Script(const Napi::CallbackInfo& info);

    private:
        void LoadScript(const Napi::CallbackInfo& info);
        DispatchFunctionT m_dispatchFunction;
        Babylon::ScriptLoader m_loader;

        arcana::cancellation_source m_cancelSource{};
        JsRuntimeScheduler m_runtimeScheduler;
    };
}
