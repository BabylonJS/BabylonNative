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
    };

    // Internal implementation of View. Holds the back-reference to the
    // Runtime that produced it.
    struct ViewImpl
    {
        explicit ViewImpl(Runtime& runtime) : m_runtime{runtime} {}
        Runtime& m_runtime;
    };
}
