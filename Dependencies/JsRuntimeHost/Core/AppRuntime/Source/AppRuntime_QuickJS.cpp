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

#include <stdexcept>
#include <cstddef>

#if defined(__ANDROID__)
#include <pthread.h>
#include <exception>
#include <functional>
#include <utility>
#endif

namespace Babylon
{
    namespace
    {
        // Runs the QuickJS environment on the calling thread. QuickJS's interpreter recurses in C
        // (one JS_CallInternal frame per JS call), so deep JS call stacks need a comparably deep C
        // stack; QuickJS's own limit (JS_DEFAULT_STACK_SIZE, 1 MiB) guards against overrun.
        // jsStackLimit: value for JS_SetMaxStackSize, or 0 to keep QuickJS's default (1 MiB). A
        // non-default limit is only safe when the caller guarantees a stack large enough to hold it
        // below the guard page -- see the Android nested-thread path below.
        void RunQuickJSEnvironment(AppRuntime& appRuntime, void (AppRuntime::*run)(Napi::Env), size_t jsStackLimit)
        {
            JSRuntime* runtime = JS_NewRuntime();
            if (!runtime)
            {
                throw std::runtime_error{"Failed to create QuickJS runtime"};
            }
            if (jsStackLimit != 0)
            {
                JS_SetMaxStackSize(runtime, jsStackLimit);
            }

            JSContext* context = JS_NewContext(runtime);
            if (!context)
            {
                JS_FreeRuntime(runtime);
                throw std::runtime_error{"Failed to create QuickJS context"};
            }

            {
                Napi::Env env = Napi::Attach(context);
                (appRuntime.*run)(env);
                Napi::Detach(env);
            }

            JS_FreeContext(context);
            JS_FreeRuntime(runtime);
        }
    }

    void AppRuntime::RunEnvironmentTier(const char* /*executablePath*/)
    {
#if defined(__ANDROID__)
        // bionic gives this worker thread ~1 MiB of stack, at or below QuickJS's default 1 MiB
        // recursion limit -- so deep-but-legal JS recursion faults the guard page (SIGSEGV) before
        // QuickJS can raise a catchable "stack overflow", while clamping the limit below 1 MiB
        // instead rejects call depths that every other engine (and desktop QuickJS on its ~8 MiB
        // stack) accepts. Run the environment on a nested thread with a desktop-sized stack so
        // QuickJS's raised limit sits safely below the guard page and deep recursion fits. The
        // stack must be generous because connectedAndroidTest is an unoptimized Debug build, whose
        // JS_CallInternal frames are several times larger than a release build's -- depth-128
        // recursion (which release QuickJS clears within the 1 MiB default) needs well over 1 MiB
        // here, so QuickJS's default limit would still reject it on any thread size.
        constexpr size_t NestedStackSize{8 * 1024 * 1024};
        constexpr size_t JsStackLimit{6 * 1024 * 1024}; // < NestedStackSize guard; > debug depth-128 need
        std::function<void()> body{[this] { RunQuickJSEnvironment(*this, &AppRuntime::Run, JsStackLimit); }};
        std::exception_ptr thrown{};
        auto payload = std::make_pair(&body, &thrown);
        auto trampoline = [](void* arg) -> void* {
            auto* p = static_cast<std::pair<std::function<void()>*, std::exception_ptr*>*>(arg);
            try
            {
                (*p->first)();
            }
            catch (...)
            {
                *p->second = std::current_exception();
            }
            return nullptr;
        };

        pthread_attr_t attr;
        if (pthread_attr_init(&attr) == 0)
        {
            pthread_attr_setstacksize(&attr, NestedStackSize);
            pthread_t tid{};
            const int created = pthread_create(&tid, &attr, trampoline, &payload);
            pthread_attr_destroy(&attr);
            if (created == 0)
            {
                pthread_join(tid, nullptr);
                if (thrown)
                {
                    std::rethrow_exception(thrown);
                }
                return;
            }
        }
        // Thread creation failed: fall back to the current (small) worker thread, where only
        // QuickJS's default limit is safe.
        RunQuickJSEnvironment(*this, &AppRuntime::Run, 0);
#else
        RunQuickJSEnvironment(*this, &AppRuntime::Run, 0);
#endif
    }

    void AppRuntime::ShutdownEnvironment(Napi::Env)
    {
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

    void AppRuntime::RunDispatchLoop()
    {
        RunDispatcher();
    }
}
