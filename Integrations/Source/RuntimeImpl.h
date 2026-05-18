#pragma once

#include <Babylon/Integrations/Runtime.h>
#include <Babylon/Integrations/View.h>

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

namespace Babylon::Integrations
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

        // Lazily constructed during the first View::Attach.
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
        // NativeXr is initialized during the first View::Attach (it
        // needs a Napi::Env). Until then, m_xrWindow holds the host's
        // most recent SetXrWindow value so we can apply it as soon as
        // the plugin is alive. m_xrMutex serializes both fields plus
        // the optional itself. m_isXrActive is updated from the JS
        // thread by NativeXr's session-state callback and read from
        // any thread by IsXrActive() polling.
        std::optional<Babylon::Plugins::NativeXr> m_nativeXr;
        void* m_xrWindow{nullptr};
        mutable std::mutex m_xrMutex;
        std::atomic<bool> m_isXrActive{false};
#endif

        // ----- Pre-init queueing -----
        //
        // Host calls to LoadScript / Eval / RunOnJsThread are chained
        // off `m_initTcs.as_task().then(inline_scheduler, ...)`. While
        // the TCS is uncompleted (i.e. the first View::Attach hasn't
        // finished plugin initialization on the JS thread), continuations
        // sit on the task payload. The first-Attach init lambda calls
        // `m_initTcs.complete()` after all plugins are initialized,
        // which fires every queued continuation in registration order
        // on the JS thread. After completion, subsequent
        // `.then(inline_scheduler, ...)` calls run their callable
        // synchronously on the calling thread, which then submits to
        // ScriptLoader directly.
        //
        // Host-side serialization is the host's responsibility, matching
        // ScriptLoader's existing contract; we do not add an outer mutex.
        arcana::task_completion_source<void, std::exception_ptr> m_initTcs;

        // Reference-counted Suspend/Resume. Atomic so `IsSuspended()`
        // can be polled cheaply from any thread (e.g. a worker checking
        // whether to bother queuing more host-side work). Mutations to
        // the count itself happen only on the frame thread (the
        // documented contract of `Runtime::Suspend` / `Runtime::Resume`),
        // so the increment/decrement do not need to be locked.
        std::atomic<int> m_suspendCount{0};

        // 0..1; tracked so we can guard against multiple concurrent
        // attachments (the API contract is "at most one View at a time").
        View* m_currentView{nullptr};

        // First-Attach engine initialization: dispatched onto the JS
        // thread by View::Attach the very first time it constructs the
        // Device. Runs all plugin/polyfill `Initialize` calls, wires
        // NativeXr session-state callbacks, then completes `m_initTcs`
        // to unblock any LoadScript / Eval / RunOnJsThread calls the
        // host queued before the first Attach.
        //
        // The `window` parameter is forwarded to TestUtils::Initialize
        // (the only plugin that wants it); ignored otherwise.
        void RunFirstAttachInit(Babylon::Graphics::WindowT window);

#if BABYLON_NATIVE_PLUGIN_SHADERCACHE
        // ----- Persistent shader cache -----
        //
        // Both methods are no-ops when `m_options.shaderCachePath` is
        // empty. Both run synchronously on the host thread; they do
        // not need to coordinate with the JS thread because callers
        // (first-Attach, post-view-Suspend, destructor) are points at
        // which the engine is known not to be compiling shaders.

        // Load the on-disk shader cache file into the in-memory cache.
        // Called from `RunFirstAttachInit` right after
        // `ShaderCache::Enable()`. Safe because no shaders have been
        // compiled yet at this point — the cache map is quiescent.
        void LoadShaderCache();

        // Serialize the in-memory shader cache to disk. Called from
        // `Runtime::Suspend` (after `ViewImpl::Suspend()` has closed
        // the current frame and locked the update safe-timespan) and
        // from `~RuntimeImpl` (after the View precondition has
        // guaranteed `ViewImpl::Suspend()` already ran via `~View`).
        // No async/JS-thread coordination is required: at these
        // points there is no in-flight engine work writing to the
        // cache.
        void SaveShaderCache();
