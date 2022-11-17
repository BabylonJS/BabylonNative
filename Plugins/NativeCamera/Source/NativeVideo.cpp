#include "NativeVideo.h"
#include "NativeCameraImpl.h"

namespace Babylon::Plugins
{
    static constexpr auto JS_CLASS_NAME = "NativeVideo";

    void NativeVideo::Initialize(Napi::Env& env, std::shared_ptr<Plugins::Camera::Impl> nativeCameraImpl)
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
                InstanceAccessor("videoWidth", &NativeVideo::GetVideoWidth, nullptr),
                InstanceAccessor("videoHeight", &NativeVideo::GetVideoHeight, nullptr),
                InstanceAccessor("frontCamera", nullptr, &NativeVideo::SetFrontCamera),
                InstanceAccessor("isNative", &NativeVideo::IsNative, nullptr),
                InstanceAccessor("readyState", &NativeVideo::GetReadyState, nullptr),
                InstanceAccessor("HAVE_CURRENT_DATA", &NativeVideo::GetHaveCurrentData, nullptr),
            });

        env.Global().Set(JS_CLASS_NAME, func);

        NativeCameraImpl = nativeCameraImpl;
    }

    Napi::Object NativeVideo::New(const Napi::CallbackInfo& info, uint32_t width, uint32_t height, bool frontCamera)
    {
        return info.Env().Global().Get(JS_CLASS_NAME).As<Napi::Function>().New({ Napi::Value::From(info.Env(), width), Napi::Value::From(info.Env(), height), Napi::Value::From(info.Env(), frontCamera) });
    }

    NativeVideo::NativeVideo(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<NativeVideo>{ info }
        , m_runtimeScheduler{JsRuntime::GetFromJavaScript(info.Env())}
        , m_maxWidth{ info[0].As<Napi::Number>().Uint32Value() }
        , m_maxHeight{ info[1].As<Napi::Number>().Uint32Value() }
        , m_frontCamera{ info[2].As<Napi::Boolean>().Value() }
    {
    }

    Napi::Value NativeVideo::GetVideoWidth(const Napi::CallbackInfo& /*info*/)
    {
        return Napi::Value::From(Env(), m_width);
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
        return Napi::Value::From(Env(), m_isReady ? 10u : 0u);
    }

    Napi::Value NativeVideo::GetHaveCurrentData(const Napi::CallbackInfo& /*info*/)
    {
        return Napi::Value::From(Env(), 1u);
    }

    void NativeVideo::UpdateTexture(bgfx::TextureHandle textureHandle)
    {
        NativeCameraImpl->UpdateCameraTexture(textureHandle);
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

    Napi::Value NativeVideo::Play(const Napi::CallbackInfo& info)
    {
        auto env{info.Env()};
        auto deferred{Napi::Promise::Deferred::New(env)};

        if (!m_IsPlaying)
        {
            m_IsPlaying = true;
            NativeCameraImpl->Open(m_maxWidth, m_maxHeight, m_frontCamera).then(m_runtimeScheduler, arcana::cancellation::none(), [this, env, deferred](const arcana::expected<Camera::Impl::CameraDimensions, std::exception_ptr>& result) {
                if (result.has_error())
                {
                    deferred.Reject(Napi::Error::New(env, result.error()).Value());
                }
                else
                {
                    auto cameraDimensions{result.value()};
                    this->m_width = cameraDimensions.width;
                    this->m_height = cameraDimensions.height;
                    this->m_isReady = true;
                    deferred.Resolve(env.Undefined());
                    RaiseEvent("playing");
                }
            });
        }
        else
        {
            deferred.Resolve(env.Undefined());
        }

        return deferred.Promise();
    }

    void NativeVideo::Pause(const Napi::CallbackInfo& /*info*/)
    {
        m_IsPlaying = false;
        NativeCameraImpl->Close();
    }
}
