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

#include <atomic>
#include <functional>
#include <mutex>
#include <optional>
#include <vector>

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

        // ----- Pre-init queueing ------
        //
        // Before the first View::Attach completes engine initialization
        // on the JS thread, LoadScript / Eval / RunOnJsThread calls are
        // recorded here and flushed (in submission order) inside the
        // first-Attach init lambda after all plugin Initialize() calls.
        // After flush, m_initialized is true and subsequent calls
        // dispatch directly through ScriptLoader / AppRuntime.
        bool m_initialized{false};
        std::vector<std::function<void()>> m_pending;
        std::mutex m_pendingMutex;

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
