#include "ChromeDevToolsImpl.h"

#include <V8InspectorAgent.h>
#include <Babylon/JsRuntime.h>

namespace Babylon::Plugins::Internal
{
    class ChromeDevToolsV8
    {
    public:
        ChromeDevToolsV8(Napi::Env env)
            : m_env{env}
        {
            JsRuntime::GetFromJavaScript(env).Dispatch([this](Napi::Env env) {
                auto& v8Platform = *JsRuntime::NativeObject::GetFromJavaScript(env).Get("_V8Platform").As<Napi::External<v8::Platform>>().Data();
                auto v8Isolate = v8::Isolate::GetCurrent();
                auto v8Context = v8Isolate->GetCurrentContext();
                m_inspector = std::make_unique<Babylon::V8InspectorAgent>(v8Platform, v8Isolate, v8Context, "BabylonNative");
            });
        }

        bool SupportsInspector()
        {
            return true;
        }

        void StartInspector(const unsigned short port, const std::string& appName)
        {
            JsRuntime::GetFromJavaScript(m_env).Dispatch([this, port, appName](Napi::Env) {
                if (m_inspector == nullptr)
                {
                    throw std::exception("The inspector hasn't been initialized, call Babylon::Plugins::ChromeDevTools::Initialize.");
                }

                if (m_inspector->IsStarted())
                {
                    StopInspector();
                }

                m_inspector->start(port, appName);
            });
        }

        void StopInspector()
        {
            JsRuntime::GetFromJavaScript(m_env).Dispatch([this](Napi::Env) {
                m_inspector->stop();
            });
        }
    protected:
        std::unique_ptr<Babylon::V8InspectorAgent> m_inspector{};
        Napi::Env m_env;
    };

    class ChromeDevTools::Impl : public ChromeDevToolsV8
    {
        public:
            Impl(const Napi::Env& env) 
                : ChromeDevToolsV8(env)
            {
            }
    };

    ChromeDevTools::ChromeDevTools(Napi::Env env)
        : m_impl{std::make_unique<ChromeDevTools::Impl>(env)}
    {
    }

    std::unique_ptr<ChromeDevTools> ChromeDevTools::s_instance = nullptr;

    void ChromeDevTools::Initialize(Napi::Env env)
    {
        s_instance = std::make_unique<ChromeDevTools>(env);
    }

    ChromeDevTools* ChromeDevTools::GetInstance()
    {
        return s_instance.get();
    }

    bool ChromeDevTools::SupportsInspector()
    {
        return m_impl->SupportsInspector();
    }

    void ChromeDevTools::StartInspector(const unsigned short port, const std::string& appName)
    {
        m_impl->StartInspector(port, appName);
    }

    void ChromeDevTools::StopInspector()
    {
        m_impl->StopInspector();
    }
}
