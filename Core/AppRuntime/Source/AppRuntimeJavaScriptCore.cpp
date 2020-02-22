#include "AppRuntime.h"

#include <JavaScriptCore/JavaScriptCore.h>

namespace Babylon
{
    void AppRuntime::CreateEnvironmentAndRun()
    {
        auto contextGroup = JSContextGroupCreate();
        auto globalContext = JSGlobalContextCreateInGroup(contextGroup, nullptr);

        Napi::Env env = Napi::Attach(globalContext);
        Run(env);
        Napi::Detach(env);

        JSGlobalContextRelease(globalContext);
        JSContextGroupRelease(contextGroup);
    }
}
