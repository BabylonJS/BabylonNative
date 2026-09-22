#pragma once

#include <napi/env.h>
#include <Babylon/Api.h>

namespace Babylon::Plugins::NativeDraco
{
    // Exposes the versioned `_native.DracoCodec` API and the compatible
    // `_native.decodeDracoMesh` / `_native.encodeDracoMesh` entry points.
    void BABYLON_API Initialize(Napi::Env env);
}
