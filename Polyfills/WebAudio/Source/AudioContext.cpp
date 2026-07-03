#include "AudioContext.h"
#include "AnalyserNode.h"
#include "AudioBuffer.h"
#include "AudioBufferSourceNode.h"
#include "AudioDestinationNode.h"
#include "AudioEngine.h"
#include "AudioListener.h"
#include "GainNode.h"
#include "PannerNode.h"
#include "StereoPannerNode.h"

#include <Babylon/JsRuntime.h>
#include <Babylon/JsRuntimeScheduler.h>

#include <arcana/threading/task.h>
#include <arcana/threading/task_schedulers.h>

#include <miniaudio.h>

#include <vector>

namespace Babylon::Polyfills::Internal
{
    void NativeAudioContext::Initialize(Napi::Env env)
    {
        Napi::HandleScope scope{env};

        Napi::Function func = DefineClass(
            env,
            JS_CONSTRUCTOR_NAME,
            {
                InstanceAccessor("destination", &NativeAudioContext::GetDestination, nullptr),
                InstanceAccessor("listener", &NativeAudioContext::GetListener, nullptr),
                InstanceAccessor("state", &NativeAudioContext::GetState, nullptr),
                InstanceAccessor("sampleRate", &NativeAudioContext::GetSampleRate, nullptr),
                InstanceAccessor("currentTime", &NativeAudioContext::GetCurrentTime, nullptr),
                InstanceMethod("resume", &NativeAudioContext::Resume),
                InstanceMethod("suspend", &NativeAudioContext::Suspend),
                InstanceMethod("close", &NativeAudioContext::Close),
                InstanceMethod("decodeAudioData", &NativeAudioContext::DecodeAudioData),
                InstanceMethod("createGain", &NativeAudioContext::CreateGain),
                InstanceMethod("createBufferSource", &NativeAudioContext::CreateBufferSource),
                InstanceMethod("createPanner", &NativeAudioContext::CreatePanner),
                InstanceMethod("createStereoPanner", &NativeAudioContext::CreateStereoPanner),
                InstanceMethod("createAnalyser", &NativeAudioContext::CreateAnalyser),
                InstanceMethod("createBuffer", &NativeAudioContext::CreateBuffer),
                InstanceMethod("addEventListener", &NativeAudioContext::AddEventListener),
                InstanceMethod("removeEventListener", &NativeAudioContext::RemoveEventListener),
            });

        JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_CONSTRUCTOR_NAME, func);
        env.Global().Set(JS_CONSTRUCTOR_NAME, func);
        env.Global().Set("webkitAudioContext", func);
    }

    NativeAudioContext::NativeAudioContext(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<NativeAudioContext>{info}
    {
        InitializeContext();
    }

    NativeAudioContext::~NativeAudioContext()
    {
        if (m_state && m_state->engine)
        {
            m_state->engine->UnregisterContext(m_state.get());
        }
    }

    void NativeAudioContext::InitializeContext()
    {
        m_state = std::make_shared<AudioContextState>();
        m_state->engine = GetAudioEngineFromJavaScript(Env());
        m_state->destination = std::make_shared<DestinationNodeState>();
        m_state->destination->context = m_state;
        m_state->running.store(false);
        m_state->state = "suspended";

        if (m_state->engine)
        {
            m_state->engine->RegisterContext(m_state);
        }

        m_destination = Napi::Persistent(NativeAudioDestinationNode::CreateInstance(Env(), std::static_pointer_cast<DestinationNodeState>(m_state->destination)));
        m_listener = Napi::Persistent(NativeAudioListener::CreateInstance(Env(), m_state->listener, MakeCurrentTimeGetter(m_state)));
    }

    std::shared_ptr<AudioContextState> NativeAudioContext::State() const
    {
        return m_state;
    }

    Napi::Value NativeAudioContext::GetDestination(const Napi::CallbackInfo&)
    {
        return m_destination.Value();
    }

    Napi::Value NativeAudioContext::GetListener(const Napi::CallbackInfo&)
    {
        return m_listener.Value();
    }

    Napi::Value NativeAudioContext::GetState(const Napi::CallbackInfo& info)
    {
        return Napi::String::New(info.Env(), m_state ? m_state->state : "closed");
    }

    Napi::Value NativeAudioContext::GetSampleRate(const Napi::CallbackInfo& info)
    {
        const uint32_t sampleRate = (m_state && m_state->engine) ? m_state->engine->SampleRate() : 48000;
        return Napi::Number::New(info.Env(), sampleRate);
    }

    Napi::Value NativeAudioContext::GetCurrentTime(const Napi::CallbackInfo& info)
    {
        const double currentTime = (m_state && m_state->engine) ? m_state->engine->CurrentTime() : 0.0;
        return Napi::Number::New(info.Env(), currentTime);
    }

    void NativeAudioContext::SetState(const std::string& state)
    {
        if (!m_state || m_state->state == state)
        {
            return;
        }

        m_state->state = state;
        m_state->running.store(state == "running");
        if (m_state->engine)
        {
            m_state->engine->OnContextRunningChanged();
        }
        m_eventTarget.DispatchEvent(Env(), "statechange");
    }

    Napi::Value NativeAudioContext::Resume(const Napi::CallbackInfo& info)
    {
        auto deferred = Napi::Promise::Deferred::New(info.Env());
        if (m_state && m_state->state != "closed")
        {
            if (m_state->engine && m_state->engine->EnsureDeviceRunning())
            {
                SetState("running");
            }
            else
            {
                SetState("suspended");
            }
        }
        deferred.Resolve(info.Env().Undefined());
        return deferred.Promise();
    }

    Napi::Value NativeAudioContext::Suspend(const Napi::CallbackInfo& info)
    {
        auto deferred = Napi::Promise::Deferred::New(info.Env());
        if (m_state && m_state->state != "closed")
        {
            SetState("suspended");
        }
        deferred.Resolve(info.Env().Undefined());
        return deferred.Promise();
    }

    Napi::Value NativeAudioContext::Close(const Napi::CallbackInfo& info)
    {
        auto deferred = Napi::Promise::Deferred::New(info.Env());
        if (m_state)
        {
            SetState("closed");
            if (m_state->engine)
            {
                m_state->engine->UnregisterContext(m_state.get());
            }
        }
        deferred.Resolve(info.Env().Undefined());
        return deferred.Promise();
    }

    Napi::Value NativeAudioContext::DecodeAudioData(const Napi::CallbackInfo& info)
    {
        auto env = info.Env();
        auto deferred = Napi::Promise::Deferred::New(env);

        if (info.Length() < 1 || !info[0].IsArrayBuffer())
        {
            deferred.Reject(Napi::Error::New(env, "decodeAudioData requires an ArrayBuffer").Value());
            return deferred.Promise();
        }

        auto arrayBuffer = info[0].As<Napi::ArrayBuffer>();
        std::vector<uint8_t> bytes(static_cast<uint8_t*>(arrayBuffer.Data()), static_cast<uint8_t*>(arrayBuffer.Data()) + arrayBuffer.ByteLength());

        auto runtimeScheduler = std::make_shared<JsRuntimeScheduler>(JsRuntime::GetFromJavaScript(env));

        arcana::make_task(arcana::threadpool_scheduler, arcana::cancellation::none(), [bytes = std::move(bytes)]() {
            auto data = std::make_shared<AudioBufferData>();

            ma_decoder_config config = ma_decoder_config_init(ma_format_f32, 0, 0);
            ma_decoder decoder;
            if (ma_decoder_init_memory(bytes.data(), bytes.size(), &config, &decoder) != MA_SUCCESS)
            {
                throw std::runtime_error("Unable to decode audio data");
            }

            ma_uint64 frameCount = 0;
            if (ma_decoder_get_length_in_pcm_frames(&decoder, &frameCount) != MA_SUCCESS)
            {
                ma_decoder_uninit(&decoder);
                throw std::runtime_error("Unable to read decoded audio length");
            }

            const uint32_t channels = decoder.outputChannels;
            const uint32_t sampleRate = decoder.outputSampleRate;
            std::vector<float> interleaved(static_cast<size_t>(frameCount) * channels);
            ma_uint64 framesRead = 0;
            if (ma_decoder_read_pcm_frames(&decoder, interleaved.data(), frameCount, &framesRead) != MA_SUCCESS)
            {
                ma_decoder_uninit(&decoder);
                throw std::runtime_error("Unable to read decoded audio frames");
            }
            ma_decoder_uninit(&decoder);

            data->sampleRate = sampleRate;
            data->numberOfChannels = channels;
            data->length = static_cast<uint32_t>(framesRead);
            data->channels.assign(channels, std::vector<float>(data->length));

            for (uint32_t frame = 0; frame < data->length; ++frame)
            {
                for (uint32_t channel = 0; channel < channels; ++channel)
                {
                    data->channels[channel][frame] = interleaved[frame * channels + channel];
                }
            }

            return data;
        }).then(*runtimeScheduler, arcana::cancellation::none(), [env, deferred, runtimeScheduler](const arcana::expected<std::shared_ptr<AudioBufferData>, std::exception_ptr>& result) {
            if (result.has_error())
            {
                deferred.Reject(Napi::Error::New(env, result.error()).Value());
                return;
            }

            deferred.Resolve(NativeAudioBuffer::CreateInstance(env, result.value()));
        });

        return deferred.Promise();
    }

    Napi::Value NativeAudioContext::CreateGain(const Napi::CallbackInfo& info)
    {
        auto object = NativeGainNode::CreateInstance(info.Env(), m_state);
        object.Set("context", info.This());
        return object;
    }

    Napi::Value NativeAudioContext::CreateBufferSource(const Napi::CallbackInfo& info)
    {
        auto object = NativeAudioBufferSourceNode::CreateInstance(info.Env(), m_state);
        object.Set("context", info.This());
        return object;
    }

    Napi::Value NativeAudioContext::CreatePanner(const Napi::CallbackInfo& info)
    {
        auto object = NativePannerNode::CreateInstance(info.Env(), m_state);
        object.Set("context", info.This());
        return object;
    }

    Napi::Value NativeAudioContext::CreateStereoPanner(const Napi::CallbackInfo& info)
    {
        auto object = NativeStereoPannerNode::CreateInstance(info.Env(), m_state);
        object.Set("context", info.This());
        return object;
    }

    Napi::Value NativeAudioContext::CreateAnalyser(const Napi::CallbackInfo& info)
    {
        auto object = NativeAnalyserNode::CreateInstance(info.Env(), m_state);
        object.Set("context", info.This());
        return object;
    }

    Napi::Value NativeAudioContext::CreateBuffer(const Napi::CallbackInfo& info)
    {
        const uint32_t numberOfChannels = info[0].As<Napi::Number>().Uint32Value();
        const uint32_t length = info[1].As<Napi::Number>().Uint32Value();
        const uint32_t sampleRate = info[2].As<Napi::Number>().Uint32Value();

        auto data = std::make_shared<AudioBufferData>();
        data->numberOfChannels = numberOfChannels;
        data->length = length;
        data->sampleRate = sampleRate;
        data->channels.assign(numberOfChannels, std::vector<float>(length, 0.f));
        return NativeAudioBuffer::CreateInstance(info.Env(), data);
    }

    void NativeAudioContext::AddEventListener(const Napi::CallbackInfo& info)
    {
        m_eventTarget.AddEventListener(info);
    }

    void NativeAudioContext::RemoveEventListener(const Napi::CallbackInfo& info)
    {
        m_eventTarget.RemoveEventListener(info);
    }
}
