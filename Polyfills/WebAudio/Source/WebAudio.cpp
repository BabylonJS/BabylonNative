#include <Babylon/Polyfills/WebAudio.h>

#include "AnalyserNode.h"
#include "AudioBuffer.h"
#include "AudioBufferSourceNode.h"
#include "AudioContext.h"
#include "AudioDestinationNode.h"
#include "AudioEngine.h"
#include "AudioListener.h"
#include "AudioParam.h"
#include "GainNode.h"
#include "HTMLAudioElement.h"
#include "PannerNode.h"
#include "StereoPannerNode.h"

#include <Babylon/JsRuntime.h>

namespace
{
    constexpr auto JS_WEBAUDIO_NAME = "_WebAudioImpl";
}

namespace Babylon::Polyfills
{
    class WebAudio::Impl final : public std::enable_shared_from_this<WebAudio::Impl>
    {
    public:
        explicit Impl(Napi::Env env)
            : m_engine{Internal::AudioEngine::Create(JsRuntime::GetFromJavaScript(env))}
        {
            JsRuntime::NativeObject::GetFromJavaScript(env)
                .Set(JS_WEBAUDIO_NAME, Napi::External<WebAudio::Impl>::New(env, this));
        }

        static Impl& GetFromJavaScript(Napi::Env env)
        {
            return *JsRuntime::NativeObject::GetFromJavaScript(env)
                        .Get(JS_WEBAUDIO_NAME)
                        .As<Napi::External<WebAudio::Impl>>()
                        .Data();
        }

        void SuspendPlayback();
        void ResumePlayback();
        void ShutdownPlayback();

        const std::shared_ptr<Internal::AudioEngine>& Engine() const
        {
            return m_engine;
        }

    private:
        std::shared_ptr<Internal::AudioEngine> m_engine{};
    };

    void WebAudio::Impl::SuspendPlayback()
    {
        if (m_engine)
        {
            m_engine->SuspendDevice();
            m_engine->SetClockSuspended(true);
        }
    }

    void WebAudio::Impl::ShutdownPlayback()
    {
        if (m_engine)
        {
            m_engine->PrepareForShutdown();
        }
    }

    void WebAudio::Impl::ResumePlayback()
    {
        if (m_engine && m_engine->HasRunningContext())
        {
            m_engine->EnsureDeviceRunning();
            m_engine->ResumeDevice();
            m_engine->SetClockSuspended(false);
        }
    }

    namespace
    {
        Napi::Value DocumentCreateElement(const Napi::CallbackInfo& info)
        {
            if (info.Length() > 0 && info[0].IsString() && info[0].As<Napi::String>().Utf8Value() == "audio")
            {
                auto audioCtor = info.Env().Global().Get("Audio").As<Napi::Function>();
                return audioCtor.New({});
            }

            return Napi::Object::New(info.Env());
        }

        void InstallDocumentStub(Napi::Env env)
        {
            if (env.Global().Has("document"))
            {
                return;
            }

            auto document = Napi::Object::New(env);
            document.Set(
                "addEventListener",
                Napi::Function::New(env, [](const Napi::CallbackInfo&) {}, "addEventListener"));
            document.Set(
                "removeEventListener",
                Napi::Function::New(env, [](const Napi::CallbackInfo&) {}, "removeEventListener"));
            document.Set("createElement", Napi::Function::New(env, DocumentCreateElement, "createElement"));
            env.Global().Set("document", document);
        }
    }
}

// Expose Impl lookup to internal translation units.
namespace Babylon::Polyfills::Internal
{
    std::shared_ptr<AudioEngine> GetAudioEngineFromJavaScript(Napi::Env env)
    {
        return WebAudio::Impl::GetFromJavaScript(env).Engine();
    }
}

namespace Babylon::Polyfills
{

    WebAudio::WebAudio(std::shared_ptr<Impl> impl)
        : m_impl{std::move(impl)}
    {
    }

    WebAudio::~WebAudio()
    {
        ShutdownPlayback();
    }

    WebAudio BABYLON_API WebAudio::Initialize(Napi::Env env)
    {
        auto impl{std::make_shared<WebAudio::Impl>(env)};

        Internal::NativeAudioParam::Initialize(env);
        Internal::NativeAudioBuffer::Initialize(env);
        Internal::NativeAudioListener::Initialize(env);
        Internal::NativeAudioDestinationNode::Initialize(env);
        Internal::NativeGainNode::Initialize(env);
        Internal::NativePannerNode::Initialize(env);
        Internal::NativeStereoPannerNode::Initialize(env);
        Internal::NativeAnalyserNode::Initialize(env);
        Internal::NativeAudioBufferSourceNode::Initialize(env);
        Internal::NativeAudioContext::Initialize(env);
        Internal::NativeHTMLAudioElement::Initialize(env);
        InstallDocumentStub(env);

        return {impl};
    }

    void WebAudio::SuspendPlayback() const
    {
        if (m_impl)
        {
            m_impl->SuspendPlayback();
        }
    }

    void WebAudio::ResumePlayback() const
    {
        if (m_impl)
        {
            m_impl->ResumePlayback();
        }
    }

    void WebAudio::ShutdownPlayback() const
    {
        if (m_impl)
        {
            m_impl->ShutdownPlayback();
        }
    }
}
