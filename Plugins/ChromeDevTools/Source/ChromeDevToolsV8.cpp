#include "ChromeDevTools.h"

#include <V8InspectorAgent.h>
#include <Babylon/JsRuntime.h>

namespace Babylon::Plugins::Internal
{
    class ChromeDevTools : public Napi::ObjectWrap<ChromeDevTools>
    {
    public:
        static constexpr auto JS_INSTANCE_NAME = "chromeDevTools";

        static void Initialize(Napi::Env env)
        {
            Napi::HandleScope scope{env};

            Napi::Function func = ChromeDevTools::DefineClass(
                env,
                JS_INSTANCE_NAME,
                {
                    ChromeDevTools::InstanceMethod("areSupported", &ChromeDevTools::AreSupported),
                    ChromeDevTools::InstanceMethod("startInspector", &ChromeDevTools::StartInspector),
                    ChromeDevTools::InstanceMethod("stopInspector", &ChromeDevTools::StopInspector),
                });
            auto singleton = func.New({});
            JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_INSTANCE_NAME, singleton);
        }

        ChromeDevTools(const Napi::CallbackInfo& info)
            : Napi::ObjectWrap<ChromeDevTools>{info}
        {
            JsRuntime::GetFromJavaScript(info.Env()).Dispatch([this](Napi::Env env) {
                auto& v8Platform = *JsRuntime::NativeObject::GetFromJavaScript(env).Get("_V8Platform").As<Napi::External<v8::Platform>>().Data();
                auto v8Isolate = v8::Isolate::GetCurrent();
                auto v8Context = v8Isolate->GetCurrentContext();
                m_inspector = std::make_unique<Babylon::V8InspectorAgent>(v8Platform, v8Isolate, v8Context, "BabylonNative");
            });
        }

        Napi::Value AreSupported(const Napi::CallbackInfo& info)
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

        ~ChromeDevTools()
        {
            if (m_inspector != nullptr)
                StopInspector();
        }

    private:
        std::unique_ptr<Babylon::V8InspectorAgent> m_inspector = nullptr;
    };
}

namespace Babylon::Plugins::ChromeDevTools
{
    void Initialize(Napi::Env env)
    {
        Babylon::Plugins::Internal::ChromeDevTools::Initialize(env);
    }
}