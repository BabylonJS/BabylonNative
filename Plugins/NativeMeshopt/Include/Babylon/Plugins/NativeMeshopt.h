#pragma once

#include <napi/env.h>
#include <Babylon/Api.h>

namespace Babylon::Plugins::NativeMeshopt
{
    // Exposes `_native.decodeMeshopt(source, count, stride, mode, filter?)`, a
    // synchronous native replacement for Babylon's WebAssembly meshopt decoder
    // (zeux/meshoptimizer). This is a compatibility export: the pinned package
    // and the current public MeshoptCompression implementation do not
    // reference it and use the script-based decoder.
    void BABYLON_API Initialize(Napi::Env env);
}
