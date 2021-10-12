#include "NativeVideo.h"
#include "NativeVideoImpl.h"

namespace Babylon::Plugins
{
    static constexpr auto JS_CLASS_NAME = "NativeVideoStream";

    // Move this struct to Graphics
    struct TextureData final
    {
        ~TextureData()
        {
            if (bgfx::isValid(Handle))
            {
                bgfx::destroy(Handle);
            }
        }

        bgfx::TextureHandle Handle{ bgfx::kInvalidHandle };
        uint32_t Width{ 0 };
        uint32_t Height{ 0 };
        uint32_t Flags{ 0 };
        uint8_t AnisotropicLevel{ 0 };
    };

    void NativeVideoStream::Initialize(Napi::Env& env, std::shared_ptr<Plugins::Video::Impl> nativeCameraImpl)
    {
        Napi::Function func = DefineClass(
            env,
            JS_CLASS_NAME,
            {
                InstanceMethod("addEventListener", &NativeVideoStream::AddEventListener),
                InstanceMethod("removeEventListener", &NativeVideoStream::RemoveEventListener),
                InstanceMethod("play", &NativeVideoStream::Play),
                InstanceMethod("pause", &NativeVideoStream::Pause),
                InstanceMethod("setAttribute", &NativeVideoStream::SetAttribute),
                InstanceAccessor("videoWidth", &NativeVideoStream::GetVideoWidth, &NativeVideoStream::SetVideoWidth),
                InstanceAccessor("videoHeight", &NativeVideoStream::GetVideoHeight, &NativeVideoStream::SetVideoHeight),
                InstanceAccessor("frontCamera", nullptr, &NativeVideoStream::SetFrontCamera),
                InstanceAccessor("isNative", &NativeVideoStream::IsNative, nullptr),
                InstanceAccessor("readyState", &NativeVideoStream::GetReadyState, nullptr),
                InstanceAccessor("HAVE_CURRENT_DATA", &NativeVideoStream::GetHaveCurrentData, nullptr),
                InstanceAccessor("src", &NativeVideoStream::GetSrc, &NativeVideoStream::SetSrc),
                InstanceAccessor("dst", &NativeVideoStream::GetDst, &NativeVideoStream::SetDst),
                InstanceMethod("updateVideoTexture", &NativeVideoStream::UpdateVideoTexture),
                InstanceMethod("addFrame", &NativeVideoStream::AddFrame),
                InstanceMethod("stop", &NativeVideoStream::Stop),
            });

        env.Global().Set(JS_CLASS_NAME, func);

        m_nativeCameraImpl = nativeCameraImpl;
    }

    Napi::Object NativeVideoStream::New(const Napi::CallbackInfo& info)
    {
        return info.Env().Global().Get(JS_CLASS_NAME).As<Napi::Function>().New({});
    }

    NativeVideoStream::NativeVideoStream(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<NativeVideoStream>{ info }
    {
    }

    void NativeVideoStream::SetVideoWidth(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        m_width = value.As<Napi::Number>().Uint32Value();
    }

    Napi::Value NativeVideoStream::GetVideoWidth(const Napi::CallbackInfo& /*info*/)
    {
        return Napi::Value::From(Env(), m_width);
    }

    void NativeVideoStream::SetVideoHeight(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        m_height = value.As<Napi::Number>().Uint32Value();
    }

    Napi::Value NativeVideoStream::GetVideoHeight(const Napi::CallbackInfo& /*info*/)
    {
        return Napi::Value::From(Env(), m_height);
    }

    void NativeVideoStream::SetFrontCamera(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        m_frontCamera = value.As<Napi::Boolean>().Value();
    }

    Napi::Value NativeVideoStream::IsNative(const Napi::CallbackInfo&)
    {
        return Napi::Value::From(Env(), true);
    }

    void NativeVideoStream::SetAttribute(const Napi::CallbackInfo&)
    {
    }

    Napi::Value NativeVideoStream::GetReadyState(const Napi::CallbackInfo& /*info*/)
    {
        return Napi::Value::From(Env(), 10u);
    }

    Napi::Value NativeVideoStream::GetHaveCurrentData(const Napi::CallbackInfo& /*info*/)
    {
        return Napi::Value::From(Env(), 1u);
    }

    void NativeVideoStream::AddEventListener(const Napi::CallbackInfo& info)
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

    void NativeVideoStream::RemoveEventListener(const Napi::CallbackInfo& info)
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

    void NativeVideoStream::RaiseEvent(const char* eventType)
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

    Napi::Value NativeVideoStream::Play(const Napi::CallbackInfo& info)
    {
        if (!m_IsPlaying)
        {
            m_IsPlaying = true;
            m_nativeCameraImpl->Open(m_src);
            RaiseEvent("playing");
        }

        auto deferred = Napi::Promise::Deferred::New(info.Env());
        return deferred.Promise();
    }

    void NativeVideoStream::Pause(const Napi::CallbackInfo& /*info*/)
    {
        m_IsPlaying = false;
        m_nativeCameraImpl->Close();
    }

    void NativeVideoStream::UpdateVideoTexture(const Napi::CallbackInfo& info)
    {
        if (m_IsPlaying)
        {
            const auto texture = info[0].As<Napi::External<TextureData>>().Data();
            const auto invertY = info[1].As<Napi::Boolean>();

            m_nativeCameraImpl->UpdateTexture(texture->Handle);
        }
    }

    Napi::Value NativeVideoStream::GetSrc(const Napi::CallbackInfo& )
    {
        return Napi::Value::From(Env(), m_src);
    }

    void NativeVideoStream::SetSrc(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        m_src = value.As<Napi::String>();
    }

    Napi::Value NativeVideoStream::GetDst(const Napi::CallbackInfo& )
    {
        return Napi::Value::From(Env(), m_dst);
    }

    void NativeVideoStream::SetDst(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        m_dst = value.As<Napi::String>();
    }

    void NativeVideoStream::AddFrame(const Napi::CallbackInfo& info)
    {
        auto data = info[0].As<Napi::ArrayBuffer>();
        auto bytesPtr = static_cast<uint8_t*>(data.Data());
        auto bytesLength = data.ByteLength();
        m_nativeCameraImpl->AddFrame(bytesPtr, bytesLength);
    }

    void NativeVideoStream::Stop(const Napi::CallbackInfo& /*info*/)
    {
        m_nativeCameraImpl->Stop();
    }

    Video::Video(std::shared_ptr<Impl> impl)
        : m_impl{ std::move(impl) }
    {
    }

    Video Video::Initialize(Napi::Env env)
    {
        auto impl{ std::make_shared<Impl>(env) };

        Babylon::Plugins::NativeVideoStream::Initialize(env, impl);

        return { impl };
    }
}