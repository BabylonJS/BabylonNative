#include "RuntimeImpl.h"

#include <Babylon/DebugTrace.h>
#include <Babylon/Graphics/DeviceQueries.h>

#include <napi/napi.h>

#include <stdexcept>
#include <string>
#include <utility>

namespace Babylon::Embedding
{
    namespace
    {
        // Convert (width, height) in `units` to logical pixels.
        std::pair<uint32_t, uint32_t> ToLogicalSize(uint32_t width, uint32_t height,
                                                    CoordinateUnits units, float dpr)
        {
            if (units == CoordinateUnits::Logical || dpr <= 0.0f)
            {
                return {width, height};
            }
            return {static_cast<uint32_t>(width / dpr),
                    static_cast<uint32_t>(height / dpr)};
        }

        // Convert (x, y) in `units` to logical pixels.
        std::pair<float, float> ToLogicalCoords(float x, float y,
                                                CoordinateUnits units, float dpr)
        {
            if (units == CoordinateUnits::Logical || dpr <= 0.0f)
            {
                return {x, y};
            }
            return {x / dpr, y / dpr};
        }

        void ValidateNonZeroSize(uint32_t width, uint32_t height, const char* operation)
        {
            if (width == 0 || height == 0)
            {
                throw std::runtime_error{std::string{operation} + " requires non-zero width and height."};
            }
        }

        // Current device-pixel ratio: the bgfx Graphics::Device's stored value
        // when present, otherwise the window's (the NativeDawn backend has no
        // Graphics::Device, so input/coordinate conversion falls back to the
        // window DPR). Only referenced by the NativeInput forwarding methods.
        [[maybe_unused]] float CurrentDpr(RuntimeImpl& impl, Babylon::Graphics::WindowT window)
        {
            if (impl.m_device)
            {
                return impl.m_device->GetDevicePixelRatio();
            }
            return Babylon::Graphics::GetDevicePixelRatio(window);
        }
    }

    View::View(Runtime& runtime, Babylon::Graphics::WindowT nativeWindow)
    {
        RuntimeImpl& runtimeImpl = *runtime.m_impl;
        if (runtimeImpl.m_currentView != nullptr)
        {
            throw std::runtime_error{"Only one View may be attached to a Runtime at a time."};
        }

        m_impl = std::make_unique<ViewImpl>(runtimeImpl);
        m_impl->m_window = nativeWindow;
        runtimeImpl.m_currentView = m_impl.get();
    }

    View::~View() = default;
    View::View(View&&) noexcept = default;
    View& View::operator=(View&&) noexcept = default;

    ViewImpl::~ViewImpl()
    {
#if BABYLON_NATIVE_PLUGIN_NATIVEDAWN
        // NativeDawn presents each frame inside its per-frame Tick, so there is
        // no host-held in-flight frame to close on teardown.
#else
        // Close the in-flight frame iff one is currently open: i.e. the
        // view is initialized and the Runtime is not externally suspended
        // (Suspend already closed the frame). The Device persists on the
        // Runtime so the next View is cheap.
        if (m_initialized &&
            m_runtime.m_suspendCount.load(std::memory_order_relaxed) == 0)
        {
            m_runtime.m_device->FinishRenderingCurrentFrame();
        }
#endif

        m_runtime.m_currentView = nullptr;
    }

    // Called by Runtime::Suspend/Resume on the 0↔1 transition (and ~ViewImpl
    // for teardown). When initialized, these are pure frame open/close.
    // When uninitialized, Suspend is a no-op and Resume retries
    // InitializeIfReady — the suspendCount drop may have been the last
    // missing precondition.
    void ViewImpl::Suspend()
    {
        if (!m_initialized)
        {
            return;
        }
#if !BABYLON_NATIVE_PLUGIN_NATIVEDAWN
        m_runtime.m_device->FinishRenderingCurrentFrame();
#endif
    }

    void ViewImpl::Resume()
    {
        if (!m_initialized)
        {
            // Suspend precondition just cleared. Succeeds if `Resize` has
            // also run; otherwise silent no-op until it does.
            InitializeIfReady();
            return;
        }
#if !BABYLON_NATIVE_PLUGIN_NATIVEDAWN
        m_runtime.m_device->StartRenderingCurrentFrame();
#endif
    }

