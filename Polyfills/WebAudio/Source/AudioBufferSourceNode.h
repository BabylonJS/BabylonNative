#pragma once

#include "AudioNode.h"

#include <napi/napi.h>

namespace Babylon::Polyfills::Internal
{
    class NativeAudioBufferSourceNode final : public Napi::ObjectWrap<NativeAudioBufferSourceNode>
    {
    public:
        static void Initialize(Napi::Env env);
        static Napi::Object CreateInstance(Napi::Env env, const std::shared_ptr<AudioContextState>& context);

        explicit NativeAudioBufferSourceNode(const Napi::CallbackInfo& info);
        ~NativeAudioBufferSourceNode() override;

    private:
        static constexpr auto JS_CONSTRUCTOR_NAME = "AudioBufferSourceNode";

        void InitializeState(const std::shared_ptr<AudioContextState>& context, const Napi::Value& options);

        Napi::Value Connect(const Napi::CallbackInfo& info);
        Napi::Value Disconnect(const Napi::CallbackInfo& info);
        Napi::Value GetBuffer(const Napi::CallbackInfo& info);
        void SetBuffer(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetLoop(const Napi::CallbackInfo& info);
        void SetLoop(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetLoopStart(const Napi::CallbackInfo& info);
        void SetLoopStart(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetLoopEnd(const Napi::CallbackInfo& info);
        void SetLoopEnd(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetPlaybackRate(const Napi::CallbackInfo& info);
        Napi::Value GetDetune(const Napi::CallbackInfo& info);
        Napi::Value GetOnEnded(const Napi::CallbackInfo& info);
        void SetOnEnded(const Napi::CallbackInfo& info, const Napi::Value& value);
        void Start(const Napi::CallbackInfo& info);
        void Stop(const Napi::CallbackInfo& info);
        void AddEventListener(const Napi::CallbackInfo& info);
        void RemoveEventListener(const Napi::CallbackInfo& info);

        void FireEnded();

        struct EndedDispatcher;

        std::shared_ptr<BufferSourceState> m_state{};
        std::shared_ptr<EndedDispatcher> m_endedDispatcher{};
        Napi::ObjectReference m_buffer{};
        Napi::ObjectReference m_playbackRate{};
        Napi::ObjectReference m_detune{};
        Napi::FunctionReference m_onended{};
        EventTargetSupport m_eventTarget{};
    };
}
