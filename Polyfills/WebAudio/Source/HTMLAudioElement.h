#pragma once

#include <napi/napi.h>

#include <string>

namespace Babylon::Polyfills::Internal
{
    class NativeHTMLAudioElement final : public Napi::ObjectWrap<NativeHTMLAudioElement>
    {
    public:
        static void Initialize(Napi::Env env);

        explicit NativeHTMLAudioElement(const Napi::CallbackInfo& info);

    private:
        static constexpr auto JS_CONSTRUCTOR_NAME = "Audio";

        Napi::Value CanPlayType(const Napi::CallbackInfo& info);
        Napi::Value Play(const Napi::CallbackInfo& info);
        void Pause(const Napi::CallbackInfo& info);

        Napi::Value GetControls(const Napi::CallbackInfo& info);
        void SetControls(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetPreload(const Napi::CallbackInfo& info);
        void SetPreload(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetLoop(const Napi::CallbackInfo& info);
        void SetLoop(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetSrc(const Napi::CallbackInfo& info);
        void SetSrc(const Napi::CallbackInfo& info, const Napi::Value& value);

        bool m_controls{false};
        std::string m_preload{"auto"};
        bool m_loop{false};
        std::string m_src{};
    };
}
