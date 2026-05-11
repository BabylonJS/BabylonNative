#include "RuntimeImpl.h"

#include <cassert>

namespace Babylon::Integrations
{
    // ---------------------------------------------------------------------
    // View::Attach (first time and subsequent)
    // ---------------------------------------------------------------------
    std::unique_ptr<View> View::Attach(Runtime& runtime, Babylon::Graphics::WindowT nativeWindow)
    {
        RuntimeImpl& impl = *runtime.m_impl;

        assert(impl.m_currentView == nullptr && "Only one View may be attached at a time.");
        if (impl.m_currentView != nullptr)
        {
            return nullptr;
        }

        const bool firstAttach = !impl.m_device;

        // Per-platform: query the surface's pixel-buffer size from the
        // native window handle. ViewImpl_*.cpp implements this; e.g.
        // ANativeWindow_getWidth on Android, GetClientRect on Win32,
        // CAMetalLayer.drawableSize on Apple.
        const auto [width, height] = ViewImpl::QuerySize(nativeWindow);

        if (firstAttach)
        {
            // First Attach on this Runtime: construct the Device.
            Babylon::Graphics::Configuration config{};
            config.Window = nativeWindow;
            config.Width = width;
            config.Height = height;
            config.MSAASamples = impl.m_options.msaaSamples;

            impl.m_device.emplace(config);
            impl.m_deviceUpdate.emplace(impl.m_device->GetUpdate("update"));
        }
        else
        {
            // Subsequent Attach: reuse the existing Device, just rebind
            // the surface. Plugins, polyfills, and any loaded scripts
            // are preserved on the JS side.
            impl.m_device->UpdateWindow(nativeWindow);
            impl.m_device->UpdateSize(width, height);
        }

        std::unique_ptr<View> view{new View{std::make_unique<ViewImpl>(runtime)}};
        impl.m_currentView = view.get();

        // Open the first frame via ViewImpl::Resume (which flips
        // m_suspended → false). On first Attach, this must happen
        // BEFORE dispatching the engine-init lambda so the
        // Device::AddToJavaScript inside the lambda sees an open
        // frame to record into.
        view->m_impl->Resume();

        if (firstAttach)
        {
            impl.RunFirstAttachInit(nativeWindow);
        }

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
    //   - View::Attach            → Resume (open frame after Device setup)
    //   - View::~View             → Suspend (close frame at teardown)
    //   - Runtime::Suspend / Resume → matching call on the currently
    //                                 attached view, if any, so the host's
    //                                 OS-level pause/resume signal cleanly
    //                                 brackets the GPU frame.
    //
    // The internal `m_suspended` flag means "no frame currently open."
    // Initial state is `true`; the very first Resume opens the first
    // frame. Multiple Suspend or Resume calls in a row are no-ops.
    // ---------------------------------------------------------------------
    void ViewImpl::Suspend()
    {
        if (m_suspended)
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
        if (!m_suspended)
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

        // Skip the GPU work entirely while this view is suspended;
        // the host can keep calling RenderFrame() from its draw
        // callback unconditionally. The view's `m_suspended` flag is
        // flipped by Runtime::Suspend/Resume (and by ~View / Attach
        // for the destruction / construction boundaries), so this
        // check covers every "frame is not currently open" case
        // including: pre-Attach, between Suspend and Resume, and
        // during teardown.
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

    void View::Resize(uint32_t width, uint32_t height)
    {
        RuntimeImpl& impl = *m_impl->m_runtime.m_impl;
        if (impl.m_device)
        {
            impl.m_device->UpdateSize(width, height);
        }
    }

#if BABYLON_NATIVE_PLUGIN_NATIVEINPUT
    void View::OnPointerDown(int32_t pointerId, float x, float y)
    {
        RuntimeImpl& impl = *m_impl->m_runtime.m_impl;
        if (impl.m_input)
        {
            const auto [lx, ly] = m_impl->ToLogicalCoords(x, y);
            impl.m_input->TouchDown(static_cast<uint32_t>(pointerId),
                                     static_cast<int32_t>(lx),
                                     static_cast<int32_t>(ly));
        }
    }

    void View::OnPointerMove(int32_t pointerId, float x, float y)
    {
        RuntimeImpl& impl = *m_impl->m_runtime.m_impl;
        if (impl.m_input)
        {
            const auto [lx, ly] = m_impl->ToLogicalCoords(x, y);
            impl.m_input->TouchMove(static_cast<uint32_t>(pointerId),
                                     static_cast<int32_t>(lx),
                                     static_cast<int32_t>(ly));
        }
    }

    void View::OnPointerUp(int32_t pointerId, float x, float y)
    {
        RuntimeImpl& impl = *m_impl->m_runtime.m_impl;
        if (impl.m_input)
        {
            const auto [lx, ly] = m_impl->ToLogicalCoords(x, y);
            impl.m_input->TouchUp(static_cast<uint32_t>(pointerId),
                                   static_cast<int32_t>(lx),
                                   static_cast<int32_t>(ly));
        }
    }

    void View::OnMouseDown(uint32_t buttonIndex, float x, float y)
    {
        RuntimeImpl& impl = *m_impl->m_runtime.m_impl;
        if (impl.m_input)
        {
            const auto [lx, ly] = m_impl->ToLogicalCoords(x, y);
            impl.m_input->MouseDown(buttonIndex,
                                     static_cast<int32_t>(lx),
                                     static_cast<int32_t>(ly));
        }
    }

    void View::OnMouseUp(uint32_t buttonIndex, float x, float y)
    {
        RuntimeImpl& impl = *m_impl->m_runtime.m_impl;
        if (impl.m_input)
        {
            const auto [lx, ly] = m_impl->ToLogicalCoords(x, y);
            impl.m_input->MouseUp(buttonIndex,
                                   static_cast<int32_t>(lx),
                                   static_cast<int32_t>(ly));
        }
    }

    void View::OnMouseMove(float x, float y)
    {
        RuntimeImpl& impl = *m_impl->m_runtime.m_impl;
        if (impl.m_input)
        {
            const auto [lx, ly] = m_impl->ToLogicalCoords(x, y);
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
