#pragma once

#include <napi/env.h>
#include <Babylon/Api.h>

namespace Babylon::Plugins::NativeMeshopt
{
    // Exposes `_native.decodeMeshopt(source, count, stride, mode, filter?)`, a
    // synchronous native replacement for Babylon's WebAssembly meshopt decoder
    // (zeux/meshoptimizer). Babylon.js routes its MeshoptCompression to this
    // function when it is present.
    void BABYLON_API Initialize(Napi::Env env);
}
