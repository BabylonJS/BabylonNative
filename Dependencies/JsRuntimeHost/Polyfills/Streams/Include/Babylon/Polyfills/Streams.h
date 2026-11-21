#pragma once

#include <Babylon/Api.h>
#include <napi/env.h>

namespace Babylon::Polyfills::Streams
{
    // Installs the WHATWG Streams constructors that are not already supplied
    // by the JavaScript engine.
    void BABYLON_API Initialize(Napi::Env env);
}
