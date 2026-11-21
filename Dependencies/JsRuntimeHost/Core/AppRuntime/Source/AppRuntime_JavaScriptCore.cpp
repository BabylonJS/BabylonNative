#include "AppRuntime.h"
#include <napi/env.h>

#if (!defined(__APPLE__) || defined(JSR_ENABLE_JSC_WATCHDOG)) && __has_include(<JavaScriptCore/JSContextRefPrivate.h>)
#include <JavaScriptCore/JSContextRefPrivate.h>
#define JSRUNTIMEHOST_HAS_JSC_EXECUTION_TIME_LIMIT 1
#elif (!defined(__APPLE__) && defined(__unix__)) || defined(JSR_ENABLE_JSC_WATCHDOG)
// Apple's SDK ships no JSContextRefPrivate.h either, but the system JavaScriptCore framework
// exports the same C ABI (it is SPI there); resolving it dynamically keeps Worker::terminate()
// able to stop a tight loop, exactly as on WebKitGTK. Without it a terminated worker that never
// yields keeps its thread forever and the finalizer's join deadlocks at teardown.
#include <dlfcn.h>
#define JSRUNTIMEHOST_LOOKUP_JSC_EXECUTION_TIME_LIMIT 1
#endif

namespace
{
#if defined(JSRUNTIMEHOST_HAS_JSC_EXECUTION_TIME_LIMIT) || \
    defined(JSRUNTIMEHOST_LOOKUP_JSC_EXECUTION_TIME_LIMIT)
    using ShouldTerminateCallback = bool (*)(JSContextRef, void*);
    using SetExecutionTimeLimit = void (*)(JSContextGroupRef, double, ShouldTerminateCallback, void*);
    using ClearExecutionTimeLimit = void (*)(JSContextGroupRef);

    // How often running script is polled for Terminate().
    constexpr double ExecutionTimeLimitSeconds{0.05};

    struct ExecutionWatchdog
    {
        Babylon::AppRuntime* Runtime{};
        SetExecutionTimeLimit Set{};
        ClearExecutionTimeLimit Clear{};
        ShouldTerminateCallback Callback{};

        bool Available() const
        {
            return Set != nullptr && Clear != nullptr;
        }

        // JavaScriptCore restarts the watchdog timer only when the callback sets a new limit
        // itself: its "callback did nothing" branch takes the CPU deadline that has just expired
        // for a timer the callback started and schedules nothing, so a single declined poll would
        // otherwise silence the watchdog until the running script returns on its own.
        void Rearm(JSContextRef context)
        {
            Set(JSContextGetGroup(context), ExecutionTimeLimitSeconds, Callback, this);
        }
    };
#endif
}

#if defined(__APPLE__)
#include <CoreFoundation/CoreFoundation.h>
#elif defined(JSRUNTIMEHOST_JAVASCRIPTCORE_GLIB)
#include <glib.h>
#include <future>
#include <mutex>
#include <thread>

namespace WTF
{
    // Exported by libjavascriptcoregtk but not declared in its public headers.
    void initializeMainThread();
}

namespace
{
    // JavaScriptCore's full collection timer uses WTF::MemoryPressureHandler, which on Linux
    // needs WTF's main RunLoop. WebKit creates it in WTF::initializeMainThread, which the
    // JavaScriptCore C API never calls, so the first timer-driven collection would crash.
    // initializeMainThread only has an effect once per process and binds the main RunLoop to
    // the calling thread, so it can't run on an AppRuntime thread that may exit while later
    // runtimes still use the RunLoop. Give it a dedicated thread that lives for the rest of
    // the process and keeps the RunLoop's GLib context pumped.
    void EnsureWtfMainRunLoop()
    {
        static std::once_flag once;
        std::call_once(once, [] {
            std::promise<void> initialized;
            std::thread{[&initialized] {
                // WTF's RunLoop adopts the thread-default context; use a private one rather
                // than the process-wide default context, which belongs to the application.
                GMainContext* context = g_main_context_new();
                g_main_context_push_thread_default(context);
                WTF::initializeMainThread();
                GMainLoop* loop = g_main_loop_new(context, FALSE);
                initialized.set_value();
                g_main_loop_run(loop);
            }}.detach();
            initialized.get_future().wait();
        });
    }
}
#endif

