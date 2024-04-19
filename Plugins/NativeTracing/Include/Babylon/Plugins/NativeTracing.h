#pragma once

#include <napi/env.h>
#include <Babylon/Api.h>

namespace Babylon::Plugins::NativeTracing
{
    void BABYLON_API Initialize(Napi::Env env);
}