    // Single recipe for binding the Device to this view's window and
    // opening the first frame. Gated on all three preconditions
    // (initialized? sized? not suspended?) so it can be called eagerly
    // from anywhere that satisfies one of them.
    void ViewImpl::InitializeIfReady()
    {
        if (m_initialized || !m_size)
        {
            return;
        }
        if (m_runtime.m_suspendCount.load(std::memory_order_relaxed) > 0)
        {
            return;
        }

        const auto [lw, lh] = *m_size;
#if BABYLON_NATIVE_PLUGIN_NATIVEDAWN
        // NativeDawn owns the device/surface lifecycle; there is no bgfx
        // Graphics::Device to construct here. The plugin's Initialize (creating
        // the Dawn device + surface bound to m_window at the current size) runs
        // inside RunFirstAttachInit on the JS thread.
        const bool firstAttachEver = !m_runtime.m_dawnInitialized;
        if (!firstAttachEver)
        {
            // Re-attach to an existing Runtime: reconfigure the Dawn surface to
            // the current size on the JS thread (where the Dawn device lives).
            m_runtime.m_appRuntime->Dispatch([lw, lh](Napi::Env) {
                Babylon::Plugins::NativeDawn::ResizeSurface(lw, lh);
            });
        }
        m_initialized = true;

        if (firstAttachEver)
        {
            m_runtime.m_dawnInitialized = true;
            m_runtime.RunFirstAttachInit(m_window, lw, lh);
        }
#else
        const bool firstAttachEver = !m_runtime.m_device;
        if (firstAttachEver)
        {
            Babylon::Graphics::Configuration config{};
            config.Window = m_window;
            config.Width = lw;
            config.Height = lh;
            config.MSAASamples = m_runtime.m_options.msaaSamples;

            m_runtime.m_device.emplace(config);
        }
        else
        {
            // Re-attach to an existing Runtime: rebind surface and size in
            // lockstep. JS state (plugins, polyfills, scripts) is preserved.
            m_runtime.m_device->UpdateWindow(m_window);
            m_runtime.m_device->UpdateSize(lw, lh);
        }

        // Open the first frame inline. On the very first attach this MUST
        // happen BEFORE dispatching RunFirstAttachInit, so the
        // Device::AddToJavaScript inside that lambda sees an open frame to
        // record into. Both run on the host thread, so the dispatched
        // lambda always sees an open frame on the JS thread.
        m_runtime.m_device->StartRenderingCurrentFrame();
        m_initialized = true;

        if (firstAttachEver)
        {
            m_runtime.RunFirstAttachInit(m_window, lw, lh);
        }
#endif
    }

    void View::RenderFrame()
    {
        RuntimeImpl& impl = m_impl->m_runtime;

        // Host may call unconditionally from its draw callback; the
        // not-rendering cases early-out.
        if (!m_impl->m_initialized)
        {
            // Flag pre-init so hosts can diagnose "my draw callback fires
            // but nothing renders". The externally-suspended case is
            // expected and stays silent.
            DEBUG_TRACE("Babylon::Embedding::View::RenderFrame skipped: View has not yet been initialized. Call View::Resize with the surface's pixel dimensions to begin rendering.");
            return;
        }
        if (impl.m_suspendCount.load(std::memory_order_relaxed) > 0)
        {
            return;
        }

#if BABYLON_NATIVE_PLUGIN_NATIVEDAWN
        // WebGPUEngine drives its render loop from requestAnimationFrame, pumped
        // by the JS-thread global frame(). Dispatch one frame at a time (throttled
        // so the JS queue can't flood): flush rAF (runs the engine's render +
        // GPU submit) and present via NativeDawn::Tick.
        if (!impl.m_dawnFrameInFlight.exchange(true))
        {
            impl.m_appRuntime->Dispatch([implPtr = &impl](Napi::Env env) {
                Napi::Value frame = env.Global().Get("frame");
                if (frame.IsFunction())
                {
                    frame.As<Napi::Function>().Call({});
                }
                Babylon::Plugins::NativeDawn::Tick(env);
                implPtr->m_dawnFrameInFlight.store(false, std::memory_order_relaxed);
            });
        }
#else
        // Babylon's JS render loop runs between Start and Finish, scheduled
        // via DeviceUpdate's SafeTimespanGuarantor onto the JS thread.
        impl.m_device->FinishRenderingCurrentFrame();
        impl.m_device->StartRenderingCurrentFrame();
#endif
    }

