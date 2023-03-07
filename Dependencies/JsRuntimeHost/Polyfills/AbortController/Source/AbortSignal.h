#pragma once

#include <Babylon/JsRuntimeScheduler.h>

#include <napi/napi.h>

#include <unordered_map>

class AbortController;

namespace Babylon::Polyfills::Internal
{
    class AbortSignal final : public Napi::ObjectWrap<AbortSignal>
    {
    public:

        static constexpr auto JS_ABORT_SIGNAL_CONSTRUCTOR_NAME = "AbortSignal";

        static void Initialize(Napi::Env env);

        explicit AbortSignal(const Napi::CallbackInfo& info);

    private:

        Napi::Value GetAborted(const Napi::CallbackInfo& info);
        void SetAborted(const Napi::CallbackInfo&, const Napi::Value& value);

        Napi::Value GetOnAbort(const Napi::CallbackInfo& info);
        void SetOnAbort(const Napi::CallbackInfo&, const Napi::Value& value);

        void AddEventListener(const Napi::CallbackInfo& info);
        void RemoveEventListener(const Napi::CallbackInfo& info);
        std::unordered_map<std::string, std::vector<Napi::FunctionReference>> m_eventHandlerRefs;

        Napi::FunctionReference m_onabort;
        bool m_aborted = false;
    };
}