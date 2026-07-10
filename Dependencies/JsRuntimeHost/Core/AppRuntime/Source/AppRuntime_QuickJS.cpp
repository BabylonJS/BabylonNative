#include "AppRuntime.h"
#include <napi/env.h>

#ifdef _WIN32
#pragma warning(push)
// cast from int64 to int32
#pragma warning(disable : 4244)
#endif
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshorten-64-to-32"
#endif
#include <quickjs.h>
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
#ifdef _WIN32
#pragma warning(pop)
#endif

namespace Babylon
{
    void AppRuntime::RunEnvironmentTier(const char* /*executablePath*/)
    {
        // Create the runtime.
        JSRuntime* runtime = JS_NewRuntime();
        if (!runtime)
        {
            throw std::runtime_error{"Failed to create QuickJS runtime"};
        }

        // Create the context.
        JSContext* context = JS_NewContext(runtime);
        if (!context)
        {
            JS_FreeRuntime(runtime);
            throw std::runtime_error{"Failed to create QuickJS context"};
        }

        // Use the context within a scope.
        {
            Napi::Env env = Napi::Attach(context);

            Run(env);

            Napi::Detach(env);
        }

        // Destroy the context and runtime.
        JS_FreeContext(context);
        JS_FreeRuntime(runtime);
    }

    void AppRuntime::DrainMicrotasks(Napi::Env env)
    {
        // QuickJS does not auto-drain its job queue. Promise continuations,
        // queueMicrotask callbacks, etc. are queued as "pending jobs" and only
        // run when explicitly pumped. We drain them here, after each user
        // callback, so async code observes the same "between turns" semantics
        // it gets on the auto-draining engines (V8/Chakra/JSC).
        JSRuntime* runtime = JS_GetRuntime(Napi::GetContext(env));
        JSContext* pending_ctx;
        while (JS_ExecutePendingJob(runtime, &pending_ctx) > 0)
        {
        }
    }
}
