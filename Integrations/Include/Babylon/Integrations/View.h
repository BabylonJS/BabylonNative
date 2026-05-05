#pragma once

#include <Babylon/Graphics/Platform.h>

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
        // Attach `nativeWindow` (the platform-specific surface handle)
        // to `runtime`.
        //
        // `nativeWindow` is `Babylon::Graphics::WindowT`, the same
        // per-platform typedef the Graphics layer already uses
        // (HWND on Win32, ANativeWindow* on Android, CA::MetalLayer*
        // on Apple, X11 `Window` on Linux, winrt::IInspectable on UWP).
        //
        // `width` and `height` are in **physical pixels** — the actual
        // pixel-buffer dimensions of the surface. Hosts pass through
        // whatever their platform's window/view delivers (Android's
        // `Surface.getSurfaceFrame()`, Apple's `CAMetalLayer.drawableSize`,
        // Win32's `GetClientRect`, etc.). The Device queries the screen
        // device-pixel-ratio internally; the host doesn't need to
        // compute or pass it.
        //
        // The first Attach on a given Runtime is the heavy step: it
        // constructs `Babylon::Graphics::Device`, dispatches GPU plugin
        // initialization (`Device::AddToJavaScript`,
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
        static std::unique_ptr<View> Attach(Runtime& runtime, Babylon::Graphics::WindowT nativeWindow, uint32_t width, uint32_t height);

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
        // **physical pixels** — the actual pixel-buffer dimensions of
        // the surface the GPU will render into. Hosts pass through
        // whatever their platform's view layer reports (e.g.
        // Android's `View.onSizeChanged` w/h, iOS's
        // `MTKViewDelegate.drawableSizeWillChange:` size).
        //
        // Must be called from the frame thread.
        void Resize(uint32_t width, uint32_t height);

#if BABYLON_NATIVE_PLUGIN_NATIVEINPUT
        // ----- Pointer / mouse input forwarding -----
        //
        // Host calls these from its event loop while the view exists.
        // Routed to the JS thread via `NativeInput`, where Babylon.js
        // consumes them as `PointerEvent.clientX/clientY`.
        //
        // **Coordinates are in logical (CSS) pixels** — the same unit
        // a browser would deliver. This matches Babylon.js's existing
        // pointer pipeline: `clientX/clientY` are CSS pixels regardless
        // of the canvas backing buffer's physical resolution.
        //
        // Some platforms' native pointer events are already in logical
        // units (iOS `UITouch` points, macOS `NSEvent`, UWP
        // `PointerPoint` at `RasterizationScale = 1`); others deliver
        // physical pixels (Android `MotionEvent.getX/getY`, Win32
        // `WM_POINTER*`). The host or interop layer is responsible for
        // dividing by `DevicePixelRatio()` (below) when its native
        // event system delivers physical pixels.
        //
        // Babylon Native distinguishes pointer (touch) input from mouse
        // input; both methods feed the same Babylon.js pointer-event
        // pipeline but with different `pointerType` ('touch' vs.
        // 'mouse'). Hosts driven by touch (Android, iOS) typically use
        // OnPointer*; hosts driven by a cursor (Win32, macOS, UWP, X11)
        // typically use OnMouse*.
        //
        // Babylon Native does not currently expose keyboard input; hosts
        // that need keyboard handling do it at the platform level and
        // forward into JS via `Runtime::RunOnJsThread`.
        //
        // Safe to call from any thread.

        // Touch / pointer events.
        void OnPointerDown(int32_t pointerId, float x, float y);
        void OnPointerMove(int32_t pointerId, float x, float y);
        void OnPointerUp(int32_t pointerId, float x, float y);

        // Mouse events. `buttonIndex` is one of LeftMouseButton(),
        // MiddleMouseButton(), RightMouseButton(); `wheelAxis` is
        // MouseWheelY(). The accessors return the matching
        // `Babylon::Plugins::NativeInput::*_ID` value (single source of
        // truth — no duplication, no risk of drift) without exposing the
        // NativeInput header from this public View.h.
        void OnMouseDown(uint32_t buttonIndex, float x, float y);
        void OnMouseUp(uint32_t buttonIndex, float x, float y);
        void OnMouseMove(float x, float y);
        void OnMouseWheel(uint32_t wheelAxis, int32_t scrollValue);

        static uint32_t LeftMouseButton();
        static uint32_t MiddleMouseButton();
        static uint32_t RightMouseButton();
        static uint32_t MouseWheelY();

        // Current screen device-pixel-ratio (physical/logical pixel
        // ratio), as queried from the platform by the underlying
        // `Babylon::Graphics::Device`. Use this to convert physical
        // pointer coordinates to the logical pixels the OnPointer* /
        // OnMouse* APIs expect, on platforms where the native event
        // system delivers physical pixels (Android, Win32).
        //
        // Only valid post-Attach (the Device is constructed there);
        // returns 1.0 if called pre-Attach.
        float DevicePixelRatio() const;
#endif

    private:
        friend class Runtime;

        std::unique_ptr<ViewImpl> m_impl;

        explicit View(std::unique_ptr<ViewImpl> impl);
    };
}
