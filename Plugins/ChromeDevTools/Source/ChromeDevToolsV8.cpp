#include "ChromeDevToolsImpl.h"

#include <V8InspectorAgent.h>
#include <Babylon/JsRuntime.h>

namespace Babylon::Plugins::Internal
{
    class ChromeDevToolsV8
    {
    public:
        ChromeDevToolsV8(const Napi::Env& env)
        {
            JsRuntime::GetFromJavaScript(env).Dispatch([this](Napi::Env env) {
                auto& v8Platform = *JsRuntime::NativeObject::GetFromJavaScript(env).Get("_V8Platform").As<Napi::External<v8::Platform>>().Data();
                auto v8Isolate = v8::Isolate::GetCurrent();
                auto v8Context = v8Isolate->GetCurrentContext();
                m_inspector = std::make_unique<Babylon::V8InspectorAgent>(v8Platform, v8Isolate, v8Context, "BabylonNative");
            });
        }

        Napi::Value SupportsInspector(const Napi::CallbackInfo& info)
        {
            return Napi::Boolean::From(info.Env(), true);
        }

        void StartInspector(const Napi::CallbackInfo &info)
        {
            if (info.Length() == 0)
                throw Napi::Error::New(info.Env(), "You must at least specify a port to start the DevTools inspector.");

            if (m_inspector->IsStarted())
            {
                StopInspector();
            }

            const auto port = static_cast<unsigned short>(info[0].As<Napi::Number>().Int32Value());

            std::string appName;
            if (info.Length() > 1 && info[1].IsString())
                appName = info[1].As<Napi::String>().Utf8Value();
            
            m_inspector->start(port, appName);
        }

        void StopInspector(const Napi::CallbackInfo& info)
        {
            (void)info;
            StopInspector();
        }

        void StopInspector()
        {
            m_inspector->stop();
        }

        ~ChromeDevToolsV8()
        {
            if (m_inspector != nullptr)
                StopInspector();
        }
    protected:
        std::unique_ptr<Babylon::V8InspectorAgent> m_inspector = nullptr;
    };

    class ChromeDevTools::Impl : public ChromeDevToolsV8
    {
        public:
            Impl(const Napi::Env& env) 
                : ChromeDevToolsV8(env)
            {
            }
    };

    ChromeDevTools::ChromeDevTools(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<ChromeDevTools>{info}
        , m_impl{std::make_unique<ChromeDevTools::Impl>(info.Env())}
    {
    }
    
    Napi::Value ChromeDevTools::SupportsInspector(const Napi::CallbackInfo& info)
    {
        return m_impl->SupportsInspector(info);
    }

    void ChromeDevTools::StartInspector(const Napi::CallbackInfo& info)
    {
        m_impl->StartInspector(info);
    }

    void ChromeDevTools::StopInspector(const Napi::CallbackInfo& info)
    {
        m_impl->StopInspector(info);
    }
}