#include "PannerNode.h"
#include "AudioContext.h"
#include "AudioEngine.h"
#include "AudioParam.h"

#include <Babylon/JsRuntime.h>

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

    void NativePannerNode::Initialize(Napi::Env env)
    {
        Napi::HandleScope scope{env};

        Napi::Function func = DefineClass(
            env,
            JS_CONSTRUCTOR_NAME,
            {
                InstanceMethod("connect", &NativePannerNode::Connect),
                InstanceMethod("disconnect", &NativePannerNode::Disconnect),
                InstanceMethod("setPosition", &NativePannerNode::SetPosition),
                InstanceMethod("setOrientation", &NativePannerNode::SetOrientation),
                InstanceAccessor("positionX", &NativePannerNode::GetPositionX, nullptr),
                InstanceAccessor("positionY", &NativePannerNode::GetPositionY, nullptr),
                InstanceAccessor("positionZ", &NativePannerNode::GetPositionZ, nullptr),
                InstanceAccessor("orientationX", &NativePannerNode::GetOrientationX, nullptr),
                InstanceAccessor("orientationY", &NativePannerNode::GetOrientationY, nullptr),
                InstanceAccessor("orientationZ", &NativePannerNode::GetOrientationZ, nullptr),
                InstanceAccessor("panningModel", &NativePannerNode::GetPanningModel, &NativePannerNode::SetPanningModel),
                InstanceAccessor("distanceModel", &NativePannerNode::GetDistanceModel, &NativePannerNode::SetDistanceModel),
                InstanceAccessor("refDistance", &NativePannerNode::GetRefDistance, &NativePannerNode::SetRefDistance),
                InstanceAccessor("maxDistance", &NativePannerNode::GetMaxDistance, &NativePannerNode::SetMaxDistance),
                InstanceAccessor("rolloffFactor", &NativePannerNode::GetRolloffFactor, &NativePannerNode::SetRolloffFactor),
                InstanceAccessor("coneInnerAngle", &NativePannerNode::GetConeInnerAngle, &NativePannerNode::SetConeInnerAngle),
                InstanceAccessor("coneOuterAngle", &NativePannerNode::GetConeOuterAngle, &NativePannerNode::SetConeOuterAngle),
                InstanceAccessor("coneOuterGain", &NativePannerNode::GetConeOuterGain, &NativePannerNode::SetConeOuterGain),
            });

        JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_CONSTRUCTOR_NAME, func);
        env.Global().Set(JS_CONSTRUCTOR_NAME, func);
    }

    Napi::Object NativePannerNode::CreateInstance(Napi::Env env, const std::shared_ptr<AudioContextState>& context)
    {
        auto func = JsRuntime::NativeObject::GetFromJavaScript(env).Get(JS_CONSTRUCTOR_NAME).As<Napi::Function>();
        auto object = func.New({});
        NativePannerNode::Unwrap(object)->InitializeState(context);
        return object;
    }

    NativePannerNode::NativePannerNode(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<NativePannerNode>{info}
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

    void NativePannerNode::InitializeState(const std::shared_ptr<AudioContextState>& context)
    {
        m_state = std::make_shared<PannerNodeState>();
        m_state->context = context;
        BindNodeState(Value(), m_state);
        SetAudioNodeProperties(Value(), 1, 1);

        auto getCurrentTime = MakeCurrentTimeGetter(context);
        m_positionX = Napi::Persistent(NativeAudioParam::CreateInstance(Env(), std::shared_ptr<AudioParamState>(m_state, &m_state->positionX), 0.f, getCurrentTime));
        m_positionY = Napi::Persistent(NativeAudioParam::CreateInstance(Env(), std::shared_ptr<AudioParamState>(m_state, &m_state->positionY), 0.f, getCurrentTime));
        m_positionZ = Napi::Persistent(NativeAudioParam::CreateInstance(Env(), std::shared_ptr<AudioParamState>(m_state, &m_state->positionZ), 0.f, getCurrentTime));
        m_orientationX = Napi::Persistent(NativeAudioParam::CreateInstance(Env(), std::shared_ptr<AudioParamState>(m_state, &m_state->orientationX), 1.f, getCurrentTime));
        m_orientationY = Napi::Persistent(NativeAudioParam::CreateInstance(Env(), std::shared_ptr<AudioParamState>(m_state, &m_state->orientationY), 0.f, getCurrentTime));
        m_orientationZ = Napi::Persistent(NativeAudioParam::CreateInstance(Env(), std::shared_ptr<AudioParamState>(m_state, &m_state->orientationZ), 0.f, getCurrentTime));
    }

    Napi::Value NativePannerNode::Connect(const Napi::CallbackInfo& info)
    {
        auto destination = UnwrapNodeState(info[0]);
        if (m_state && destination)
        {
            LockAnd(m_state->context.lock(), [&]() { m_state->Connect(destination); });
        }
        return info[0];
    }

    Napi::Value NativePannerNode::Disconnect(const Napi::CallbackInfo& info)
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

    void NativePannerNode::SetPosition(const Napi::CallbackInfo& info)
    {
        if (!m_state || info.Length() < 3)
        {
            return;
        }
        auto context = m_state->context.lock();
        const double currentTime = context && context->engine ? context->engine->CurrentTime() : 0.0;
        m_state->positionX.SetImmediate(info[0].As<Napi::Number>().FloatValue(), currentTime);
        m_state->positionY.SetImmediate(info[1].As<Napi::Number>().FloatValue(), currentTime);
        m_state->positionZ.SetImmediate(info[2].As<Napi::Number>().FloatValue(), currentTime);
    }

    void NativePannerNode::SetOrientation(const Napi::CallbackInfo& info)
    {
        if (!m_state || info.Length() < 3)
        {
            return;
        }
        auto context = m_state->context.lock();
        const double currentTime = context && context->engine ? context->engine->CurrentTime() : 0.0;
        m_state->orientationX.SetImmediate(info[0].As<Napi::Number>().FloatValue(), currentTime);
        m_state->orientationY.SetImmediate(info[1].As<Napi::Number>().FloatValue(), currentTime);
        m_state->orientationZ.SetImmediate(info[2].As<Napi::Number>().FloatValue(), currentTime);
    }

    Napi::Value NativePannerNode::GetPositionX(const Napi::CallbackInfo&) { return m_positionX.Value(); }
    Napi::Value NativePannerNode::GetPositionY(const Napi::CallbackInfo&) { return m_positionY.Value(); }
    Napi::Value NativePannerNode::GetPositionZ(const Napi::CallbackInfo&) { return m_positionZ.Value(); }
    Napi::Value NativePannerNode::GetOrientationX(const Napi::CallbackInfo&) { return m_orientationX.Value(); }
    Napi::Value NativePannerNode::GetOrientationY(const Napi::CallbackInfo&) { return m_orientationY.Value(); }
    Napi::Value NativePannerNode::GetOrientationZ(const Napi::CallbackInfo&) { return m_orientationZ.Value(); }

    Napi::Value NativePannerNode::GetPanningModel(const Napi::CallbackInfo& info)
    {
        return Napi::String::New(info.Env(), m_state ? m_state->panningModel : "equalpower");
    }

    void NativePannerNode::SetPanningModel(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        if (m_state)
        {
            m_state->panningModel = value.ToString().Utf8Value();
        }
    }

    Napi::Value NativePannerNode::GetDistanceModel(const Napi::CallbackInfo& info)
    {
        return Napi::String::New(info.Env(), m_state ? m_state->distanceModel : "inverse");
    }

    void NativePannerNode::SetDistanceModel(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        if (m_state)
        {
            m_state->distanceModel = value.ToString().Utf8Value();
        }
    }

    Napi::Value NativePannerNode::GetRefDistance(const Napi::CallbackInfo& info)
    {
        return Napi::Number::New(info.Env(), m_state ? m_state->refDistance : 1.f);
    }

    void NativePannerNode::SetRefDistance(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        if (m_state)
        {
            m_state->refDistance = value.ToNumber().FloatValue();
        }
    }

    Napi::Value NativePannerNode::GetMaxDistance(const Napi::CallbackInfo& info)
    {
        return Napi::Number::New(info.Env(), m_state ? m_state->maxDistance : 10000.f);
    }

    void NativePannerNode::SetMaxDistance(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        if (m_state)
        {
            m_state->maxDistance = value.ToNumber().FloatValue();
        }
    }

    Napi::Value NativePannerNode::GetRolloffFactor(const Napi::CallbackInfo& info)
    {
        return Napi::Number::New(info.Env(), m_state ? m_state->rolloffFactor : 1.f);
    }

    void NativePannerNode::SetRolloffFactor(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        if (m_state)
        {
            m_state->rolloffFactor = value.ToNumber().FloatValue();
        }
    }

    Napi::Value NativePannerNode::GetConeInnerAngle(const Napi::CallbackInfo& info)
    {
        return Napi::Number::New(info.Env(), m_state ? m_state->coneInnerAngle : 360.f);
    }

    void NativePannerNode::SetConeInnerAngle(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        if (m_state)
        {
            m_state->coneInnerAngle = value.ToNumber().FloatValue();
        }
    }

    Napi::Value NativePannerNode::GetConeOuterAngle(const Napi::CallbackInfo& info)
    {
        return Napi::Number::New(info.Env(), m_state ? m_state->coneOuterAngle : 360.f);
    }

    void NativePannerNode::SetConeOuterAngle(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        if (m_state)
        {
            m_state->coneOuterAngle = value.ToNumber().FloatValue();
        }
    }

    Napi::Value NativePannerNode::GetConeOuterGain(const Napi::CallbackInfo& info)
    {
        return Napi::Number::New(info.Env(), m_state ? m_state->coneOuterGain : 0.f);
    }

    void NativePannerNode::SetConeOuterGain(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        if (m_state)
        {
            m_state->coneOuterGain = value.ToNumber().FloatValue();
        }
    }
}
