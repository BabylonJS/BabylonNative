#pragma once

#include <napi/env.h>
#include <Babylon/Api.h>

namespace Babylon::Plugins::NativeDraco
{
    // Exposes `_native.decodeDracoMesh(dataView, attributes?)`, a synchronous
    // native replacement for Babylon's WebAssembly Draco decoder. Babylon.js
    // routes its DracoDecoder to this function when it is present.
    void BABYLON_API Initialize(Napi::Env env);
}
