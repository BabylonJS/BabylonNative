#pragma once

#include <Babylon/Api.h>
#include <napi/env.h>

namespace Babylon::Polyfills::IndexedDB
{
    // Installs an in-memory IndexedDB implementation when the selected
    // JavaScript engine does not already provide one.
    void BABYLON_API Initialize(Napi::Env env);
}
