#pragma once

#include <Babylon/Api.h>
#include <napi/env.h>

#include <functional>
#include <string>

namespace Babylon::Plugins::DynamicScriptLoader
{
    // Return source as a JavaScript string or a Promise resolving to one.
    // Return null/undefined (or reject) when the resource is unavailable.
    // Called on the JavaScript thread; any asynchronous completion must also use
    // the runtime dispatcher before accessing Node-API values.
    using ResolverT = std::function<Napi::Value BABYLON_API (Napi::Env, const std::string&)>;

    // Installs _native.loadScript(name): Promise<void>. The resolver alone determines
    // which resources can be loaded; no file or network fallback is provided.
    // The host must stop asynchronous producers before destroying the runtime.
    void BABYLON_API Initialize(Napi::Env env, ResolverT resolver);
}