    // Stores the host-supplied size on the ViewImpl, then either pushes
    // it through Device::UpdateSize (initialized) or drives
    // InitializeIfReady (uninitialized). Single source of truth for
    // surface size in the Embedding layer.
    void View::Resize(uint32_t width, uint32_t height, CoordinateUnits units)
    {
        ValidateNonZeroSize(width, height, "View::Resize size");

        RuntimeImpl& impl = m_impl->m_runtime;

        // DPR source: before init, query the window directly (Device
        // doesn't exist or is still bound to the previous attach). After
        // init, the Device's stored DPR is authoritative (bgfx). NativeDawn
        // has no Device, so keep querying the window.
        float dpr;
        if (!m_impl->m_initialized)
        {
            dpr = Babylon::Graphics::GetDevicePixelRatio(m_impl->m_window);
        }
        else
        {
#if BABYLON_NATIVE_PLUGIN_NATIVEDAWN
            dpr = Babylon::Graphics::GetDevicePixelRatio(m_impl->m_window);
#else
            dpr = impl.m_device->GetDevicePixelRatio();
#endif
        }
        const auto [lw, lh] = ToLogicalSize(width, height, units, dpr);
        ValidateNonZeroSize(lw, lh, "View::Resize logical size");

        m_impl->m_size = {lw, lh};

        if (m_impl->m_initialized)
        {
#if BABYLON_NATIVE_PLUGIN_NATIVEDAWN
            // Reconfigure the Dawn surface and the JS engine's drawing buffer on
            // the JS thread (where the Dawn device lives).
            impl.m_appRuntime->Dispatch([lw, lh](Napi::Env env) {
                Babylon::Plugins::NativeDawn::ResizeSurface(lw, lh);
                Napi::Value resizeFn = env.Global().Get("__dawnResize");
                if (resizeFn.IsFunction())
                {
                    resizeFn.As<Napi::Function>().Call({
                        Napi::Number::New(env, lw),
                        Napi::Number::New(env, lh)});
                }
            });
#else
            impl.m_device->UpdateSize(lw, lh);
#endif
        }
        else
        {
            // "Size known" precondition just satisfied. Silent no-op if
            // currently suspended; Runtime::Resume will retry.
            m_impl->InitializeIfReady();
        }
    }

#if BABYLON_NATIVE_PLUGIN_NATIVEINPUT
    void View::OnPointerDown(int32_t pointerId, float x, float y, CoordinateUnits units)
    {
        RuntimeImpl& impl = m_impl->m_runtime;
        if (impl.m_input)
        {
            const auto [lx, ly] = ToLogicalCoords(x, y, units,
                                                  CurrentDpr(impl, m_impl->m_window));
            impl.m_input->TouchDown(static_cast<uint32_t>(pointerId),
                                     static_cast<int32_t>(lx),
                                     static_cast<int32_t>(ly));
        }
    }

    void View::OnPointerMove(int32_t pointerId, float x, float y, CoordinateUnits units)
    {
        RuntimeImpl& impl = m_impl->m_runtime;
        if (impl.m_input)
        {
            const auto [lx, ly] = ToLogicalCoords(x, y, units,
                                                  CurrentDpr(impl, m_impl->m_window));
            impl.m_input->TouchMove(static_cast<uint32_t>(pointerId),
                                     static_cast<int32_t>(lx),
                                     static_cast<int32_t>(ly));
        }
    }

    void View::OnPointerUp(int32_t pointerId, float x, float y, CoordinateUnits units)
    {
        RuntimeImpl& impl = m_impl->m_runtime;
        if (impl.m_input)
        {
            const auto [lx, ly] = ToLogicalCoords(x, y, units,
                                                  CurrentDpr(impl, m_impl->m_window));
            impl.m_input->TouchUp(static_cast<uint32_t>(pointerId),
                                   static_cast<int32_t>(lx),
                                   static_cast<int32_t>(ly));
        }
    }

    void View::OnMouseDown(uint32_t buttonIndex, float x, float y, CoordinateUnits units)
    {
        RuntimeImpl& impl = m_impl->m_runtime;
        if (impl.m_input)
        {
            const auto [lx, ly] = ToLogicalCoords(x, y, units,
                                                  CurrentDpr(impl, m_impl->m_window));
            impl.m_input->MouseDown(buttonIndex,
                                     static_cast<int32_t>(lx),
                                     static_cast<int32_t>(ly));
        }
    }

    void View::OnMouseUp(uint32_t buttonIndex, float x, float y, CoordinateUnits units)
    {
        RuntimeImpl& impl = m_impl->m_runtime;
        if (impl.m_input)
        {
            const auto [lx, ly] = ToLogicalCoords(x, y, units,
                                                  CurrentDpr(impl, m_impl->m_window));
            impl.m_input->MouseUp(buttonIndex,
                                   static_cast<int32_t>(lx),
                                   static_cast<int32_t>(ly));
        }
    }

    void View::OnMouseMove(float x, float y, CoordinateUnits units)
    {
        RuntimeImpl& impl = m_impl->m_runtime;
        if (impl.m_input)
        {
            const auto [lx, ly] = ToLogicalCoords(x, y, units,
                                                  CurrentDpr(impl, m_impl->m_window));
            impl.m_input->MouseMove(static_cast<int32_t>(lx),
                                     static_cast<int32_t>(ly));
        }
    }

    void View::OnMouseWheel(uint32_t wheelAxis, int32_t scrollValue)
    {
        RuntimeImpl& impl = m_impl->m_runtime;
        if (impl.m_input)
        {
            impl.m_input->MouseWheel(wheelAxis, scrollValue);
        }
    }

    uint32_t View::LeftMouseButton()   { return Babylon::Plugins::NativeInput::LEFT_MOUSE_BUTTON_ID; }
    uint32_t View::MiddleMouseButton() { return Babylon::Plugins::NativeInput::MIDDLE_MOUSE_BUTTON_ID; }
    uint32_t View::RightMouseButton()  { return Babylon::Plugins::NativeInput::RIGHT_MOUSE_BUTTON_ID; }
    uint32_t View::MouseWheelY()       { return Babylon::Plugins::NativeInput::MOUSEWHEEL_Y_ID; }
#endif
}
