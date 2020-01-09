#pragma once

#include <functional>
#include <memory>
#include <string>

namespace Napi
{
    class Env;
}

namespace Babylon
{
    class PluginHost
    {
    public:
        static PluginHost& PluginHost::GetRuntimeImplFromJavaScript(Napi::Env env);
    };
}