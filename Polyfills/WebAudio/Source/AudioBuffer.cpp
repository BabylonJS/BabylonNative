#include "AudioBuffer.h"

#include <Babylon/JsRuntime.h>

#include <algorithm>
#include <cstring>

namespace Babylon::Polyfills::Internal
{
    void NativeAudioBuffer::Initialize(Napi::Env env)
    {
        Napi::HandleScope scope{env};

        Napi::Function func = DefineClass(
            env,
            JS_CONSTRUCTOR_NAME,
            {
                InstanceAccessor("sampleRate", &NativeAudioBuffer::GetSampleRate, nullptr),
                InstanceAccessor("length", &NativeAudioBuffer::GetLength, nullptr),
                InstanceAccessor("duration", &NativeAudioBuffer::GetDuration, nullptr),
                InstanceAccessor("numberOfChannels", &NativeAudioBuffer::GetNumberOfChannels, nullptr),
                InstanceMethod("getChannelData", &NativeAudioBuffer::GetChannelData),
                InstanceMethod("copyFromChannel", &NativeAudioBuffer::CopyFromChannel),
                InstanceMethod("copyToChannel", &NativeAudioBuffer::CopyToChannel),
            });

        JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_CONSTRUCTOR_NAME, func);
        env.Global().Set(JS_CONSTRUCTOR_NAME, func);
    }

    Napi::Object NativeAudioBuffer::CreateInstance(Napi::Env env, const std::shared_ptr<AudioBufferData>& data)
    {
        auto func = JsRuntime::NativeObject::GetFromJavaScript(env).Get(JS_CONSTRUCTOR_NAME).As<Napi::Function>();
        auto object = func.New({});
        auto* buffer = NativeAudioBuffer::Unwrap(object);
        buffer->m_data = data;
        return object;
    }

    NativeAudioBuffer::NativeAudioBuffer(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<NativeAudioBuffer>{info}
        , m_data{std::make_shared<AudioBufferData>()}
    {
        if (info.Length() > 0 && info[0].IsObject())
        {
            auto options = info[0].As<Napi::Object>();
            m_data->length = options.Get("length").ToNumber().Uint32Value();
            m_data->numberOfChannels = options.Has("numberOfChannels") ? options.Get("numberOfChannels").ToNumber().Uint32Value() : 1;
            m_data->sampleRate = options.Has("sampleRate") ? options.Get("sampleRate").ToNumber().Uint32Value() : 48000;
            m_data->channels.assign(m_data->numberOfChannels, std::vector<float>(m_data->length, 0.f));
        }
    }

    std::shared_ptr<AudioBufferData> NativeAudioBuffer::Data() const
    {
        return m_data;
    }

    Napi::Value NativeAudioBuffer::GetSampleRate(const Napi::CallbackInfo& info)
    {
        return Napi::Number::New(info.Env(), m_data ? m_data->sampleRate : 0);
    }

    Napi::Value NativeAudioBuffer::GetLength(const Napi::CallbackInfo& info)
    {
        return Napi::Number::New(info.Env(), m_data ? m_data->length : 0);
    }

    Napi::Value NativeAudioBuffer::GetDuration(const Napi::CallbackInfo& info)
    {
        return Napi::Number::New(info.Env(), m_data ? m_data->Duration() : 0.0);
    }

    Napi::Value NativeAudioBuffer::GetNumberOfChannels(const Napi::CallbackInfo& info)
    {
        return Napi::Number::New(info.Env(), m_data ? m_data->numberOfChannels : 0);
    }

    Napi::Value NativeAudioBuffer::GetChannelData(const Napi::CallbackInfo& info)
    {
        const uint32_t channel = info[0].As<Napi::Number>().Uint32Value();
        if (!m_data || channel >= m_data->numberOfChannels)
        {
            throw Napi::Error::New(info.Env(), "IndexSizeError: channel index out of range");
        }

        auto& channelData = m_data->channels[channel];
        return Napi::Float32Array::New(info.Env(), channelData.size(), Napi::ArrayBuffer::New(info.Env(), channelData.data(), channelData.size() * sizeof(float), [data = m_data](Napi::Env, void*) {}), 0);
    }

    void NativeAudioBuffer::CopyFromChannel(const Napi::CallbackInfo& info)
    {
        auto destination = info[0].As<Napi::Float32Array>();
        const uint32_t channel = info[1].As<Napi::Number>().Uint32Value();
        const uint32_t startInChannel = info.Length() > 2 ? info[2].As<Napi::Number>().Uint32Value() : 0;

        if (!m_data || channel >= m_data->numberOfChannels)
        {
            throw Napi::Error::New(info.Env(), "IndexSizeError: channel index out of range");
        }

        const auto& source = m_data->channels[channel];
        const size_t count = std::min(static_cast<size_t>(destination.ElementLength()), source.size() > startInChannel ? source.size() - startInChannel : 0);
        if (count > 0)
        {
            std::memcpy(destination.Data(), source.data() + startInChannel, count * sizeof(float));
        }
    }

    void NativeAudioBuffer::CopyToChannel(const Napi::CallbackInfo& info)
    {
        auto source = info[0].As<Napi::Float32Array>();
        const uint32_t channel = info[1].As<Napi::Number>().Uint32Value();
        const uint32_t startInChannel = info.Length() > 2 ? info[2].As<Napi::Number>().Uint32Value() : 0;

        if (!m_data || channel >= m_data->numberOfChannels)
        {
            throw Napi::Error::New(info.Env(), "IndexSizeError: channel index out of range");
        }

        auto& destination = m_data->channels[channel];
        const size_t count = std::min(static_cast<size_t>(source.ElementLength()), destination.size() > startInChannel ? destination.size() - startInChannel : 0);
        if (count > 0)
        {
            std::memcpy(destination.data() + startInChannel, source.Data(), count * sizeof(float));
        }
    }
}
