#pragma once

#include <napi/env.h>

namespace Babylon::Plugins::ChromeDevTools
{
    void Initialize(Napi::Env env);
    bool SupportsInspector();
    void StartInspector(const unsigned short port, const std::string& appName);
    void StopInspector();
}
