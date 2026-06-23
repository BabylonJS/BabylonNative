#include "AppRuntime.h"
#include <napi/env.h>

namespace Babylon
{
    void AppRuntime::RunEnvironmentTier(const char*)
    {
        // All Hermes runtime + napi_env setup is encapsulated inside the napi
        // library's env_hermes.cc (see Napi::Attach/Detach).  Keeping the
        // engine-specific machinery there avoids dragging Hermes headers into
        // AppRuntime's translation unit.
        Napi::Env env = Napi::Attach();

        Run(env);

        Napi::Detach(env);
    }

    void AppRuntime::DrainMicrotasks(Napi::Env env)
    {
        // Hermes does not auto-drain its job queue.  Promise continuations,
        // queueMicrotask callbacks, and pending NAPI finalizers all run via
        // Runtime::drainJobs().  We pump it after each user callback so async
        // code (Promises, Mocha's async tests, polyfill schedulers, etc.)
        // observes the same "between turns" semantics it gets on V8/Chakra.
        Napi::DrainJobs(env);
    }
}
