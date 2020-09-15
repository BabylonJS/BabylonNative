#pragma once

#include <Babylon/Graphics.h>

#include <napi/env.h>

namespace Babylon::Plugins::NativeEngine
{
    void Initialize(Napi::Env env, bool renderAutomatically = true);
}
