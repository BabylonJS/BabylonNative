#pragma once

#include "AudioNode.h"

#include <napi/napi.h>

#include <functional>

namespace Babylon::Polyfills::Internal
{
    class NativeAudioListener final : public Napi::ObjectWrap<NativeAudioListener>
    {
    public:
        static void Initialize(Napi::Env env);
        static Napi::Object CreateInstance(Napi::Env env, AudioListenerState& listener, const std::function<double()>& getCurrentTime);

        explicit NativeAudioListener(const Napi::CallbackInfo& info);

    private:
        static constexpr auto JS_CONSTRUCTOR_NAME = "AudioListener";

        Napi::Value GetPositionX(const Napi::CallbackInfo& info);
        Napi::Value GetPositionY(const Napi::CallbackInfo& info);
        Napi::Value GetPositionZ(const Napi::CallbackInfo& info);
        Napi::Value GetForwardX(const Napi::CallbackInfo& info);
        Napi::Value GetForwardY(const Napi::CallbackInfo& info);
        Napi::Value GetForwardZ(const Napi::CallbackInfo& info);
        Napi::Value GetUpX(const Napi::CallbackInfo& info);
        Napi::Value GetUpY(const Napi::CallbackInfo& info);
        Napi::Value GetUpZ(const Napi::CallbackInfo& info);
        void SetPosition(const Napi::CallbackInfo& info);
        void SetOrientation(const Napi::CallbackInfo& info);

        AudioListenerState* m_listener{nullptr};
        std::function<double()> m_getCurrentTime{};
        Napi::ObjectReference m_positionX{};
        Napi::ObjectReference m_positionY{};
        Napi::ObjectReference m_positionZ{};
        Napi::ObjectReference m_forwardX{};
        Napi::ObjectReference m_forwardY{};
        Napi::ObjectReference m_forwardZ{};
        Napi::ObjectReference m_upX{};
        Napi::ObjectReference m_upY{};
        Napi::ObjectReference m_upZ{};
    };
}
