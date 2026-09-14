#pragma once

#include "AudioNode.h"

#include <napi/napi.h>

namespace Babylon::Polyfills::Internal
{
    class NativeAudioParam final : public Napi::ObjectWrap<NativeAudioParam>
    {
    public:
        static void Initialize(Napi::Env env);
        static Napi::Object CreateInstance(
            Napi::Env env,
            const std::shared_ptr<AudioParamState>& state,
            float defaultValue,
            const std::function<double()>& getCurrentTime);

        explicit NativeAudioParam(const Napi::CallbackInfo& info);

        std::shared_ptr<AudioParamState> State() const;

    private:
        static constexpr auto JS_CONSTRUCTOR_NAME = "AudioParam";

        double CurrentTime() const;

        Napi::Value GetValue(const Napi::CallbackInfo& info);
        void SetValue(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetDefaultValue(const Napi::CallbackInfo& info);
        Napi::Value SetValueAtTime(const Napi::CallbackInfo& info);
        Napi::Value LinearRampToValueAtTime(const Napi::CallbackInfo& info);
        Napi::Value ExponentialRampToValueAtTime(const Napi::CallbackInfo& info);
        Napi::Value SetValueCurveAtTime(const Napi::CallbackInfo& info);
        Napi::Value CancelScheduledValues(const Napi::CallbackInfo& info);

        void SetImmediate(float value);

        std::shared_ptr<AudioParamState> m_state{};
        float m_defaultValue{0.f};
        std::function<double()> m_getCurrentTime{};
    };
}
