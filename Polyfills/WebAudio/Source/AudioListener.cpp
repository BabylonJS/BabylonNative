#include "AudioListener.h"
#include "AudioEngine.h"
#include "AudioParam.h"

#include <Babylon/JsRuntime.h>

namespace Babylon::Polyfills::Internal
{
    void NativeAudioListener::Initialize(Napi::Env env)
    {
        Napi::HandleScope scope{env};

        Napi::Function func = DefineClass(
            env,
            JS_CONSTRUCTOR_NAME,
            {
                InstanceAccessor("positionX", &NativeAudioListener::GetPositionX, nullptr),
                InstanceAccessor("positionY", &NativeAudioListener::GetPositionY, nullptr),
                InstanceAccessor("positionZ", &NativeAudioListener::GetPositionZ, nullptr),
                InstanceAccessor("forwardX", &NativeAudioListener::GetForwardX, nullptr),
                InstanceAccessor("forwardY", &NativeAudioListener::GetForwardY, nullptr),
                InstanceAccessor("forwardZ", &NativeAudioListener::GetForwardZ, nullptr),
                InstanceAccessor("upX", &NativeAudioListener::GetUpX, nullptr),
                InstanceAccessor("upY", &NativeAudioListener::GetUpY, nullptr),
                InstanceAccessor("upZ", &NativeAudioListener::GetUpZ, nullptr),
                InstanceMethod("setPosition", &NativeAudioListener::SetPosition),
                InstanceMethod("setOrientation", &NativeAudioListener::SetOrientation),
            });

        JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_CONSTRUCTOR_NAME, func);
    }

    Napi::Object NativeAudioListener::CreateInstance(Napi::Env env, AudioListenerState& listener, const std::function<double()>& getCurrentTime)
    {
        auto func = JsRuntime::NativeObject::GetFromJavaScript(env).Get(JS_CONSTRUCTOR_NAME).As<Napi::Function>();
        auto object = func.New({});
        auto* self = NativeAudioListener::Unwrap(object);
        self->m_listener = &listener;
        self->m_getCurrentTime = getCurrentTime;

        auto bindParam = [&](AudioParamState& state, float defaultValue, Napi::ObjectReference& ref) {
            auto statePtr = std::shared_ptr<AudioParamState>(std::shared_ptr<void>{}, &state);
            auto param = NativeAudioParam::CreateInstance(env, statePtr, defaultValue, getCurrentTime);
            ref = Napi::Persistent(param);
            state.SetDefault(defaultValue);
        };

        bindParam(listener.positionX, 0.f, self->m_positionX);
        bindParam(listener.positionY, 0.f, self->m_positionY);
        bindParam(listener.positionZ, 0.f, self->m_positionZ);
        bindParam(listener.forwardX, 0.f, self->m_forwardX);
        bindParam(listener.forwardY, 0.f, self->m_forwardY);
        bindParam(listener.forwardZ, -1.f, self->m_forwardZ);
        bindParam(listener.upX, 0.f, self->m_upX);
        bindParam(listener.upY, 1.f, self->m_upY);
        bindParam(listener.upZ, 0.f, self->m_upZ);

        return object;
    }

    NativeAudioListener::NativeAudioListener(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<NativeAudioListener>{info}
    {
    }

    Napi::Value NativeAudioListener::GetPositionX(const Napi::CallbackInfo&) { return m_positionX.Value(); }
    Napi::Value NativeAudioListener::GetPositionY(const Napi::CallbackInfo&) { return m_positionY.Value(); }
    Napi::Value NativeAudioListener::GetPositionZ(const Napi::CallbackInfo&) { return m_positionZ.Value(); }
    Napi::Value NativeAudioListener::GetForwardX(const Napi::CallbackInfo&) { return m_forwardX.Value(); }
    Napi::Value NativeAudioListener::GetForwardY(const Napi::CallbackInfo&) { return m_forwardY.Value(); }
    Napi::Value NativeAudioListener::GetForwardZ(const Napi::CallbackInfo&) { return m_forwardZ.Value(); }
    Napi::Value NativeAudioListener::GetUpX(const Napi::CallbackInfo&) { return m_upX.Value(); }
    Napi::Value NativeAudioListener::GetUpY(const Napi::CallbackInfo&) { return m_upY.Value(); }
    Napi::Value NativeAudioListener::GetUpZ(const Napi::CallbackInfo&) { return m_upZ.Value(); }

    void NativeAudioListener::SetPosition(const Napi::CallbackInfo& info)
    {
        if (!m_listener || info.Length() < 3)
        {
            return;
        }

        const double currentTime = m_getCurrentTime ? m_getCurrentTime() : 0.0;
        m_listener->positionX.SetImmediate(info[0].As<Napi::Number>().FloatValue(), currentTime);
        m_listener->positionY.SetImmediate(info[1].As<Napi::Number>().FloatValue(), currentTime);
        m_listener->positionZ.SetImmediate(info[2].As<Napi::Number>().FloatValue(), currentTime);
    }

    void NativeAudioListener::SetOrientation(const Napi::CallbackInfo& info)
    {
        if (!m_listener || info.Length() < 6)
        {
            return;
        }

        const double currentTime = m_getCurrentTime ? m_getCurrentTime() : 0.0;
        m_listener->forwardX.SetImmediate(info[0].As<Napi::Number>().FloatValue(), currentTime);
        m_listener->forwardY.SetImmediate(info[1].As<Napi::Number>().FloatValue(), currentTime);
        m_listener->forwardZ.SetImmediate(info[2].As<Napi::Number>().FloatValue(), currentTime);
        m_listener->upX.SetImmediate(info[3].As<Napi::Number>().FloatValue(), currentTime);
        m_listener->upY.SetImmediate(info[4].As<Napi::Number>().FloatValue(), currentTime);
        m_listener->upZ.SetImmediate(info[5].As<Napi::Number>().FloatValue(), currentTime);
    }
}
