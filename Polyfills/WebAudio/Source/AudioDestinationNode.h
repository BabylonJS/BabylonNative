#pragma once

#include "AudioNode.h"

#include <napi/napi.h>

namespace Babylon::Polyfills::Internal
{
    class NativeAudioDestinationNode final : public Napi::ObjectWrap<NativeAudioDestinationNode>
    {
    public:
        static void Initialize(Napi::Env env);
        static Napi::Object CreateInstance(Napi::Env env, const std::shared_ptr<DestinationNodeState>& state);

        explicit NativeAudioDestinationNode(const Napi::CallbackInfo& info);

        std::shared_ptr<AudioNodeState> State() const;

    private:
        static constexpr auto JS_CONSTRUCTOR_NAME = "AudioDestinationNode";

        Napi::Value Connect(const Napi::CallbackInfo& info);
        Napi::Value Disconnect(const Napi::CallbackInfo& info);
        Napi::Value GetMaxChannelCount(const Napi::CallbackInfo& info);

        std::shared_ptr<DestinationNodeState> m_state{};
    };
}
