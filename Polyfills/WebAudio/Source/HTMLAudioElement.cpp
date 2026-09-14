#include "HTMLAudioElement.h"

#include <Babylon/JsRuntime.h>

#include <algorithm>
#include <cctype>
#include <string>

namespace Babylon::Polyfills::Internal
{
    namespace
    {
        bool IsSupportedMimeType(std::string mimeType)
        {
            std::transform(mimeType.begin(), mimeType.end(), mimeType.begin(), [](unsigned char c) {
                return static_cast<char>(std::tolower(c));
            });

            return mimeType.find("audio/wav") != std::string::npos
                || mimeType.find("audio/wave") != std::string::npos
                || mimeType.find("audio/x-wav") != std::string::npos
                || mimeType.find("audio/mpeg") != std::string::npos
                || mimeType.find("audio/mp3") != std::string::npos
                || mimeType.find("audio/mp4") != std::string::npos
                || mimeType.find("audio/aac") != std::string::npos
                || mimeType.find("audio/flac") != std::string::npos
                || mimeType.find("audio/ogg") != std::string::npos
                || mimeType.find("audio/webm") != std::string::npos
                || mimeType.find("audio/ac3") != std::string::npos;
        }
    }

    void NativeHTMLAudioElement::Initialize(Napi::Env env)
    {
        Napi::HandleScope scope{env};

        Napi::Function func = DefineClass(
            env,
            JS_CONSTRUCTOR_NAME,
            {
                InstanceMethod("canPlayType", &NativeHTMLAudioElement::CanPlayType),
                InstanceMethod("play", &NativeHTMLAudioElement::Play),
                InstanceMethod("pause", &NativeHTMLAudioElement::Pause),
                InstanceAccessor("controls", &NativeHTMLAudioElement::GetControls, &NativeHTMLAudioElement::SetControls),
                InstanceAccessor("preload", &NativeHTMLAudioElement::GetPreload, &NativeHTMLAudioElement::SetPreload),
                InstanceAccessor("loop", &NativeHTMLAudioElement::GetLoop, &NativeHTMLAudioElement::SetLoop),
                InstanceAccessor("src", &NativeHTMLAudioElement::GetSrc, &NativeHTMLAudioElement::SetSrc),
            });

        JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_CONSTRUCTOR_NAME, func);
        env.Global().Set(JS_CONSTRUCTOR_NAME, func);
    }

    NativeHTMLAudioElement::NativeHTMLAudioElement(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<NativeHTMLAudioElement>{info}
    {
    }

    Napi::Value NativeHTMLAudioElement::CanPlayType(const Napi::CallbackInfo& info)
    {
        if (info.Length() < 1 || !info[0].IsString())
        {
            return Napi::String::New(info.Env(), "");
        }

        const auto mimeType = info[0].As<Napi::String>().Utf8Value();
        return Napi::String::New(info.Env(), IsSupportedMimeType(mimeType) ? "probably" : "");
    }

    Napi::Value NativeHTMLAudioElement::Play(const Napi::CallbackInfo& info)
    {
        auto deferred = Napi::Promise::Deferred::New(info.Env());
        deferred.Resolve(info.Env().Undefined());
        return deferred.Promise();
    }

    void NativeHTMLAudioElement::Pause(const Napi::CallbackInfo&)
    {
    }

    Napi::Value NativeHTMLAudioElement::GetControls(const Napi::CallbackInfo& info)
    {
        return Napi::Boolean::New(info.Env(), m_controls);
    }

    void NativeHTMLAudioElement::SetControls(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        m_controls = value.ToBoolean();
    }

    Napi::Value NativeHTMLAudioElement::GetPreload(const Napi::CallbackInfo& info)
    {
        return Napi::String::New(info.Env(), m_preload);
    }

    void NativeHTMLAudioElement::SetPreload(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        m_preload = value.ToString().Utf8Value();
    }

    Napi::Value NativeHTMLAudioElement::GetLoop(const Napi::CallbackInfo& info)
    {
        return Napi::Boolean::New(info.Env(), m_loop);
    }

    void NativeHTMLAudioElement::SetLoop(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        m_loop = value.ToBoolean();
    }

    Napi::Value NativeHTMLAudioElement::GetSrc(const Napi::CallbackInfo& info)
    {
        return Napi::String::New(info.Env(), m_src);
    }

    void NativeHTMLAudioElement::SetSrc(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        m_src = value.ToString().Utf8Value();
    }
}
