#include "NativeVideo.h"

namespace Babylon::Plugins::Internal
{
    void NativeVideo::Initialize(Napi::Env& env)
    {
        Napi::Function func = DefineClass(
            env,
            JS_CLASS_NAME,
            {
                InstanceMethod("addEventListener", &NativeVideo::AddEventListener),
                InstanceMethod("removeEventListener", &NativeVideo::RemoveEventListener),
                InstanceMethod("play", &NativeVideo::Play),
                InstanceMethod("pause", &NativeVideo::Pause),
                InstanceMethod("setAttribute", &NativeVideo::SetAttribute),
                InstanceAccessor("videoWidth", &NativeVideo::GetVideoWidth, &NativeVideo::SetVideoWidth),
                InstanceAccessor("videoHeight", &NativeVideo::GetVideoHeight, &NativeVideo::SetVideoHeight),
                InstanceAccessor("frontCamera", nullptr, &NativeVideo::SetFrontCamera),
                InstanceAccessor("isNative", &NativeVideo::IsNative, nullptr),
                InstanceAccessor("readyState", &NativeVideo::GetReadyState, nullptr),
                InstanceAccessor("HAVE_CURRENT_DATA", &NativeVideo::GetHaveCurrentData, nullptr),
            });

        env.Global().Set(JS_CLASS_NAME, func);
    }

    Napi::Object NativeVideo::New(const Napi::CallbackInfo& info, uint32_t width, uint32_t height, bool frontCamera)
    {
        return info.Env().Global().Get(JS_CLASS_NAME).As<Napi::Function>().New({ Napi::Value::From(info.Env(), width), Napi::Value::From(info.Env(), height), Napi::Value::From(info.Env(), frontCamera) });
    }

    NativeVideo::NativeVideo(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<NativeVideo>{ info }
        , m_width{ info[0].As<Napi::Number>().Uint32Value() }
        , m_height{ info[1].As<Napi::Number>().Uint32Value() }
        , m_frontCamera{ info[2].As<Napi::Boolean>().Value() }
    {
    }

    void NativeVideo::SetVideoWidth(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        m_width = value.As<Napi::Number>().Uint32Value();
    }

    Napi::Value NativeVideo::GetVideoWidth(const Napi::CallbackInfo& /*info*/)
    {
        return Napi::Value::From(Env(), m_width);
    }

    void NativeVideo::SetVideoHeight(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        m_height = value.As<Napi::Number>().Uint32Value();
    }

    Napi::Value NativeVideo::GetVideoHeight(const Napi::CallbackInfo& /*info*/)
    {
        return Napi::Value::From(Env(), m_height);
    }

    void NativeVideo::SetFrontCamera(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        m_frontCamera = value.As<Napi::Boolean>().Value();
    }

    Napi::Value NativeVideo::IsNative(const Napi::CallbackInfo&)
    {
        return Napi::Value::From(Env(), true);
    }

    void NativeVideo::SetAttribute(const Napi::CallbackInfo&)
    {
    }

    Napi::Value NativeVideo::GetReadyState(const Napi::CallbackInfo& /*info*/)
    {
        return Napi::Value::From(Env(), 10u);
    }

    Napi::Value NativeVideo::GetHaveCurrentData(const Napi::CallbackInfo& /*info*/)
    {
        return Napi::Value::From(Env(), 1u);
    }

    void NativeVideo::UpdateTexture(bgfx::TextureHandle textureHandle)
    {
        if (!m_cameraInterface)
        {
            m_cameraInterface = CameraInterface::CreateInterface(Env(), m_width, m_height, m_frontCamera);
        }
        m_cameraInterface->UpdateCameraTexture(textureHandle);
    }

    void NativeVideo::AddEventListener(const Napi::CallbackInfo& info)
    {
        std::string eventType = info[0].As<Napi::String>().Utf8Value();
        Napi::Function eventHandler = info[1].As<Napi::Function>();

        const auto& eventHandlerRefs = m_eventHandlerRefs[eventType];
        for (auto it = eventHandlerRefs.begin(); it != eventHandlerRefs.end(); ++it)
        {
            if (it->Value() == eventHandler)
            {
                throw Napi::Error::New(info.Env(), "Cannot add the same event handler twice");
            }
        }

        m_eventHandlerRefs[eventType].push_back(Napi::Persistent(eventHandler));
    }

    void NativeVideo::RemoveEventListener(const Napi::CallbackInfo& info)
    {
        std::string eventType = info[0].As<Napi::String>().Utf8Value();
        Napi::Function eventHandler = info[1].As<Napi::Function>();
        auto itType = m_eventHandlerRefs.find(eventType);
        if (itType != m_eventHandlerRefs.end())
        {
            auto& eventHandlerRefs = itType->second;
            for (auto it = eventHandlerRefs.begin(); it != eventHandlerRefs.end(); ++it)
            {
                if (it->Value() == eventHandler)
                {
                    eventHandlerRefs.erase(it);
                    break;
                }
            }
        }
    }

    void NativeVideo::RaiseEvent(const char* eventType)
    {
        auto it = m_eventHandlerRefs.find(eventType);
        if (it != m_eventHandlerRefs.end())
        {
            const auto& eventHandlerRefs = it->second;
            for (const auto& eventHandlerRef : eventHandlerRefs)
            {
                eventHandlerRef.Call({});
            }
        }
    }

    void NativeVideo::Play(const Napi::CallbackInfo& /*info*/)
    {
        if (!m_IsPlaying)
        {
            m_IsPlaying = true;
            RaiseEvent("playing");
        }
    }

    void NativeVideo::Pause(const Napi::CallbackInfo& /*info*/)
    {
        m_IsPlaying = false;
        m_cameraInterface.reset();
    }
}