#pragma once

#include <Babylon/Api.h>
#include <napi/env.h>

namespace Babylon::Polyfills::Compression
{
    // Installs CompressionStream and DecompressionStream when the host does
    // not already provide them. The Streams polyfill is initialized first.
    void BABYLON_API Initialize(Napi::Env env);
}
