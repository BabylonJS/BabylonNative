#include "ChromeDevTools.h"

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
        }

        Napi::Value AreSupported(const Napi::CallbackInfo& info)
        {
            return Napi::Boolean::From(info.Env(), false);
        }

        void StartInspector(const Napi::CallbackInfo &info)
        {
            throw Napi::Error::New(info.Env(), "This method is currently unsupported on this JavaScript engine.");
        }

        void StopInspector(const Napi::CallbackInfo &info)
        {
            throw Napi::Error::New(info.Env(), "This method is currently unsupported on this JavaScript engine.");
        }
    };
}

namespace Babylon::Plugins::ChromeDevTools
{
    void Initialize(Napi::Env env)
    {
        Babylon::Plugins::Internal::ChromeDevTools::Initialize(env);
    }
}