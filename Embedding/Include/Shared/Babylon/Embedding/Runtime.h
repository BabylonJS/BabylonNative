#pragma once

#include <Babylon/Embedding/RuntimeOptions.h>

#include <napi/env.h>

#include <functional>
#include <memory>
#include <string_view>

namespace Babylon::Embedding
{
    class View;
    struct RuntimeImpl;

    // Long-lived: typically created once per app/process. Sets up the
    // AppRuntime (JS thread + Napi env), JsRuntime, and non-GPU
    // polyfills/plugins. Construction is cheap and synchronous; the GPU
    // Device and plugins (NativeEngine, etc.) are deferred to the first
    // attached `View`.
    class Runtime
    {
    public:
        explicit Runtime(RuntimeOptions options = {});

        // // Future construction mode: adopt a host-owned Babylon::JsRuntime
        // // (e.g. React Native with Hermes/JSC + CallInvoker). In Attach mode
        // // `~Runtime` does NOT tear down the JS engine, and Suspend/Resume
        // // only toggles Device rendering.
        // static Runtime Adopt(Babylon::JsRuntime& jsRuntime,
        //                      RuntimeOptions options = {});

        ~Runtime();

        // Non-copyable; movable. Cross-references between Runtime and View
        // point at the heap-allocated pimpls, so moves of the outer wrappers
        // are safe and don't invalidate any back-pointers.
        Runtime(const Runtime&) = delete;
        Runtime& operator=(const Runtime&) = delete;
        Runtime(Runtime&&) noexcept;
        Runtime& operator=(Runtime&&) noexcept;

        // ----- JS interaction -----
        //
        // Calls made before the first `View` is attached are queued and
        // dispatched onto the JS thread after engine initialization
        // completes during that first attach. Calls after first attach are
        // dispatched immediately.
        //
        // Not internally synchronized — call from a single thread (typically
        // the host's UI/main thread), matching `Babylon::ScriptLoader` /
        // `AppRuntime::Dispatch`.
        void LoadScript(std::string_view url);
        void Eval(std::string_view source, std::string_view sourceUrl = {});

        // Escape hatch: post `callback` onto the JS thread after engine
        // initialization completes. Useful for installing custom Napi globals,
        // registering ObjectWrap classes, or capturing FunctionReferences for
        // native→JS calls. Same single-thread contract as LoadScript / Eval.
        //
        // `afterScriptLoad`:
        //   false (default) — dispatch via `AppRuntime::Dispatch`. The callback
        //     runs on the JS thread as soon as init is complete; it is NOT
        //     serialized behind queued `LoadScript` / `Eval` calls and may
        //     interleave with their execution.
        //   true — dispatch via `ScriptLoader::Dispatch`. The callback is
        //     serialized behind any earlier `LoadScript` / `Eval` / `RunOnJsThread(true)`
        //     calls, so it observes the JS state after those have finished.
        void RunOnJsThread(std::function<void(Napi::Env)> callback, bool afterScriptLoad = false);

        // ----- Suspend / Resume -----
        //
        // Orthogonal to view attachment. Use for backgrounding, throttling,
        // modal dialogs, etc. While suspended:
        //   - JS timers (setTimeout/setInterval) pause.
        //   - In-flight microtasks complete; no new tasks are dispatched.
        //   - Any attached View's RenderFrame() becomes a no-op.
        // Reference-counted; nesting is safe.
        //
        // `Suspend` / `Resume` must be called from the frame thread (same
        // thread driving `View::RenderFrame` / `View::Resize`). Platform
        // lifecycle callbacks fire on the main thread already, so wiring them
        // up gets this for free. `IsSuspended` is atomic and safe from any
        // thread.
        void Suspend();
        void Resume();
        bool IsSuspended() const;

#if BABYLON_NATIVE_PLUGIN_NATIVEXR
        // ----- XR session control -----
        //
        // Set the platform window XR will render into:
        //   Android : ANativeWindow*  (typically a transparent SurfaceView overlay)
        //   Apple   : CAMetalLayer* / MTKView* (separate Metal layer from the main View)
        //
        // Pass nullptr to clear. Safe to call before the first `View` is
        // attached (the window is applied when NativeXr finishes
        // initializing) and from any thread.
        void SetXrWindow(void* nativeWindow);

        // True while an XR session is active. Atomic; safe to poll from any
        // thread (e.g. a draw callback selecting render targets).
        bool IsXrActive() const;
#endif

    private:
        friend class View;

        std::unique_ptr<RuntimeImpl> m_impl;
    };
}