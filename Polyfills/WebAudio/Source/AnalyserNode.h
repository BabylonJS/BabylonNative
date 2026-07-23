#pragma once

#include "AudioNode.h"

#include <napi/napi.h>

namespace Babylon::Polyfills::Internal
{
    class NativeAnalyserNode final : public Napi::ObjectWrap<NativeAnalyserNode>
    {
    public:
        static void Initialize(Napi::Env env);
        static Napi::Object CreateInstance(Napi::Env env, const std::shared_ptr<AudioContextState>& context);

        explicit NativeAnalyserNode(const Napi::CallbackInfo& info);

    private:
        static constexpr auto JS_CONSTRUCTOR_NAME = "AnalyserNode";

        void InitializeState(const std::shared_ptr<AudioContextState>& context);

        Napi::Value Connect(const Napi::CallbackInfo& info);
        Napi::Value Disconnect(const Napi::CallbackInfo& info);
        void GetByteFrequencyData(const Napi::CallbackInfo& info);
        void GetFloatFrequencyData(const Napi::CallbackInfo& info);
        void GetByteTimeDomainData(const Napi::CallbackInfo& info);
        void GetFloatTimeDomainData(const Napi::CallbackInfo& info);

        Napi::Value GetFftSize(const Napi::CallbackInfo& info);
        void SetFftSize(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetFrequencyBinCount(const Napi::CallbackInfo& info);
        Napi::Value GetMinDecibels(const Napi::CallbackInfo& info);
        void SetMinDecibels(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetMaxDecibels(const Napi::CallbackInfo& info);
        void SetMaxDecibels(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetSmoothingTimeConstant(const Napi::CallbackInfo& info);
        void SetSmoothingTimeConstant(const Napi::CallbackInfo& info, const Napi::Value& value);

        std::shared_ptr<AnalyserNodeState> m_state{};
    };
}
