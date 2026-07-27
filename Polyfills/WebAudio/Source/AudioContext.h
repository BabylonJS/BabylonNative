#pragma once

#include "AudioNode.h"
#include "AudioEngine.h"

#include <napi/napi.h>

namespace Babylon::Polyfills::Internal
{
    class NativeAudioContext final : public Napi::ObjectWrap<NativeAudioContext>
    {
    public:
        static void Initialize(Napi::Env env);

        explicit NativeAudioContext(const Napi::CallbackInfo& info);
        ~NativeAudioContext() override;

        std::shared_ptr<AudioContextState> State() const;

    private:
        static constexpr auto JS_CONSTRUCTOR_NAME = "AudioContext";

        void InitializeContext();

        Napi::Value GetDestination(const Napi::CallbackInfo& info);
        Napi::Value GetListener(const Napi::CallbackInfo& info);
        Napi::Value GetState(const Napi::CallbackInfo& info);
        Napi::Value GetSampleRate(const Napi::CallbackInfo& info);
        Napi::Value GetCurrentTime(const Napi::CallbackInfo& info);
        Napi::Value Resume(const Napi::CallbackInfo& info);
        Napi::Value Suspend(const Napi::CallbackInfo& info);
        Napi::Value Close(const Napi::CallbackInfo& info);
        Napi::Value DecodeAudioData(const Napi::CallbackInfo& info);
        Napi::Value CreateGain(const Napi::CallbackInfo& info);
        Napi::Value CreateBufferSource(const Napi::CallbackInfo& info);
        Napi::Value CreatePanner(const Napi::CallbackInfo& info);
        Napi::Value CreateStereoPanner(const Napi::CallbackInfo& info);
        Napi::Value CreateAnalyser(const Napi::CallbackInfo& info);
        Napi::Value CreateBuffer(const Napi::CallbackInfo& info);
        void AddEventListener(const Napi::CallbackInfo& info);
        void RemoveEventListener(const Napi::CallbackInfo& info);

        void SetState(const std::string& state);

        std::shared_ptr<AudioContextState> m_state{};
        Napi::ObjectReference m_destination{};
        Napi::ObjectReference m_listener{};
        EventTargetSupport m_eventTarget{};
    };
}
