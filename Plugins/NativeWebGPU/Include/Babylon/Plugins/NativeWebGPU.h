#pragma once

#include <Babylon/Api.h>

#include <napi/env.h>

namespace Babylon::Plugins::NativeWebGPU
{
    void BABYLON_API Initialize(Napi::Env env);
}
