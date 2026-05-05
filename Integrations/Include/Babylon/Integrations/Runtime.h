#pragma once

#include <Babylon/Integrations/RuntimeOptions.h>

#include <napi/env.h>

#include <functional>
#include <memory>
#include <string_view>

namespace Babylon::Integrations
{
    class View;
    struct RuntimeImpl;

    // Long-lived: typically created once per app/process. Sets up the
    // AppRuntime (JS thread + Napi env), JsRuntime, and non-GPU
    // polyfills/plugins. Construction is cheap and synchronous —
    // no GPU device exists yet. Device construction and GPU plugin
    // initialization (NativeEngine, etc.) are deferred to the first
    // `View::Attach` call.
    //
    // See SimplifiedAPI.md §4.1 for the full design.
    class Runtime
    {
    public:
        static std::unique_ptr<Runtime> Create(RuntimeOptions options = {});

        // // Future construction mode — adopt a host-owned Babylon::JsRuntime
        // // instead of letting Runtime construct its own AppRuntime+JsRuntime.
        // // Intended for hosts that already own a JS engine and want
        // // Babylon Native plugins to live inside it (e.g. React Native:
        // // Hermes/JSC + CallInvoker dispatcher). The Integrations layer
        // // never sees JSI directly — only Babylon::JsRuntime, which the
        // // host wires up against whatever JS engine they have.
        // //
        // // In Attach mode `~Runtime` does NOT tear down the JS engine
        // // (the host owns it); Suspend/Resume only DisableRendering on
        // // the Device since the JS thread isn't ours to pause. Same
        // // instance API as Create-mode otherwise. See SimplifiedAPI.md
        // // §4.1 "Construction modes".
        // static std::unique_ptr<Runtime> Attach(Babylon::JsRuntime& jsRuntime,
        //                                        RuntimeOptions options = {});

        ~Runtime();

        // Non-copyable, non-movable (Views hold raw pointers back to this).
        Runtime(const Runtime&) = delete;
        Runtime& operator=(const Runtime&) = delete;
        Runtime(Runtime&&) = delete;
        Runtime& operator=(Runtime&&) = delete;

        // ----- JS interaction -----
        //
        // Calls made before the first `View::Attach` are queued internally
        // and dispatched onto the JS thread after engine initialization
        // completes during that first Attach. Calls made after the first
        // Attach are dispatched immediately.
        //
        // Threading: these methods are NOT internally synchronized.
        // Hosts should call them from a single thread (typically the
        // host's UI/main thread), matching the existing contract of
        // `Babylon::ScriptLoader` and `Babylon::AppRuntime::Dispatch`.
        void LoadScript(std::string_view url);
        void Eval(std::string_view source, std::string_view sourceUrl = {});

        // Escape hatch: post `callback` onto the JS thread. The callback
        // runs after any pending init has completed. Useful for installing
        // custom Napi globals, registering ObjectWrap classes, capturing
        // `Napi::FunctionReference`s for native→JS calls, etc.
        //
        // Threading: same single-thread contract as LoadScript / Eval.
        void RunOnJsThread(std::function<void(Napi::Env)> callback);

        // ----- Suspend / Resume -----
        //
        // Orthogonal to view attachment. Use when the host app is
        // backgrounded, throttled, or otherwise should not be doing work
        // (iOS applicationWillResignActive, Android onPause, modal
        // dialogs, power-saving mode). While suspended:
        //   - JS timers (setTimeout/setInterval) pause.
        //   - In-flight microtasks complete; no new tasks are dispatched.
        //   - Any attached View becomes a no-op for RenderFrame() — the
        //     host can keep calling it from its draw callback; nothing
        //     happens until Resume().
        // Calls are reference-counted; nesting is safe.
        //
        // Safe to call from any thread.
        void Suspend();
        void Resume();
        bool IsSuspended() const;

#if BABYLON_NATIVE_PLUGIN_NATIVEXR
        // ----- XR session control -----
        //
        // Set the platform window XR will render into. The `void*`
        // type carries:
        //   Android : ANativeWindow*  (typically from a separate
        //             transparent SurfaceView overlay)
        //   Apple   : CAMetalLayer* / MTKView* (a separate Metal layer
        //             distinct from the main View's layer)
        //
        // Pass nullptr to clear the XR surface. Safe to call before
        // the first `View::Attach`; the supplied window is applied
        // when NativeXr finishes initializing during that first Attach.
        // Safe to call from any thread.
        void SetXrWindow(void* nativeWindow);

        // True while an XR session is active. Updated from the JS
        // thread by NativeXr's internal session-state callback;
        // atomic so it can be polled from any thread (e.g. a host's
        // draw callback choosing between rendering targets).
        bool IsXrActive() const;
#endif

    private:
        friend class View;
        friend struct ViewImpl;

        Runtime();

        std::unique_ptr<RuntimeImpl> m_impl;
    };
}