namespace Babylon
{
    void AppRuntime::RunEnvironmentTier(const char*)
    {
#ifdef JSRUNTIMEHOST_JAVASCRIPTCORE_GLIB
        EnsureWtfMainRunLoop();

        // The VM binds to this thread's WTF RunLoop, which adopts the thread-default GLib main
        // context when it is first created. Make that a context RunDispatchLoop can iterate.
        GMainContext* mainContext = g_main_context_new();
        g_main_context_push_thread_default(mainContext);
#endif

        auto globalContext = JSGlobalContextCreateInGroup(nullptr, nullptr);

#if defined(JSRUNTIMEHOST_HAS_JSC_EXECUTION_TIME_LIMIT) || \
    defined(JSRUNTIMEHOST_LOOKUP_JSC_EXECUTION_TIME_LIMIT)
        auto contextGroup = JSContextGetGroup(globalContext);
        ExecutionWatchdog watchdog{this};
#if defined(JSRUNTIMEHOST_HAS_JSC_EXECUTION_TIME_LIMIT)
        watchdog.Set = &JSContextGroupSetExecutionTimeLimit;
        watchdog.Clear = &JSContextGroupClearExecutionTimeLimit;
#else
        // WebKitGTK deliberately omits JSContextRefPrivate.h from its dev
        // package, but current system builds export the same C ABI. Resolve it
        // dynamically so JsRuntimeHost stays buildable against the public
        // package and gracefully falls back to between-dispatch termination on
        // older builds that do not export the watchdog.
        watchdog.Set = reinterpret_cast<SetExecutionTimeLimit>(
            dlsym(RTLD_DEFAULT, "JSContextGroupSetExecutionTimeLimit"));
        watchdog.Clear = reinterpret_cast<ClearExecutionTimeLimit>(
            dlsym(RTLD_DEFAULT, "JSContextGroupClearExecutionTimeLimit"));
#endif
        // Poll at a modest interval while JS is running. Returning true from
        // this callback raises a catchable termination exception (a bare
        // string, "JavaScript execution terminated.") and lets the AppRuntime
        // thread unwind, so Worker::terminate() also stops a tight loop that
        // never reaches the dispatch queue.
        watchdog.Callback = [](JSContextRef context, void* data) {
            auto* watchdog = static_cast<ExecutionWatchdog*>(data);
            if (watchdog->Runtime->IsExecutionTerminationRequested())
            {
                return true;
            }
            watchdog->Rearm(context);
            return false;
        };
        if (watchdog.Available())
        {
            watchdog.Set(contextGroup, ExecutionTimeLimitSeconds, watchdog.Callback, &watchdog);
        }
#endif

#if __APPLE__
        if (__builtin_available(iOS 16.4, macOS 13.3, *))
        {
            JSGlobalContextSetInspectable(globalContext, m_options.EnableDebugger);
        }
#endif

        Napi::Env env = Napi::Attach(globalContext);

        Run(env);

#if defined(JSRUNTIMEHOST_HAS_JSC_EXECUTION_TIME_LIMIT) || \
    defined(JSRUNTIMEHOST_LOOKUP_JSC_EXECUTION_TIME_LIMIT)
        if (watchdog.Available())
        {
            watchdog.Clear(contextGroup);
        }
#endif

        JSGlobalContextRelease(globalContext);

        // Detach must come after JSGlobalContextRelease since it triggers finalizers which require env.
        Napi::Detach(env);

#ifdef JSRUNTIMEHOST_JAVASCRIPTCORE_GLIB
        g_main_context_pop_thread_default(mainContext);
        g_main_context_unref(mainContext);
#endif
    }

    void AppRuntime::ShutdownEnvironment(Napi::Env)
    {
    }

    void AppRuntime::DrainMicrotasks(Napi::Env)
    {
        // JavaScriptCore drains microtasks automatically at script boundaries.
    }

#if defined(__APPLE__)
    // JavaScriptCore schedules some engine work on the CFRunLoop of the thread that created its
    // VM, notably settling asynchronous WebAssembly compile/instantiate promises (its
    // DeferredWorkTimer). Run this thread's CFRunLoop so that work executes, and wake it through
    // a run loop source whenever work is dispatched.
    void AppRuntime::RunDispatchLoop()
    {
        CFRunLoopSourceContext context{};
        context.info = this;
        context.perform = [](void* info) {
            static_cast<AppRuntime*>(info)->TickDispatcher();
        };

        CFRunLoopRef runLoop = CFRunLoopGetCurrent();
        CFRunLoopSourceRef source = CFRunLoopSourceCreate(kCFAllocatorDefault, 0, &context);
        CFRunLoopAddSource(runLoop, source, kCFRunLoopDefaultMode);

        SetDispatchLoopWaker([runLoop, source]() {
            CFRunLoopSourceSignal(source);
            CFRunLoopWakeUp(runLoop);
        });

        // Drain work queued before the waker was registered.
        CFRunLoopSourceSignal(source);

        while (!IsDispatchLoopCancelled())
        {
            CFRunLoopRunInMode(kCFRunLoopDefaultMode, 1.0e10, true);
        }

        SetDispatchLoopWaker({});

        CFRunLoopSourceInvalidate(source);
        CFRelease(source);
    }
#elif defined(JSRUNTIMEHOST_JAVASCRIPTCORE_GLIB)
    // WebKitGTK's JavaScriptCore schedules the same engine work (such as settling asynchronous
    // WebAssembly promises) as GLib sources on the main context RunEnvironmentTier made
    // thread-default. Iterate that context, and wake it whenever work is dispatched.
    // g_main_context_wakeup makes the next iteration return immediately if the loop isn't
    // blocked yet, so a wake-up between TickDispatcher and the iteration isn't lost.
    void AppRuntime::RunDispatchLoop()
    {
        GMainContext* mainContext = g_main_context_get_thread_default();

        SetDispatchLoopWaker([mainContext]() {
            g_main_context_wakeup(mainContext);
        });

        while (!IsDispatchLoopCancelled())
        {
            TickDispatcher();
            g_main_context_iteration(mainContext, TRUE);
        }

        SetDispatchLoopWaker({});
    }
#else
    void AppRuntime::RunDispatchLoop()
    {
        RunDispatcher();
    }
#endif
}
