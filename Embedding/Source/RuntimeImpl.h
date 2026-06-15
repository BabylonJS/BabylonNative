#pragma once

#include <Babylon/Embedding/Runtime.h>
#include <Babylon/Embedding/View.h>

#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/ScriptLoader.h>

#if BABYLON_NATIVE_POLYFILL_CANVAS
#include <Babylon/Polyfills/Canvas.h>
#endif

#if BABYLON_NATIVE_PLUGIN_NATIVEINPUT
#include <Babylon/Plugins/NativeInput.h>
#endif

#if BABYLON_NATIVE_PLUGIN_NATIVEXR
#include <Babylon/Plugins/NativeXr.h>
#endif

#include <arcana/threading/task.h>

#include <atomic>
#include <mutex>
#include <optional>
#include <utility>

namespace Babylon::Embedding
{
    // Internal implementation of Runtime. Lives in Source/ so View.cpp
    // can reach into it without exposing internals on the public header.
    struct RuntimeImpl
    {
        explicit RuntimeImpl(RuntimeOptions options);
        ~RuntimeImpl();

        RuntimeImpl(const RuntimeImpl&) = delete;
        RuntimeImpl& operator=(const RuntimeImpl&) = delete;

        // Configuration captured at construction.
        RuntimeOptions m_options;

        // Always-alive: JS engine + thread, plus the script loader that
        // serializes LoadScript / Eval / Dispatch onto that thread.
        std::optional<Babylon::AppRuntime> m_appRuntime;
        std::optional<Babylon::ScriptLoader> m_scriptLoader;

        // Lazily constructed during the first attached View.
        std::optional<Babylon::Graphics::Device> m_device;
        std::optional<Babylon::Graphics::DeviceUpdate> m_deviceUpdate;

#if BABYLON_NATIVE_POLYFILL_CANVAS
        std::optional<Babylon::Polyfills::Canvas> m_canvas;
#endif

#if BABYLON_NATIVE_PLUGIN_NATIVEINPUT
        // Owned by the JS world (returned by NativeInput::CreateForJavaScript).
        // We just keep a pointer for forwarding View::OnPointer* calls.
        Babylon::Plugins::NativeInput* m_input{nullptr};
#endif

#if BABYLON_NATIVE_PLUGIN_NATIVEXR
        // NativeXr is initialized during the first View attach (needs a
        // Napi::Env). Until then, m_xrWindow holds the most recent
        // SetXrWindow value for application post-init. m_xrMutex serializes
        // these fields. m_isXrActive is written from the JS thread and
        // polled from any thread.
        std::optional<Babylon::Plugins::NativeXr> m_nativeXr;
        void* m_xrWindow{nullptr};
        mutable std::mutex m_xrMutex;
        std::atomic<bool> m_isXrActive{false};
#endif

        // Pre-init queue for host LoadScript / Eval / RunOnJsThread.
        // Each call chains its work off `m_initTcs.as_task().then(...)`.
        // The first-attach init lambda calls `m_initTcs.complete()` after
        // all plugins are initialized, firing every queued continuation
        // (in registration order) on the JS thread. After completion,
        // future `.then(inline_scheduler, ...)` calls run synchronously
        // on the calling thread and submit to ScriptLoader directly.
        //
        // Host-side serialization is the host's responsibility (same
        // contract as ScriptLoader); no outer mutex.
        arcana::task_completion_source<void, std::exception_ptr> m_initTcs;

        // Reference-counted suspend depth. Mutated only on the frame
        // thread; atomic so `IsSuspended()` can be polled from any thread.
        std::atomic<int> m_suspendCount{0};

        // 0..1 — enforces "at most one View attached at a time". Points
        // at the ViewImpl directly (not the outer View), so the back-ref
        // stays valid across moves of the outer View.
        ViewImpl* m_currentView{nullptr};

        // Dispatched onto the JS thread by the very first View attach.
        // Runs all plugin/polyfill Initialize calls, wires NativeXr
        // callbacks, and completes m_initTcs. `window` is forwarded to
        // TestUtils::Initialize; ignored otherwise.
        void RunFirstAttachInit(Babylon::Graphics::WindowT window);

#if BABYLON_NATIVE_PLUGIN_SHADERCACHE
        // Persistent shader cache. No-ops when `m_options.shaderCachePath`
        // is empty. Both run synchronously on the host thread at points
        // where the engine is known not to be compiling shaders
        // (first-attach, post-view-Suspend, ~RuntimeImpl) — no JS-thread
        // coordination required.
        void LoadShaderCache();
        void SaveShaderCache();
#endif
    };

    // Internal implementation of View. Owns the back-reference to its
    // RuntimeImpl, the window handle captured at construction, the most
    // recent Resize size, and the frame-lifecycle Suspend/Resume hooks.
    //
    // Construction is cheap: it just stashes the window and registers as
    // the current view. All Device work runs in `InitializeIfReady`, which
    // is gated on three preconditions:
    //
    //   1. `m_initialized == false`,
    //   2. `m_size` is set (host has called Resize at least once), and
    //   3. `RuntimeImpl::m_suspendCount == 0`.
    //
    // Called from `View::Resize` (satisfies #2) and `ViewImpl::Resume`
    // (can clear #3); whichever caller satisfies the last precondition
    // runs the init. Folding `UpdateWindow` + `UpdateSize` together
    // inside this function is what keeps the bgfx pair safe.
    //
    // Once initialized, a Device frame is open iff `m_suspendCount == 0`.
    // ViewImpl::Suspend/Resume toggle that frame on the 0↔1 transitions.
    struct ViewImpl
    {
        explicit ViewImpl(RuntimeImpl& runtime) : m_runtime{runtime} {}
        ~ViewImpl();

        ViewImpl(const ViewImpl&) = delete;
        ViewImpl& operator=(const ViewImpl&) = delete;

        RuntimeImpl& m_runtime;

        // Captured at construction; used by InitializeIfReady to
        // construct (first-attach-ever) or rebind the Device.
        Babylon::Graphics::WindowT m_window{};

        // Most recent Resize size, in logical pixels. Empty until the
        // host calls Resize.
        std::optional<std::pair<uint32_t, uint32_t>> m_size;

        // Latches true the first time InitializeIfReady succeeds; never
        // flips back. A new ViewImpl is constructed per View attach.
        bool m_initialized{false};

        // Close the in-flight frame (called by Runtime::Suspend on 0→1
        // and by ~ViewImpl). No-op when uninitialized.
        void Suspend();

        // Open a new frame (called by Runtime::Resume on 1→0). When
        // uninitialized, retries InitializeIfReady — handles the case
        // where the host attached and resized while suspended.
        void Resume();

        // Runs the deferred init iff all three preconditions hold.
        // Safe to call from any caller that just satisfied one.
        void InitializeIfReady();
    };
}
