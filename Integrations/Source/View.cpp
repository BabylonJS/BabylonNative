#include "RuntimeImpl.h"

#include <Babylon/DebugTrace.h>
#include <Babylon/Graphics/DeviceQueries.h>

#include <cassert>
#include <stdexcept>
#include <string>

namespace Babylon::Integrations
{
    namespace
    {
        // Convert a (width, height) measured in `units` to logical
        // pixels. Caller always passes the real DPR; this helper
        // decides whether to apply it based on `units`.
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

        // Convert a (x, y) coordinate pair measured in `units` to
        // logical pixels.
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

    // ---------------------------------------------------------------------
    // View::Attach
    //
    // Lightweight: just register as the current view and stash the
    // native window handle. All Device interaction (first-time
    // construction, or `UpdateWindow` + `UpdateSize` on a re-attach to
    // an existing Runtime) is deferred to `ViewImpl::InitializeIfReady`,
    // which is called from `View::Resize` and `ViewImpl::Resume` and
    // only actually runs when all three init preconditions hold (see
    // `RuntimeImpl.h` for details).
    //
    // Why deferred: `Device::UpdateWindow` MUST be paired with a
    // matching `Device::UpdateSize` (otherwise bgfx renders the next
    // frame to the new window at the old size), so we wait until the
    // host has supplied a size via `Resize` and the Runtime is not
    // currently externally suspended before binding the new surface.
    // Folding the window-rebind and size-update together inside
    // `InitializeIfReady` makes the host the single source of truth
    // for surface size — the Integrations layer never queries the
    // window for its size.
    //
    // Render-loop callbacks that fire between `Attach` and successful
    // init are safe: `RenderFrame` early-outs while `m_initialized`
    // is `false`, without touching the (potentially still-unbound)
    // Device.
    // ---------------------------------------------------------------------
    std::unique_ptr<View> View::Attach(Runtime& runtime, Babylon::Graphics::WindowT nativeWindow)
    {
        RuntimeImpl& impl = *runtime.m_impl;

        assert(impl.m_currentView == nullptr && "Only one View may be attached at a time.");
        if (impl.m_currentView != nullptr)
        {
            return nullptr;
        }

        std::unique_ptr<View> view{new View{std::make_unique<ViewImpl>(runtime)}};
        view->m_impl->m_window = nativeWindow;
        impl.m_currentView = view.get();
        return view;
    }

    View::View(std::unique_ptr<ViewImpl> impl)
        : m_impl{std::move(impl)}
    {
    }

    View::~View()
    {
        RuntimeImpl& impl = *m_impl->m_runtime.m_impl;

        // Close the in-flight frame iff one is currently open. A frame
        // is open exactly when this view is initialized and the
        // Runtime is not externally suspended (Runtime::Suspend would
        // already have closed the frame via ViewImpl::Suspend). The
        // Device persists on the Runtime so the next Attach is cheap.
        if (m_impl->m_initialized &&
            impl.m_suspendCount.load(std::memory_order_relaxed) == 0)
        {
            impl.m_deviceUpdate->Finish();
            impl.m_device->FinishRenderingCurrentFrame();
        }

        impl.m_currentView = nullptr;
    }

    // ---------------------------------------------------------------------
    // ViewImpl::Suspend / Resume
    //
    // Called by `Runtime::Suspend` / `Runtime::Resume` on the
    // suspendCount 0↔1 transitions (and by `~View` for the teardown
    // close). When the view is already initialized, these are pure
    // frame open/close operations. When the view is not yet
    // initialized, Suspend has nothing to do (no frame exists) and
    // Resume retries `InitializeIfReady` — because the suspendCount
    // dropping to 0 may have been the last missing precondition.
    // ---------------------------------------------------------------------
    void ViewImpl::Suspend()
    {
        if (!m_initialized)
        {
            return;
        }
        RuntimeImpl& impl = *m_runtime.m_impl;
        impl.m_deviceUpdate->Finish();
        impl.m_device->FinishRenderingCurrentFrame();
    }

    void ViewImpl::Resume()
    {
        if (!m_initialized)
        {
            // Runtime just resumed; the suspendCount precondition is
            // now satisfied. If the host has also already called
            // `View::Resize` (size precondition), this will succeed;
            // otherwise it's a silent no-op until they do.
            InitializeIfReady();
            return;
        }
        RuntimeImpl& impl = *m_runtime.m_impl;
        impl.m_device->StartRenderingCurrentFrame();
        impl.m_deviceUpdate->Start();
    }