#endif
    };

    // Internal implementation of View. Holds the back-reference to the
    // Runtime that produced it, the native window handle captured at
    // `View::Attach`, the most recent size handed in via `View::Resize`,
    // plus Suspend / Resume that manage the in-flight frame across
    // runtime suspension.
    //
    // `View::Attach` is intentionally cheap: it just stashes the
    // window handle and registers as the current view. All Device
    // interaction (first-time construction, or `UpdateWindow` +
    // `UpdateSize` on a re-attach to an existing Runtime) is deferred
    // to `InitializeIfReady`, which only runs once all three
    // preconditions hold:
    //
    //   1. `m_initialized` is still `false` (the view hasn't been
    //      initialized yet for this Attach),
    //   2. `m_size` is set (the host has called `View::Resize` at
    //      least once with a non-zero size), and
    //   3. `RuntimeImpl::m_suspendCount` is zero (the host hasn't
    //      called `Runtime::Suspend` without a matching `Resume`).
    //
    // `InitializeIfReady` is called from `View::Resize` (which sets
    // condition 2) and from `ViewImpl::Resume` (which can clear
    // condition 3). Whichever caller satisfies the last missing
    // precondition is the one that actually runs the init recipe.
    // This is also what makes the `UpdateWindow` + `UpdateSize` pair
    // safe: both happen together inside `InitializeIfReady`, so bgfx
    // never sees a window change without a matching size change.
    //
    // Once `m_initialized` flips to `true`, a Device frame is open
    // iff `RuntimeImpl::m_suspendCount == 0`. `ViewImpl::Suspend` and
    // `ViewImpl::Resume` are called by `Runtime::Suspend` /
    // `Runtime::Resume` on the suspendCount 0↔1 transitions and
    // toggle that frame open/closed.
    struct ViewImpl
    {
        explicit ViewImpl(Runtime& runtime) : m_runtime{runtime} {}
        Runtime& m_runtime;

        // Window handle captured at `View::Attach`. Used by
        // `InitializeIfReady` to construct (first-Attach-ever) or
        // rebind (subsequent Attach) the Device.
        Babylon::Graphics::WindowT m_window{};

        // Most recent size handed in via `View::Resize`, in logical
        // pixels. Empty until the host has called `Resize` at least
        // once. Used by `InitializeIfReady` to size the Device on the
        // first init.
        std::optional<std::pair<uint32_t, uint32_t>> m_size;

        // Latches to `true` the first time `InitializeIfReady` runs
        // all three preconditions to completion. Never flips back for
        // the lifetime of this `ViewImpl`; a new `ViewImpl` is
        // constructed on each `View::Attach`.
        bool m_initialized{false};

        // Close the in-flight frame on the Device (Finish +
        // FinishRenderingCurrentFrame). Called by `Runtime::Suspend`
        // on the suspendCount 0→1 transition, and by `~View`. No-op
        // when the view is not yet initialized (no frame to close).
        void Suspend();

        // Open a new frame (StartRenderingCurrentFrame +
        // DeviceUpdate::Start) on the Device. Called by
        // `Runtime::Resume` on the suspendCount 1→0 transition. When
        // the view is not yet initialized, this instead tries to run
        // the deferred first-Resize init (which itself opens the
        // first frame if it succeeds) — so a host that attached and
        // resized while the Runtime was externally suspended sees its
        // init kick off cleanly on the next `Runtime::Resume`.
        void Resume();

        // Run the deferred Device-binding + first-frame-opening
        // recipe iff all three preconditions are satisfied (see the
        // class-level comment). No-op otherwise. Safe to call from
        // any of the entry points that can satisfy a precondition.
        void InitializeIfReady();
    };
}
