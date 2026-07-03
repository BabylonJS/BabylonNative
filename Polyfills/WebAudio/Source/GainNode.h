#pragma once

#include "AudioNode.h"

#include <napi/napi.h>

namespace Babylon::Polyfills::Internal
{
    class NativeGainNode final : public Napi::ObjectWrap<NativeGainNode>
    {
    public:
        static void Initialize(Napi::Env env);
        static Napi::Object CreateInstance(Napi::Env env, const std::shared_ptr<AudioContextState>& context);

        explicit NativeGainNode(const Napi::CallbackInfo& info);

    private:
        static constexpr auto JS_CONSTRUCTOR_NAME = "GainNode";

        void InitializeState(const std::shared_ptr<AudioContextState>& context);

        Napi::Value Connect(const Napi::CallbackInfo& info);
        Napi::Value Disconnect(const Napi::CallbackInfo& info);
        Napi::Value GetGain(const Napi::CallbackInfo& info);

        std::shared_ptr<GainNodeState> m_state{};
        Napi::ObjectReference m_gain{};
    };
}