    // ---------------------------------------------------------------------
    // ViewImpl::InitializeIfReady
    //
    // The single recipe for binding the Device to this view's window
    // and opening the first frame. Gated on all three preconditions
    // (initialized? sized? not externally suspended?) so it can be
    // called eagerly from anywhere that satisfies one of them, and
    // does nothing until the last missing condition is fulfilled.
    // ---------------------------------------------------------------------
    void ViewImpl::InitializeIfReady()
    {
        if (m_initialized || !m_size)
        {
            return;
        }
        RuntimeImpl& impl = *m_runtime.m_impl;
        if (impl.m_suspendCount.load(std::memory_order_relaxed) > 0)
        {
            return;
        }

        const auto [lw, lh] = *m_size;
        const bool firstAttachEver = !impl.m_device;
        if (firstAttachEver)
        {
            Babylon::Graphics::Configuration config{};
            config.Window = m_window;
            config.Width = lw;
            config.Height = lh;
            config.MSAASamples = impl.m_options.msaaSamples;

            impl.m_device.emplace(config);
            impl.m_deviceUpdate.emplace(impl.m_device->GetUpdate("update"));
        }
        else
        {
            // Re-attach to an existing Runtime: rebind the surface and
            // update the size in lockstep. Plugins, polyfills, and any
            // loaded scripts are preserved on the JS side.
            impl.m_device->UpdateWindow(m_window);
            impl.m_device->UpdateSize(lw, lh);
        }

        // Open the first frame inline. On very first Attach this MUST
        // happen BEFORE dispatching `RunFirstAttachInit` so the
        // `Device::AddToJavaScript` inside that lambda sees an open
        // frame to record into. Both happen on the same host thread,
        // so by the time the dispatched lambda runs on the JS thread,
        // the frame is already open regardless of whether we entered
        // here from `View::Resize` or from `ViewImpl::Resume`.
        impl.m_device->StartRenderingCurrentFrame();
        impl.m_deviceUpdate->Start();
        m_initialized = true;

        if (firstAttachEver)
        {
            impl.RunFirstAttachInit(m_window);
        }
    }

    void View::RenderFrame()
    {
        RuntimeImpl& impl = *m_impl->m_runtime.m_impl;

        // Only render when a frame is currently open. The host can keep
        // calling RenderFrame() from its draw callback unconditionally;
        // the two non-rendering cases early-out below.
        if (!m_impl->m_initialized)
        {
            // Flag the pre-init case to help hosts diagnose "my draw
            // callback is firing but nothing renders" mistakes. The
            // externally-suspended case (Runtime::Suspend) is expected
            // behavior and stays silent.
            DEBUG_TRACE("Babylon::Integrations::View::RenderFrame skipped: View has not yet been initialized. Call View::Resize with the surface's pixel dimensions to begin rendering.");
            return;
        }
        if (impl.m_suspendCount.load(std::memory_order_relaxed) > 0)
        {
            return;
        }

        // Babylon's JS render loop (requestAnimationFrame / scene.render)
        // runs between Start and Finish, scheduled via DeviceUpdate onto
        // the JS thread. This call doesn't enter JS directly —
        // DeviceUpdate handles the cross-thread coordination via
        // SafeTimespanGuarantor.
        impl.m_deviceUpdate->Finish();
        impl.m_device->FinishRenderingCurrentFrame();
        impl.m_device->StartRenderingCurrentFrame();
        impl.m_deviceUpdate->Start();
    }

    // ---------------------------------------------------------------------
    // View::Resize
    //
    // Always stores the host-supplied size on the ViewImpl, then either
    // pushes it through to `Device::UpdateSize` (already initialized)
    // or kicks `InitializeIfReady` (still uninitialized). This is the
    // single source of truth for surface size on the Integrations side.
    // ---------------------------------------------------------------------
    void View::Resize(uint32_t width, uint32_t height, CoordinateUnits units)
    {
        ValidateNonZeroSize(width, height, "View::Resize size");

        RuntimeImpl& impl = *m_impl->m_runtime.m_impl;

        // DPR source: before init, query the window directly because
        // the Device either doesn't exist yet (first Attach ever) or is
        // still bound to the previous attach's window (re-attach).
        // After init, the Device's stored DPR matches the window we're
        // bound to and is the authoritative source.
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
            // Just satisfied the "size known" precondition; try to init.
            // Silent no-op if the Runtime is currently externally
            // suspended; the eventual `Runtime::Resume` will retry via
            // `ViewImpl::Resume`.
            m_impl->InitializeIfReady();
        }
    }

#if BABYLON_NATIVE_PLUGIN_NATIVEINPUT
    void View::OnPointerDown(int32_t pointerId, float x, float y, CoordinateUnits units)
    {
        RuntimeImpl& impl = *m_impl->m_runtime.m_impl;
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
        RuntimeImpl& impl = *m_impl->m_runtime.m_impl;
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
        RuntimeImpl& impl = *m_impl->m_runtime.m_impl;
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
        RuntimeImpl& impl = *m_impl->m_runtime.m_impl;
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
        RuntimeImpl& impl = *m_impl->m_runtime.m_impl;
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
        RuntimeImpl& impl = *m_impl->m_runtime.m_impl;
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
        RuntimeImpl& impl = *m_impl->m_runtime.m_impl;
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
