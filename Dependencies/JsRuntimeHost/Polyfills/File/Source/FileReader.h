#pragma once

#include <napi/napi.h>

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace Babylon::Polyfills::Internal
{
    class FileReader final : public Napi::ObjectWrap<FileReader>
    {
    public:
        static constexpr int32_t EMPTY = 0;
        static constexpr int32_t LOADING = 1;
        static constexpr int32_t DONE = 2;

        static void Initialize(Napi::Env env);

        explicit FileReader(const Napi::CallbackInfo& info);

    private:
        enum class ReadMode
        {
            ArrayBuffer,
            Text,
            DataUrl,
        };

        void ReadAsArrayBuffer(const Napi::CallbackInfo& info);
        void ReadAsText(const Napi::CallbackInfo& info);
        void ReadAsDataURL(const Napi::CallbackInfo& info);
        void Abort(const Napi::CallbackInfo& info);
        void AddEventListener(const Napi::CallbackInfo& info);
        void RemoveEventListener(const Napi::CallbackInfo& info);
        Napi::Value DispatchEvent(const Napi::CallbackInfo& info);

        // readonly attributes (WHATWG IDL): prototype getters reading C++ state,
        // so JS can neither overwrite them nor fool the state-machine checks.
        Napi::Value GetReadyState(const Napi::CallbackInfo& info);
        Napi::Value GetResult(const Napi::CallbackInfo& info);
        Napi::Value GetError(const Napi::CallbackInfo& info);

        // EventHandler IDL attributes (onload, onerror, ...): prototype
        // accessor pairs backed by Napi::FunctionReference slots. The accessor
        // `data` carries the event type (without the "on" prefix) so a single
        // get/set pair services every slot.
        Napi::Value GetOnHandler(const Napi::CallbackInfo& info);
        void SetOnHandler(const Napi::CallbackInfo& info, const Napi::Value& value);

        void StartRead(const Napi::CallbackInfo& info, ReadMode mode);
        void HandleReadResult(uint64_t myReadId, ReadMode mode, const std::string& contentType,
                              Napi::Object jsThis, const Napi::Value& bufValue);
        void HandleReadError(uint64_t myReadId, Napi::Object jsThis, const Napi::Value& error);
        void Dispatch(Napi::Env env, const Napi::Object& jsThis, const std::string& eventType);

        void StoreResult(const Napi::Value& value);
        void StoreError(const Napi::Value& value);

        // Monotonic read id. StartRead bumps it to mint a fresh id; Abort bumps
        // it to invalidate the in-flight read's queued continuation so a
        // promise that settles after an abort-then-restart cannot dispatch a
        // phantom "load" against the new read's state. The in-flight read's
        // wrapper is kept alive by an externally-held anchor (see StartRead),
        // so `this` is always valid when a continuation reads this field.
        uint64_t m_readId{0};
        std::unordered_map<std::string, std::vector<Napi::FunctionReference>> m_eventHandlerRefs;

        // readonly attribute state, surfaced through the getters above.
        int32_t m_readyState{EMPTY};

        // result/error live as properties on this persistent holder object
        // rather than in Napi::Reference<Value> slots: napi_create_reference
        // only accepts heap values (object/function/symbol) on the real N-API
        // backends (V8/JSC), so referencing a primitive string result (from
        // readAsText/readAsDataURL) throws there. Boxing inside a held object
        // keeps the state C++-owned and tamper-proof while remaining valid for
        // every value type.
        Napi::ObjectReference m_state;

        // on* EventHandler slots, keyed by event type ("load", "error", ...).
        std::unordered_map<std::string, Napi::FunctionReference> m_onHandlers;
    };
}
