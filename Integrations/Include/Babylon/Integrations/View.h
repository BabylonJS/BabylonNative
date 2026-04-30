#pragma once

#include <Babylon/Integrations/ViewDescriptor.h>

#include <cstdint>
#include <memory>

namespace Babylon::Integrations
{
    class Runtime;
    struct ViewImpl;

    // Transient: created when a host surface appears, destroyed when
    // it goes away. Multiple sequential Views may be attached to the
    // same Runtime over its lifetime. **At most one View may be attached
    // at a time** — to switch surfaces, destroy the current View and
    // construct a new one.
    //
    // See SimplifiedAPI.md §4.1 for the full design.
    class View
    {
    public:
        // Attaches a surface described by `descriptor` to `runtime`.
        //
        // The first Attach on a given Runtime is the heavy step: it
        // constructs `Babylon::Graphics::Device` against `descriptor`,
        // dispatches GPU plugin initialization (`Device::AddToJavaScript`,
        // `NativeEngine::Initialize`, `NativeInput::CreateForJavaScript`,
        // ...), and flushes any scripts queued via `Runtime::LoadScript`
        // before this point. Opens the first frame.
        //
        // Subsequent Attach calls on the same Runtime are cheap: the
        // Device is already constructed, plugins are initialized, the
        // JS engine is running. They just call `Device::UpdateWindow` +
        // `Device::EnableRendering` to bind the new surface, then open
        // the first frame for the new attachment.
        //
        // Detach (`~View`) closes the in-flight frame and calls
        // `Device::DisableRendering`. The Device persists on the
        // Runtime, so the next Attach is fast.
        //
        // Must be called from the same thread that will call
        // `RenderFrame` and `Resize` (the "frame thread").
        static std::unique_ptr<View> Attach(Runtime& runtime, const ViewDescriptor& descriptor);

        ~View();

        View(const View&) = delete;
        View& operator=(const View&) = delete;
        View(View&&) = delete;
        View& operator=(View&&) = delete;

        // Render exactly one frame. Must be called from the same thread
        // as `Attach` and `Resize` (the frame thread). No-op if the
        // runtime is suspended. The host calls this from the platform
        // view/control's existing draw callback (WM_PAINT on Win32,
        // MTKViewDelegate::draw(in:) on Apple, View.onDraw on Android,
        // etc. — see SimplifiedAPI.md §4.1 "How frames actually get
        // rendered").
        void RenderFrame();

        // Resize the bound surface. `width` and `height` are in
        // **logical pixels**; `devicePixelRatio` is the physical-to-logical
        // ratio (e.g. 2.0 for a Retina display).
        //
        // The platform interop layer is responsible for converting
        // whatever its UI framework provides into this convention
        // (Android `View.onSizeChanged` is in physical pixels; iOS
        // `MTKViewDelegate.drawableSizeWillChange:` is in physical
        // pixels; UWP `SwapChainPanel.SizeChanged` is in logical pixels;
        // etc.). See SimplifiedAPI.md §4.2 "Pixel units".
        //
        // Must be called from the frame thread.
        void Resize(uint32_t width, uint32_t height, float devicePixelRatio = 1.0f);

#if BABYLON_NATIVE_PLUGIN_NATIVEINPUT
        // ----- Pointer input forwarding -----
        //
        // Host calls these from its event loop while the view exists.
        // Routed to the JS thread via `NativeInput::Touch*`. Coordinates
        // are in logical pixels (same convention as Resize).
        //
        // Babylon Native's `NativeInput` only exposes pointer (touch /
        // mouse-as-pointer) input today; keyboard input is not part of
        // the public Babylon Native input contract and is therefore not
        // exposed here. Hosts that need keyboard handling can do it at
        // the platform level and forward into JS via Runtime::RunOnJsThread.
        //
        // Safe to call from any thread.
        void OnPointerDown(int32_t pointerId, float x, float y);
        void OnPointerMove(int32_t pointerId, float x, float y);
        void OnPointerUp(int32_t pointerId, float x, float y);
#endif

    private:
        friend class Runtime;

        std::unique_ptr<ViewImpl> m_impl;

        explicit View(std::unique_ptr<ViewImpl> impl);
    };
}
