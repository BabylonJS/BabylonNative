#include <Babylon/Plugins/NativeAudio.h>

#include <Babylon/JsRuntime.h>
#include <LabSound/LabSound.h>

namespace Babylon::Plugins::Internal
{
    class AudioContext : public Napi::ObjectWrap<AudioContext>
    {
    public:
        static constexpr auto JS_CLASS_NAME = "AudioContext";
        static void Initialize(Napi::Env env)
        {
            Napi::HandleScope scope{env};
            Napi::Function func = AudioContext::DefineClass(
                env,
                JS_CLASS_NAME,
                {
                    // methods we want the javascript object to implement.
                    AudioContext::InstanceMethod("createGain", &AudioContext::CreateGain),
                    AudioContext::InstanceMethod("resume", &AudioContext::Resume),
                    AudioContext::InstanceMethod("createMediaElementSource", &AudioContext::CreateMediaElementSource),
                    AudioContext::InstanceMethod("createBufferSource", &AudioContext::CreateBufferSource),
                    AudioContext::InstanceMethod("createPanner", &AudioContext::CreatePanner),

                    AudioContext::InstanceMethod("decodeAudioData", &AudioContext::DecodeAudioData),
                    AudioContext::InstanceAccessor("destination", &AudioContext::Destination, nullptr),
                    AudioContext::InstanceAccessor("state", &AudioContext::State, nullptr),
                    AudioContext::InstanceAccessor("currentTime", &AudioContext::CurrentTime, nullptr)
                });
        }

        AudioContext(const Napi::CallbackInfo& info)
            : Napi::ObjectWrap<AudioContext>{info}
            , m_audioContext{lab::MakeRealtimeAudioContext(lab::GetDefaultOutputAudioDeviceConfiguration(), lab::GetDefaultInputAudioDeviceConfiguration())}
        {
        }

        ~AudioContext()
        {
        }
        
        Napi::Value CreatePanner(const Napi::CallbackInfo& info)
        {
            // call to JS environment to create PannerNode object, and return it.
        }
        
        Napi::Value CreateBufferSource(const Napi::CallbackInfo& info)
        {
            // call to JS environment to create AudioBufferNode object, and return it.
        }

        Napi::Value CreateMediaElementSource(const Napi::CallbackInfo& info)
        {
            // call to JS environment to create MediaElementSourceNode object, and return it.
        }

        Napi::Value CreateGain(const Napi::CallbackInfo& info)
        {
            // call to JS environment to create gain node object, and return it.
        }

        Napi::Value DecodeAudioData(const Napi::CallbackInfo& info) 
        {
            // TODO: Implement 
        }

        Napi::Value Resume(const Napi::CallbackInfo& info)
        {
            // TODO: we need to implement audio context suspension + resuming.
        }

        Napi::Value Destination(const Napi::CallbackInfo& info) {
            return m_destination ? m_destination.Value() : Napi::Value();
        }

        Napi::Value State(const Napi::CallbackInfo& info)
        {
            //TODO we need to implement state, for now stub out as always running.
            return Napi::String::From(info.Env(), std::string("running"));
        }

        Napi::Value CurrentTime(const Napi::CallbackInfo& info)
        {
            return Napi::Number::From(info.Env(), m_audioContext->currentTime());
        }


    private:
        std::unique_ptr<lab::AudioContext> m_audioContext;
        Napi::ObjectReference m_destination;
    
    };

    class MediaElementAudioSourceNode : public Napi::ObjectWrap<MediaElementAudioSourceNode>
    {

    };

    class AudioBufferNode : public Napi::ObjectWrap<AudioBufferNode>
    {
        //TODO implement the currently-used portions of this...
    };

    class PannerNode : public Napi::ObjectWrap<PannerNode>
    {
        //TODO implement the currently-used portions of this...
    };

    class GainNode : public Napi::ObjectWrap<GainNode>
    {
        //TODO implement the currently-used portions of this...
    };


    class NativeAudio : public Napi::ObjectWrap<NativeAudio>
    {
    public:
        static constexpr auto JS_CLASS_NAME = "NativeAudio";

        static void Initialize(Napi::Env env)
        {
            Napi::HandleScope scope{env};

            Napi::Function func = NativeAudio::DefineClass(
                env,
                JS_CLASS_NAME,
                {
                    NativeAudio::InstanceMethod("addCallback", &NativeAudio::AddCallback),
                    NativeAudio::InstanceMethod("dispose", &NativeAudio::Dispose),
                });

            env.Global().Set(JS_CLASS_NAME, func);
        }

        NativeAudio(const Napi::CallbackInfo& info)
            : Napi::ObjectWrap<NativeAudio>{info}
            , m_runtime{JsRuntime::GetFromJavaScript(info.Env())}
            , m_jsData{Napi::Persistent(Napi::Object::New(info.Env()))}
        {
            Napi::Object jsData = m_jsData.Value();
            jsData.Set("data", Napi::ArrayBuffer::New(info.Env(), 0));


        }

        ~NativeAudio()
        {
        }

    private:
        void AddCallback(const Napi::CallbackInfo& info)
        {
        }

        void Dispose()
        {
        }

        void Dispose(const Napi::CallbackInfo&)
        {
            Dispose();
        }

        JsRuntime& m_runtime;
        Napi::ObjectReference m_jsData{};
    };
}


namespace Babylon::Plugins::NativeAudio
{
    void Initialize(Napi::Env env)
    {
        Babylon::Plugins::Internal::NativeAudio::Initialize(env);
    }
}