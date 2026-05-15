#pragma once

#include <Babylon/Graphics/Platform.h>

#include <cstdint>
#include <memory>

namespace Babylon::Integrations
{
    class Runtime;
    struct ViewImpl;

    // Tag for the units of coordinates and dimensions handed to View
    // methods. Babylon.js consumes pointer events and render-target
    // sizes in **logical** (CSS / DIP) pixels; the View internally
    // divides by the Device's queried device-pixel-ratio when given
    // values in **physical** (surface pixel-buffer) units, so hosts
    // pass through whatever their platform's native event system /
    // surface-size API delivers.
    //
    // Examples:
    //   Physical: Android `MotionEvent.getX/getY` /
    //             `ANativeWindow_getWidth/Height`,
    //             Apple `CAMetalLayer.drawableSize`,
    //             Win32 `GetClientRect` / `WM_POINTER*` (DPI-aware),
    //             X11 button events / `XGetGeometry`.
    //   Logical:  iOS `UITouch.location`,
    //             macOS `NSEvent.locationInWindow`,
    //             UWP `PointerPoint.Position` / `CoreWindow.Bounds`,
    //             host code that has already done its own DPR divide.
    enum class CoordinateUnits
    {
        Physical,
        Logical,
    };

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
        // The host owns the surface size: the View captures the
        // window handle here and binds it to the Device on the first
        // `Resize` call. Hosts MUST call `Resize` at least once
        // (with the surface's current pixel dimensions) before the
        // first frame will be rendered.
        //
        // Attach itself is lightweight — it just registers as the
        // current view and stashes the window handle. All Device
        // work (first-time construction, or `UpdateWindow` +
        // `UpdateSize` on a re-attach to an existing Runtime) is
        // performed by the first `Resize` call, where the host-
        // supplied dimensions become available. This folding is
        // required: `Device::UpdateWindow` MUST be paired with a
        // matching `Device::UpdateSize` or the next frame would be
        // rendered to the new surface at the wrong size.
        //
        // The first Attach + Resize on a given Runtime is the heavy
        // step: the Resize constructs `Babylon::Graphics::Device`,
        // dispatches GPU plugin initialization
        // (`Device::AddToJavaScript`, `NativeEngine::Initialize`,
        // `NativeInput::CreateForJavaScript`, ...), and flushes any
        // scripts queued via `Runtime::LoadScript` before this point.
        // Opens the first frame.
        //
        // Subsequent Attach + Resize calls on the same Runtime are
        // cheap: the Device is already constructed, plugins are
        // initialized, the JS engine is running. They just call
        // `Device::UpdateWindow` + `Device::UpdateSize` to bind the
        // new surface, then open the first frame for the new
        // attachment.
        //
        // Detach (`~View`) closes the in-flight frame. The Device
        // persists on the Runtime, so the next Attach is fast.
        //
        // Must be called from the same thread that will call
        // `RenderFrame` and `Resize` (the "frame thread").
        static std::unique_ptr<View> Attach(Runtime& runtime, Babylon::Graphics::WindowT nativeWindow);

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

        // Resize the bound surface. The View converts physical →
        // logical internally if `units == CoordinateUnits::Physical`,
        // so the host can pass whatever its platform's view layer
        // reports without doing the DPR divide.
        //
        // Examples:
        //   Android `View.onSizeChanged(w, h)` → Physical.
        //   iOS `MTKViewDelegate.drawableSizeWillChange:` → Physical.
        //   UWP `SizeChangedEventArgs.NewSize` (already in DIPs) → Logical.
        //
        // Must be called from the frame thread.
        void Resize(uint32_t width, uint32_t height, CoordinateUnits units);

#if BABYLON_NATIVE_PLUGIN_NATIVEINPUT
        // ----- Pointer / mouse input forwarding -----
        //
        // Host calls these from its event loop while the view exists.
        // Routed to the JS thread via `NativeInput`, where Babylon.js
        // consumes them as `PointerEvent.clientX/clientY` (logical /
        // CSS pixels). The View converts physical → logical internally
        // if `units == CoordinateUnits::Physical`.
        //
        // Pass coordinates in whatever unit your platform's native
        // event system delivers:
        //   Physical: Android `MotionEvent.getX/getY`,
        //             Win32 `WM_POINTER*` / `WM_MOUSE*` (DPI-aware),
        //             X11 button events.
        //   Logical:  iOS `UITouch.location`,
        //             macOS `NSEvent.locationInWindow`,
        //             UWP `PointerPoint.Position`.
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
        void OnPointerDown(int32_t pointerId, float x, float y, CoordinateUnits units);
        void OnPointerMove(int32_t pointerId, float x, float y, CoordinateUnits units);
        void OnPointerUp(int32_t pointerId, float x, float y, CoordinateUnits units);

        // Mouse events. `buttonIndex` is one of LeftMouseButton(),
        // MiddleMouseButton(), RightMouseButton(); `wheelAxis` is
        // MouseWheelY(). The accessors return the matching
        // `Babylon::Plugins::NativeInput::*_ID` value (single source of
        // truth — no duplication, no risk of drift) without exposing the
        // NativeInput header from this public View.h.
        void OnMouseDown(uint32_t buttonIndex, float x, float y, CoordinateUnits units);
        void OnMouseUp(uint32_t buttonIndex, float x, float y, CoordinateUnits units);
        void OnMouseMove(float x, float y, CoordinateUnits units);
        void OnMouseWheel(uint32_t wheelAxis, int32_t scrollValue);

        static uint32_t LeftMouseButton();
        static uint32_t MiddleMouseButton();
        static uint32_t RightMouseButton();
        static uint32_t MouseWheelY();
#endif

    private:
        friend class Runtime;

        std::unique_ptr<ViewImpl> m_impl;

        explicit View(std::unique_ptr<ViewImpl> impl);
    };
}