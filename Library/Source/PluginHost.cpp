#include <Babylon/PluginHost.h>
#include <napi/env.h>
#include "RuntimeImpl.h"

namespace Babylon
{
    PluginHost& PluginHost::GetRuntimeImplFromJavaScript(Napi::Env env)
    {
        return static_cast<PluginHost&>(RuntimeImpl::GetRuntimeImplFromJavaScript(env));
    }
}