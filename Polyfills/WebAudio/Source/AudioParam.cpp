#include "AudioParam.h"

#include <Babylon/JsRuntime.h>

#include <vector>

namespace Babylon::Polyfills::Internal
{
    void NativeAudioParam::Initialize(Napi::Env env)
    {
        Napi::HandleScope scope{env};

        Napi::Function func = DefineClass(
            env,
            JS_CONSTRUCTOR_NAME,
            {
                InstanceAccessor("value", &NativeAudioParam::GetValue, &NativeAudioParam::SetValue),
                InstanceAccessor("defaultValue", &NativeAudioParam::GetDefaultValue, nullptr),
                InstanceMethod("setValueAtTime", &NativeAudioParam::SetValueAtTime),
                InstanceMethod("linearRampToValueAtTime", &NativeAudioParam::LinearRampToValueAtTime),
                InstanceMethod("exponentialRampToValueAtTime", &NativeAudioParam::ExponentialRampToValueAtTime),
                InstanceMethod("setValueCurveAtTime", &NativeAudioParam::SetValueCurveAtTime),
                InstanceMethod("cancelScheduledValues", &NativeAudioParam::CancelScheduledValues),
            });

        JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_CONSTRUCTOR_NAME, func);
        env.Global().Set(JS_CONSTRUCTOR_NAME, func);
    }

    Napi::Object NativeAudioParam::CreateInstance(
        Napi::Env env,
        const std::shared_ptr<AudioParamState>& state,
        float defaultValue,
        const std::function<double()>& getCurrentTime)
    {
        auto func = JsRuntime::NativeObject::GetFromJavaScript(env).Get(JS_CONSTRUCTOR_NAME).As<Napi::Function>();
        auto object = func.New({});
        auto* param = NativeAudioParam::Unwrap(object);
        param->m_state = state;
        param->m_defaultValue = defaultValue;
        param->m_getCurrentTime = getCurrentTime;
        if (state)
        {
            state->SetDefault(defaultValue);
        }
        return object;
    }

    NativeAudioParam::NativeAudioParam(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<NativeAudioParam>{info}
        , m_state{std::make_shared<AudioParamState>()}
    {
    }

    std::shared_ptr<AudioParamState> NativeAudioParam::State() const
    {
        return m_state;
    }

    double NativeAudioParam::CurrentTime() const
    {
        return m_getCurrentTime ? m_getCurrentTime() : 0.0;
    }

    void NativeAudioParam::SetImmediate(float value)
    {
        if (m_state)
        {
            m_state->SetImmediate(value, CurrentTime());
        }
    }

    Napi::Value NativeAudioParam::GetValue(const Napi::CallbackInfo& info)
    {
        const float value = m_state ? m_state->GetAtTime(CurrentTime()) : 0.f;
        return Napi::Number::New(info.Env(), value);
    }

    void NativeAudioParam::SetValue(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        SetImmediate(value.As<Napi::Number>().FloatValue());
    }

    Napi::Value NativeAudioParam::GetDefaultValue(const Napi::CallbackInfo& info)
    {
        return Napi::Number::New(info.Env(), m_defaultValue);
    }

    Napi::Value NativeAudioParam::SetValueAtTime(const Napi::CallbackInfo& info)
    {
        if (m_state && info.Length() >= 2)
        {
            m_state->SetValueAtTime(info[0].As<Napi::Number>().FloatValue(), info[1].As<Napi::Number>().DoubleValue());
        }
        return info.This();
    }

    Napi::Value NativeAudioParam::LinearRampToValueAtTime(const Napi::CallbackInfo& info)
    {
        if (m_state && info.Length() >= 2)
        {
            m_state->LinearRampToValueAtTime(info[0].As<Napi::Number>().FloatValue(), info[1].As<Napi::Number>().DoubleValue());
        }
        return info.This();
    }

    Napi::Value NativeAudioParam::ExponentialRampToValueAtTime(const Napi::CallbackInfo& info)
    {
        if (m_state && info.Length() >= 2)
        {
            m_state->ExponentialRampToValueAtTime(info[0].As<Napi::Number>().FloatValue(), info[1].As<Napi::Number>().DoubleValue());
        }
        return info.This();
    }

    Napi::Value NativeAudioParam::SetValueCurveAtTime(const Napi::CallbackInfo& info)
    {
        if (m_state && info.Length() >= 3 && info[0].IsTypedArray())
        {
            auto values = info[0].As<Napi::Float32Array>();
            const double startTime = info[1].As<Napi::Number>().DoubleValue();
            const double duration = info[2].As<Napi::Number>().DoubleValue();

            std::vector<float> curve(values.ElementLength());
            for (size_t index = 0; index < values.ElementLength(); ++index)
            {
                curve[index] = values[index];
            }

            m_state->SetValueCurveAtTime(curve, startTime, duration);
        }
        return info.This();
    }

    Napi::Value NativeAudioParam::CancelScheduledValues(const Napi::CallbackInfo& info)
    {
        if (m_state && info.Length() >= 1)
        {
            m_state->CancelScheduledValues(info[0].As<Napi::Number>().DoubleValue());
        }
        return info.This();
    }
}
