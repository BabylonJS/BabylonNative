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

        // Transition the signal to the aborted state with the given reason (undefined -> a default
        // "AbortError"), firing onabort and any "abort" listeners. No-op if already aborted.
        void Abort(const Napi::Value& reason);

        // Build the default abort reason: an Error whose name is "AbortError" (there is no
        // DOMException polyfill), matching what the platform uses when abort() is called with no
        // reason and what fetch() rejects with on abort.
        static Napi::Value CreateAbortError(Napi::Env env, const char* message);

    private:
        Napi::Value GetAborted(const Napi::CallbackInfo& info);

        Napi::Value GetReason(const Napi::CallbackInfo& info);
        void ThrowIfAborted(const Napi::CallbackInfo& info);

        // AbortSignal.abort(reason?) -- returns an AbortSignal already in the aborted state.
        static Napi::Value AbortStatic(const Napi::CallbackInfo& info);

        Napi::Value GetOnAbort(const Napi::CallbackInfo& info);
        void SetOnAbort(const Napi::CallbackInfo&, const Napi::Value& value);

        void AddEventListener(const Napi::CallbackInfo& info);
        void RemoveEventListener(const Napi::CallbackInfo& info);
        void RaiseEvent(const char* eventType);

        std::unordered_map<std::string, std::vector<Napi::FunctionReference>> m_eventHandlerRefs;

        Napi::FunctionReference m_onabort;
        Napi::Reference<Napi::Value> m_reason;
        bool m_aborted = false;
    };
}