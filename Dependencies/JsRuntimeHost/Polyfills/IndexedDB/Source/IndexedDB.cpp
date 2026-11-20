#include <Babylon/Polyfills/IndexedDB.h>
#include <Babylon/Polyfills/Scheduling.h>

#include "IndexedDBScripts.h"

#include <string>

namespace Babylon::Polyfills::IndexedDB
{
    void BABYLON_API Initialize(Napi::Env env)
    {
        Napi::HandleScope scope{env};
        auto global = env.Global();
        const auto indexedDB = global.Get("indexedDB");
        if (!indexedDB.IsUndefined() && !indexedDB.IsNull())
        {
            return;
        }

        // ChakraCore predates globalThis. fake-indexeddb deliberately targets
        // that browser global, so provide the standard alias on older hosts
        // before evaluating the bundle while preserving any host definition.
        const auto globalThis = global.Get("globalThis");
        if (globalThis.IsUndefined() || globalThis.IsNull())
        {
            global.Set("globalThis", global);
        }

        // IndexedDB queues database work as tasks rather than microtasks.
        Scheduling::Initialize(env);
        std::string source;
        for (const auto part : Internal::IndexedDBScripts::PolyfillParts)
        {
            source.append(part.data(), part.size());
        }
        Napi::Eval(
            env,
            source.c_str(),
            "jsruntimehost://fake-indexeddb.js");
    }
}
