#include "AppRuntime.h"
#include <napi/env.h>

#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#endif

namespace Babylon
{
    void AppRuntime::RunEnvironmentTier(const char*)
    {
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
    }

    void AppRuntime::ShutdownEnvironment(Napi::Env)
    {
    }

    void AppRuntime::DrainMicrotasks(Napi::Env)
    {
        // JavaScriptCore drains microtasks automatically at script boundaries.
    }

#ifdef __APPLE__
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
#else
    void AppRuntime::RunDispatchLoop()
    {
        // On Linux, JavaScriptCore (WebKitGTK) completes async WebAssembly promises on its
        // GLib-based RunLoop, which this loop doesn't iterate yet, so those promises don't settle.
        RunDispatcher();
    }
#endif
}
