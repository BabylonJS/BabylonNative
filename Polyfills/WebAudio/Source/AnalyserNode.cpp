#include "AnalyserNode.h"
#include "AudioContext.h"
#include "AudioEngine.h"

#include <Babylon/JsRuntime.h>

#include <algorithm>
#include <cmath>
#include <cstring>

namespace Babylon::Polyfills::Internal
{
    namespace
    {
        void LockAnd(const std::shared_ptr<AudioContextState>& context, auto&& fn)
        {
            if (context && context->engine)
            {
                std::lock_guard<std::mutex> lock{context->engine->GraphMutex()};
                fn();
            }
            else
            {
                fn();
            }
        }
    }

    void NativeAnalyserNode::Initialize(Napi::Env env)
    {
        Napi::HandleScope scope{env};

        Napi::Function func = DefineClass(
            env,
            JS_CONSTRUCTOR_NAME,
            {
                InstanceMethod("connect", &NativeAnalyserNode::Connect),
                InstanceMethod("disconnect", &NativeAnalyserNode::Disconnect),
                InstanceMethod("getByteFrequencyData", &NativeAnalyserNode::GetByteFrequencyData),
                InstanceMethod("getFloatFrequencyData", &NativeAnalyserNode::GetFloatFrequencyData),
                InstanceMethod("getByteTimeDomainData", &NativeAnalyserNode::GetByteTimeDomainData),
                InstanceMethod("getFloatTimeDomainData", &NativeAnalyserNode::GetFloatTimeDomainData),
                InstanceAccessor("fftSize", &NativeAnalyserNode::GetFftSize, &NativeAnalyserNode::SetFftSize),
                InstanceAccessor("frequencyBinCount", &NativeAnalyserNode::GetFrequencyBinCount, nullptr),
                InstanceAccessor("minDecibels", &NativeAnalyserNode::GetMinDecibels, &NativeAnalyserNode::SetMinDecibels),
                InstanceAccessor("maxDecibels", &NativeAnalyserNode::GetMaxDecibels, &NativeAnalyserNode::SetMaxDecibels),
                InstanceAccessor("smoothingTimeConstant", &NativeAnalyserNode::GetSmoothingTimeConstant, &NativeAnalyserNode::SetSmoothingTimeConstant),
            });

        JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_CONSTRUCTOR_NAME, func);
        env.Global().Set(JS_CONSTRUCTOR_NAME, func);
    }

    Napi::Object NativeAnalyserNode::CreateInstance(Napi::Env env, const std::shared_ptr<AudioContextState>& context)
    {
        auto func = JsRuntime::NativeObject::GetFromJavaScript(env).Get(JS_CONSTRUCTOR_NAME).As<Napi::Function>();
        auto object = func.New({});
        NativeAnalyserNode::Unwrap(object)->InitializeState(context);
        return object;
    }

    NativeAnalyserNode::NativeAnalyserNode(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<NativeAnalyserNode>{info}
    {
        if (info.Length() > 0 && info[0].IsObject())
        {
            auto* context = NativeAudioContext::Unwrap(info[0].As<Napi::Object>());
            if (context)
            {
                info.This().As<Napi::Object>().Set("context", info[0]);
                InitializeState(context->State());
            }
        }
    }

    void NativeAnalyserNode::InitializeState(const std::shared_ptr<AudioContextState>& context)
    {
        m_state = std::make_shared<AnalyserNodeState>();
        m_state->context = context;
        BindNodeState(Value(), m_state);
        SetAudioNodeProperties(Value(), 1, 1);
    }

    Napi::Value NativeAnalyserNode::Connect(const Napi::CallbackInfo& info)
    {
        auto destination = UnwrapNodeState(info[0]);
        if (m_state && destination)
        {
            LockAnd(m_state->context.lock(), [&]() { m_state->Connect(destination); });
        }
        return info[0];
    }

    Napi::Value NativeAnalyserNode::Disconnect(const Napi::CallbackInfo& info)
    {
        if (!m_state)
        {
            return info.Env().Undefined();
        }

        if (info.Length() > 0)
        {
            auto destination = UnwrapNodeState(info[0]);
            LockAnd(m_state->context.lock(), [&]() { m_state->Disconnect(destination); });
        }
        else
        {
            LockAnd(m_state->context.lock(), [&]() { m_state->Disconnect(); });
        }
        return info.Env().Undefined();
    }

    void NativeAnalyserNode::GetByteFrequencyData(const Napi::CallbackInfo& info)
    {
        if (!m_state || info.Length() == 0 || !info[0].IsTypedArray())
        {
            return;
        }

        auto array = info[0].As<Napi::Uint8Array>();
        std::vector<float> magnitudesDb;
        LockAnd(m_state->context.lock(), [&]() { m_state->ComputeFrequencyData(magnitudesDb); });

        const size_t count = std::min(array.ElementLength(), magnitudesDb.size());
        const float dbRange = m_state->maxDecibels - m_state->minDecibels;
        for (size_t index = 0; index < count; ++index)
        {
            const float normalized = dbRange > 0.f ? (magnitudesDb[index] - m_state->minDecibels) / dbRange : 0.f;
            array[index] = static_cast<uint8_t>(std::round(std::max(0.f, std::min(1.f, normalized)) * 255.f));
        }
    }

    void NativeAnalyserNode::GetFloatFrequencyData(const Napi::CallbackInfo& info)
    {
        if (!m_state || info.Length() == 0 || !info[0].IsTypedArray())
        {
            return;
        }

        auto array = info[0].As<Napi::Float32Array>();
        std::vector<float> magnitudesDb;
        LockAnd(m_state->context.lock(), [&]() { m_state->ComputeFrequencyData(magnitudesDb); });

        const size_t count = std::min(array.ElementLength(), magnitudesDb.size());
        for (size_t index = 0; index < count; ++index)
        {
            array[index] = magnitudesDb[index];
        }
    }

    void NativeAnalyserNode::GetByteTimeDomainData(const Napi::CallbackInfo& info)
    {
        if (!m_state || info.Length() == 0 || !info[0].IsTypedArray())
        {
            return;
        }

        auto array = info[0].As<Napi::Uint8Array>();
        std::vector<float> snapshot;
        LockAnd(m_state->context.lock(), [&]() { m_state->CopyTimeDomainSnapshot(snapshot); });

        const size_t count = std::min(array.ElementLength(), snapshot.size());
        for (size_t index = 0; index < count; ++index)
        {
            const float clamped = std::max(-1.f, std::min(1.f, snapshot[index]));
            array[index] = static_cast<uint8_t>(std::round((clamped + 1.f) * 128.f));
        }
    }

    void NativeAnalyserNode::GetFloatTimeDomainData(const Napi::CallbackInfo& info)
    {
        if (!m_state || info.Length() == 0 || !info[0].IsTypedArray())
        {
            return;
        }

        auto array = info[0].As<Napi::Float32Array>();
        std::vector<float> snapshot;
        LockAnd(m_state->context.lock(), [&]() { m_state->CopyTimeDomainSnapshot(snapshot); });

        const size_t count = std::min(array.ElementLength(), snapshot.size());
        for (size_t index = 0; index < count; ++index)
        {
            array[index] = snapshot[index];
        }
    }

    Napi::Value NativeAnalyserNode::GetFftSize(const Napi::CallbackInfo& info)
    {
        return Napi::Number::New(info.Env(), m_state ? m_state->fftSize : 2048);
    }

    void NativeAnalyserNode::SetFftSize(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        if (m_state)
        {
            LockAnd(m_state->context.lock(), [&]() { m_state->SetFftSize(value.ToNumber().Uint32Value()); });
        }
    }

    Napi::Value NativeAnalyserNode::GetFrequencyBinCount(const Napi::CallbackInfo& info)
    {
        return Napi::Number::New(info.Env(), m_state ? m_state->FrequencyBinCount() : 1024);
    }

    Napi::Value NativeAnalyserNode::GetMinDecibels(const Napi::CallbackInfo& info)
    {
        return Napi::Number::New(info.Env(), m_state ? m_state->minDecibels : -100.f);
    }

    void NativeAnalyserNode::SetMinDecibels(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        if (m_state)
        {
            m_state->minDecibels = value.ToNumber().FloatValue();
        }
    }

    Napi::Value NativeAnalyserNode::GetMaxDecibels(const Napi::CallbackInfo& info)
    {
        return Napi::Number::New(info.Env(), m_state ? m_state->maxDecibels : -30.f);
    }

    void NativeAnalyserNode::SetMaxDecibels(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        if (m_state)
        {
            m_state->maxDecibels = value.ToNumber().FloatValue();
        }
    }

    Napi::Value NativeAnalyserNode::GetSmoothingTimeConstant(const Napi::CallbackInfo& info)
    {
        return Napi::Number::New(info.Env(), m_state ? m_state->smoothingTimeConstant : 0.8f);
    }

    void NativeAnalyserNode::SetSmoothingTimeConstant(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        if (m_state)
        {
            m_state->smoothingTimeConstant = value.ToNumber().FloatValue();
        }
    }
}
