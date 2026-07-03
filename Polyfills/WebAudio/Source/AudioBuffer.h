#pragma once

#include "AudioNode.h"

#include <napi/napi.h>

namespace Babylon::Polyfills::Internal
{
    class NativeAudioBuffer final : public Napi::ObjectWrap<NativeAudioBuffer>
    {
    public:
        static void Initialize(Napi::Env env);
        static Napi::Object CreateInstance(Napi::Env env, const std::shared_ptr<AudioBufferData>& data);

        explicit NativeAudioBuffer(const Napi::CallbackInfo& info);

        std::shared_ptr<AudioBufferData> Data() const;

    private:
        static constexpr auto JS_CONSTRUCTOR_NAME = "AudioBuffer";

        Napi::Value GetSampleRate(const Napi::CallbackInfo& info);
        Napi::Value GetLength(const Napi::CallbackInfo& info);
        Napi::Value GetDuration(const Napi::CallbackInfo& info);
        Napi::Value GetNumberOfChannels(const Napi::CallbackInfo& info);
        Napi::Value GetChannelData(const Napi::CallbackInfo& info);
        void CopyFromChannel(const Napi::CallbackInfo& info);
        void CopyToChannel(const Napi::CallbackInfo& info);

        std::shared_ptr<AudioBufferData> m_data{};
    };
}
