#include "AppRuntime.h"
#include <napi/env.h>

#if defined(__APPLE__)
#include <CoreFoundation/CoreFoundation.h>
#elif defined(JSRUNTIMEHOST_JAVASCRIPTCORE_GLIB)
#include <glib.h>

namespace WTF
{
    // Exported by libjavascriptcoregtk but not declared in its public headers.
    void initializeMainThread();
}
#endif

namespace Babylon
{
    void AppRuntime::RunEnvironmentTier(const char*)
    {
#ifdef JSRUNTIMEHOST_JAVASCRIPTCORE_GLIB
        // The VM binds to this thread's WTF RunLoop, which adopts the thread-default GLib main
        // context when it is first created. Make that a context RunDispatchLoop can iterate.
        GMainContext* mainContext = g_main_context_new();
        g_main_context_push_thread_default(mainContext);

        // Iterating that context also fires JavaScriptCore's GC timers. The full collection timer
        // uses WTF::MemoryPressureHandler, which on Linux needs WTF's main RunLoop. WebKit creates
        // it in WTF::initializeMainThread, which the JavaScriptCore C API never calls, so the
        // first collection would crash. Call it here; only the first call in the process has an
        // effect, binding WTF's main thread and RunLoop to this thread and its pushed context.
        WTF::initializeMainThread();
#endif

        auto globalContext = JSGlobalContextCreateInGroup(nullptr, nullptr);

#if __APPLE__
        if (__builtin_available(iOS 16.4, macOS 13.3, *))
        {
            JSGlobalContextSetInspectable(globalContext, m_options.EnableDebugger);
        }
#endif

        Napi::Env env = Napi::Attach(globalContext);

        Run(env);

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