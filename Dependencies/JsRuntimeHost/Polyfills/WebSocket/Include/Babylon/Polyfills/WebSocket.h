#pragma once

#include <napi/env.h>
#include <Babylon/Api.h>

namespace Babylon::Polyfills::WebSocket
{
    void BABYLON_API Initialize(Napi::Env env);
}
