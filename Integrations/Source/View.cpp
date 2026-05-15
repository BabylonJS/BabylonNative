#include "RuntimeImpl.h"

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
    // an existing Runtime) is deferred to the first `View::Resize`.
    //
    // Why deferred: `Device::UpdateWindow` MUST be paired with a
    // matching `Device::UpdateSize` (otherwise bgfx renders the next
    // frame to the new window at the old size). The previous design
    // queried the window's size at Attach time via a per-platform
    // helper, which created two sources of truth for size — the
    // initial query, and the host's subsequent `Resize` calls — and
    // forced the platform integration layer (BNView on Apple) to do
    // gymnastics to ensure the surface had a non-zero size before
    // Attach. Folding both into the first `Resize` makes the host the
    // single source of truth.
    //
    // Render-loop callbacks that fire between `Attach` and the first
    // `Resize` are safe: `m_initialized` is still `false`, so
    // `RenderFrame` / `ViewImpl::Resume` early-out without touching
    // the (potentially still-unbound) Device.
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

        // End the in-flight frame if one is open. Idempotent: if the
        // Runtime was already suspended (which closed the frame via
        // ViewImpl::Suspend), this is a no-op. The Device persists on
        // the Runtime so the next Attach is cheap.
        m_impl->Suspend();

        impl.m_currentView = nullptr;
    }

    // ---------------------------------------------------------------------
    // ViewImpl::Suspend / Resume
    //
    // Idempotent open/close of the in-flight Device frame. Called from:
    //   - View::Resize (first call)  → Resume (open frame after Device
    //                                 binding completes)
    //   - View::~View                → Suspend (close frame at teardown)
    //   - Runtime::Suspend / Resume  → matching call on the currently
    //                                 attached view, if any, so the host's
    //                                 OS-level pause/resume signal cleanly
    //                                 brackets the GPU frame.
    //
    // The internal `m_suspended` flag means "no frame currently open."
    // Initial state is `true`; the first `View::Resize` opens the first
    // frame via Resume. Both methods are no-ops while `m_initialized`
    // is still `false` (no Device binding exists yet for this view, so
    // there is nothing to suspend/resume).
    // ---------------------------------------------------------------------
    void ViewImpl::Suspend()
    {
        if (m_suspended || !m_initialized)
        {
            return;
        }
        RuntimeImpl& impl = *m_runtime.m_impl;
        if (impl.m_device && impl.m_deviceUpdate)
        {
            impl.m_deviceUpdate->Finish();
            impl.m_device->FinishRenderingCurrentFrame();
        }
        m_suspended = true;
    }

    void ViewImpl::Resume()
    {
        if (!m_suspended || !m_initialized)
        {
            return;
        }
        RuntimeImpl& impl = *m_runtime.m_impl;
        if (impl.m_device && impl.m_deviceUpdate)
        {
            impl.m_device->StartRenderingCurrentFrame();
            impl.m_deviceUpdate->Start();
        }
        m_suspended = false;
    }

    void View::RenderFrame()
    {
        RuntimeImpl& impl = *m_impl->m_runtime.m_impl;

        // Skip the GPU work entirely while this view has no open
        // frame: the host can keep calling RenderFrame() from its
        // draw callback unconditionally. `m_suspended` covers every
        // "frame is not currently open" case, including:
        //   - Before the first Resize (m_initialized still false, so
        //     m_suspended has never been flipped).
        //   - Between Runtime::Suspend and Runtime::Resume.
        //   - During teardown (after ~View → Suspend).
        if (m_impl->m_suspended)
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
    // Owns deferred Device initialization. On the first call after
    // `View::Attach`, this is where the Device is constructed (very
    // first Attach on the Runtime) or re-bound to the new surface
    // (subsequent Attach), and where the first frame is opened. Folding
    // `UpdateWindow` + `UpdateSize` together here is required: the two
    // calls MUST stay paired or bgfx will render to the new surface at
    // the old size.
    //
    // On subsequent calls, this is a plain `UpdateSize` against the
    // already-bound Device.
    // ---------------------------------------------------------------------
    void View::Resize(uint32_t width, uint32_t height, CoordinateUnits units)
    {
        ValidateNonZeroSize(width, height, "View::Resize size");

        RuntimeImpl& impl = *m_impl->m_runtime.m_impl;

        if (!m_impl->m_initialized)
        {
            // First Resize on this Attach: bind the Device to the
            // window+size captured at Attach. We must compute DPR from
            // the window directly via the standalone free function
            // because, on a very first Attach, the Device doesn't
            // exist yet; on a re-attach, the Device's stored DPR
            // reflects the previous window and may not match the new
            // one.
            const auto window = m_impl->m_window;
            const float dpr = Babylon::Graphics::GetDevicePixelRatio(window);
            const auto [lw, lh] = ToLogicalSize(width, height, units, dpr);
            ValidateNonZeroSize(lw, lh, "View::Resize logical size");

            const bool firstAttachEver = !impl.m_device;
            if (firstAttachEver)
            {
                Babylon::Graphics::Configuration config{};
                config.Window = window;
                config.Width = lw;
                config.Height = lh;
                config.MSAASamples = impl.m_options.msaaSamples;

                impl.m_device.emplace(config);
                impl.m_deviceUpdate.emplace(impl.m_device->GetUpdate("update"));
            }
            else
            {
                // Re-attach to an existing Runtime: rebind the surface
                // and update the size in lockstep. Plugins, polyfills,
                // and any loaded scripts are preserved on the JS side.
                impl.m_device->UpdateWindow(window);
                impl.m_device->UpdateSize(lw, lh);
            }

            // Flip the initialized latch BEFORE Resume() so the open-
            // frame logic actually runs.
            m_impl->m_initialized = true;

            // Open the first frame. On very first Attach this must
            // happen BEFORE dispatching `RunFirstAttachInit` so the
            // `Device::AddToJavaScript` inside that lambda sees an
            // open frame to record into.
            m_impl->Resume();

            if (firstAttachEver)
            {
                impl.RunFirstAttachInit(window);
            }
            return;
        }

        // Subsequent Resize on an initialized view: plain UpdateSize
        // against the already-bound Device, using the Device's stored
        // DPR (which matches the window the Device is currently bound
        // to).
        const auto [lw, lh] = ToLogicalSize(width, height, units,
                                            impl.m_device->GetDevicePixelRatio());
        ValidateNonZeroSize(lw, lh, "View::Resize logical size");
        impl.m_device->UpdateSize(lw, lh);
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
