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

        // Reference-counted Suspend/Resume.
        int m_suspendCount{0};
        mutable std::mutex m_suspendMutex;

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
    // `View::Attach`, plus Suspend / Resume that manage the in-flight
    // frame across runtime suspension.
    //
    // `View::Attach` is intentionally cheap: it just stashes the
    // window handle and registers as the current view. All Device
    // interaction (first-time construction, or `UpdateWindow` +
    // `UpdateSize` on a re-attach to an existing Runtime) is deferred
    // to the first `View::Resize` call. This is a hard requirement:
    // `Device::UpdateWindow` MUST be paired with a matching
    // `Device::UpdateSize` or bgfx will render the next frame to the
    // new window at the wrong size. Folding both into the first
    // `Resize` makes the host-supplied dimensions the single source
    // of truth — the Integrations layer never queries the window for
    // its size.
    //
    // `m_suspended` is the View's view of whether it's currently
    // holding an open Device frame:
    //   - true  → no frame is open (StartRenderingCurrentFrame +
    //             DeviceUpdate::Start have NOT been called, or have
    //             been matched by Finish / FinishRenderingCurrentFrame)
    //   - false → a frame is open and the JS thread's safe timespan
    //             is active
    //
    // Initially `true`; the first `View::Resize` flips it to `false`
    // after binding the Device to the new window+size and opening the
    // first frame. `~View` calls `Suspend` before relinquishing the
    // view slot. `Runtime::Suspend / Resume` call through here on the
    // suspendCount 0↔1 transitions; both are no-ops while
    // `m_initialized` is still `false`, since there is no Device
    // binding to suspend/resume yet.
    //
    // `m_initialized` is the View's "first Resize on this Attach has
    // run" latch. Starts `false`; the first successful `Resize` sets
    // it `true` after the Device is bound. It is the gate that lets
    // `Resume()` open a frame: while `false`, `Resume()` is a no-op,
    // so render-loop callbacks that fire between `Attach` and the
    // first `Resize` (typical on Apple, where MTKView fires
    // `drawInMTKView:` before `drawableSizeWillChange:`) silently
    // do nothing rather than rendering to an unbound surface.
    struct ViewImpl
    {
        explicit ViewImpl(Runtime& runtime) : m_runtime{runtime} {}
        Runtime& m_runtime;

        // Window handle captured at `View::Attach`. Used by the first
        // `View::Resize` to construct (first-Attach-ever) or rebind
        // (subsequent Attach) the Device.
        Babylon::Graphics::WindowT m_window{};

        // See class-level comment. Latches to `true` on first successful
        // Resize; never flips back for the lifetime of this ViewImpl.
        bool m_initialized{false};

        // See class-level comment. "No frame is currently open."
        bool m_suspended{true};

        // End the in-flight frame on the Device (Finish +
        // FinishRenderingCurrentFrame). Idempotent — no-op if already
        // suspended or if the view has not yet been initialized.
        void Suspend();

        // Open a new frame (StartRenderingCurrentFrame +
        // DeviceUpdate::Start). Idempotent — no-op if not currently
        // suspended or if the view has not yet been initialized.
        void Resume();
    };
}
