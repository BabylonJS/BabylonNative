#pragma once

#include <napi/env.h>
#include <Babylon/Graphics.h>

namespace Babylon::Plugins::TestUtils
{
    extern int errorCode;
    void Initialize(Napi::Env env, WindowType window);
}
