#include "RuntimeImpl.h"

#include <Babylon/DebugTrace.h>
#include <Babylon/Graphics/DeviceQueries.h>

#include <stdexcept>
#include <string>
#include <utility>

namespace Babylon::Integrations
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
        // Close the in-flight frame iff one is currently open: i.e. the
        // view is initialized and the Runtime is not externally suspended
        // (Suspend already closed the frame). The Device persists on the
        // Runtime so the next View is cheap.
        if (m_initialized &&
            m_runtime.m_suspendCount.load(std::memory_order_relaxed) == 0)
        {
            m_runtime.m_device->FinishRenderingCurrentFrame();
        }

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
        m_runtime.m_device->FinishRenderingCurrentFrame();
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
        m_runtime.m_device->StartRenderingCurrentFrame();
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
            m_runtime.RunFirstAttachInit(m_window);
        }
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
            DEBUG_TRACE("Babylon::Integrations::View::RenderFrame skipped: View has not yet been initialized. Call View::Resize with the surface's pixel dimensions to begin rendering.");
            return;
        }
        if (impl.m_suspendCount.load(std::memory_order_relaxed) > 0)
        {
            return;
        }

        // Babylon's JS render loop runs between Start and Finish, scheduled
        // via DeviceUpdate's SafeTimespanGuarantor onto the JS thread.
        impl.m_device->FinishRenderingCurrentFrame();
        impl.m_device->StartRenderingCurrentFrame();
    }

    // Stores the host-supplied size on the ViewImpl, then either pushes
    // it through Device::UpdateSize (initialized) or drives
    // InitializeIfReady (uninitialized). Single source of truth for
    // surface size in the Integrations layer.
    void View::Resize(uint32_t width, uint32_t height, CoordinateUnits units)
    {
        ValidateNonZeroSize(width, height, "View::Resize size");

        RuntimeImpl& impl = m_impl->m_runtime;

        // DPR source: before init, query the window directly (Device
        // doesn't exist or is still bound to the previous attach). After
        // init, the Device's stored DPR is authoritative.
        const float dpr = !m_impl->m_initialized
            ? Babylon::Graphics::GetDevicePixelRatio(m_impl->m_window)
            : impl.m_device->GetDevicePixelRatio();
        const auto [lw, lh] = ToLogicalSize(width, height, units, dpr);
        ValidateNonZeroSize(lw, lh, "View::Resize logical size");

        m_impl->m_size = {lw, lh};

        if (m_impl->m_initialized)
        {
            impl.m_device->UpdateSize(lw, lh);
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
        if (impl.m_input && impl.m_device)
        {
            const auto [lx, ly] = ToLogicalCoords(x, y, units,
                                                  impl.m_device->GetDevicePixelRatio());
            impl.m_input->TouchDown(static_cast<uint32_t>(pointerId),
                                     static_cast<int32_t>(lx),
                                     static_cast<int32_t>(ly));
        }
    }

    void View::OnPointerMove(int32_t pointerId, float x, float y, CoordinateUnits units)
    {
        RuntimeImpl& impl = m_impl->m_runtime;
        if (impl.m_input && impl.m_device)
        {
            const auto [lx, ly] = ToLogicalCoords(x, y, units,
                                                  impl.m_device->GetDevicePixelRatio());
            impl.m_input->TouchMove(static_cast<uint32_t>(pointerId),
                                     static_cast<int32_t>(lx),
                                     static_cast<int32_t>(ly));
        }
    }

    void View::OnPointerUp(int32_t pointerId, float x, float y, CoordinateUnits units)
    {
        RuntimeImpl& impl = m_impl->m_runtime;
        if (impl.m_input && impl.m_device)
        {
            const auto [lx, ly] = ToLogicalCoords(x, y, units,
                                                  impl.m_device->GetDevicePixelRatio());
            impl.m_input->TouchUp(static_cast<uint32_t>(pointerId),
                                   static_cast<int32_t>(lx),
                                   static_cast<int32_t>(ly));
        }
    }

    void View::OnMouseDown(uint32_t buttonIndex, float x, float y, CoordinateUnits units)
    {
        RuntimeImpl& impl = m_impl->m_runtime;
        if (impl.m_input && impl.m_device)
        {
            const auto [lx, ly] = ToLogicalCoords(x, y, units,
                                                  impl.m_device->GetDevicePixelRatio());
            impl.m_input->MouseDown(buttonIndex,
                                     static_cast<int32_t>(lx),
                                     static_cast<int32_t>(ly));
        }
    }

    void View::OnMouseUp(uint32_t buttonIndex, float x, float y, CoordinateUnits units)
    {
        RuntimeImpl& impl = m_impl->m_runtime;
        if (impl.m_input && impl.m_device)
        {
            const auto [lx, ly] = ToLogicalCoords(x, y, units,
                                                  impl.m_device->GetDevicePixelRatio());
            impl.m_input->MouseUp(buttonIndex,
                                   static_cast<int32_t>(lx),
                                   static_cast<int32_t>(ly));
        }
    }

    void View::OnMouseMove(float x, float y, CoordinateUnits units)
    {
        RuntimeImpl& impl = m_impl->m_runtime;
        if (impl.m_input && impl.m_device)
        {
            const auto [lx, ly] = ToLogicalCoords(x, y, units,
                                                  impl.m_device->GetDevicePixelRatio());
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
