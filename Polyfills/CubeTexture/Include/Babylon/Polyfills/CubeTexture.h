#pragma once

#include <napi/env.h>
#include <Babylon/Api.h>

namespace Babylon::Polyfills::CubeTexture
{
    // Patches BABYLON.NativeEngine.prototype.createCubeTexture to transparently
    // retry .dds / .ktx / .ktx2 single-URL cubemap loads as .env (a format BN
    // does support). Babylon's CDN co-hosts both, so the swap is invisible to
    // consumer JS. Falls back to the original (which throws "Cannot load
    // cubemap because 6 files were not defined") on 404 - preserving existing
    // error semantics.
    //
    // Call Initialize AFTER babylon.max.js has been evaluated; the patch
    // requires BABYLON.NativeEngine.prototype to exist. When using the
    // jsruntimehost ScriptLoader, the simplest pattern is:
    //
    //   scriptLoader.LoadScript("app:///Scripts/babylon.max.js");
    //   scriptLoader.Dispatch([](Napi::Env env) {
    //       Babylon::Polyfills::CubeTexture::Initialize(env);
    //   });
    //
    // The Dispatch queues onto the same ordered task chain as LoadScript, so
    // Initialize is guaranteed to run after babylon.max.js finishes evaluating.
    //
    // This is a tactical bridge until Plugins/NativeEngine wires a proper
    // loader registry path for cubemap formats. Safe to call multiple times;
    // the patch is idempotent.
    void BABYLON_API Initialize(Napi::Env env);
}
