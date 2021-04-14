#include "ChromeDevToolsImpl.h"

namespace Babylon::Plugins::Internal
{
    void ChromeDevTools::Initialize(Napi::Env env)
    {
        Napi::HandleScope scope{env};

        Napi::Function func = ChromeDevTools::DefineClass(
            env,
            "ChromeDevTools",
            {
                ChromeDevTools::InstanceMethod("supportsInspector", &ChromeDevTools::SupportsInspector),
                ChromeDevTools::InstanceMethod("startInspector", &ChromeDevTools::StartInspector),
                ChromeDevTools::InstanceMethod("stopInspector", &ChromeDevTools::StopInspector),
            });
        auto devTools = func.New({});
        env.Global().Set(JS_INSTANCE_NAME, devTools);
    }
}

namespace Babylon::Plugins::ChromeDevTools
{
    void Initialize(Napi::Env env)
    {
        Babylon::Plugins::Internal::ChromeDevTools::Initialize(env);
    }
}