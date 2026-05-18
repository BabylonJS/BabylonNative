#pragma once

#include <Babylon/Graphics/Platform.h>

#include <cstdint>
#include <memory>

namespace Babylon::Integrations
{
    class Runtime;
    struct ViewImpl;

    // Units for coordinates / dimensions passed to View methods. Babylon.js
    // works in **logical** (CSS / DIP) pixels; the View divides by the
    // Device's device-pixel-ratio when given Physical values so hosts can
    // pass whatever their native event/surface API delivers.
    //
    //   Physical: Android MotionEvent / ANativeWindow, Apple
    //             CAMetalLayer.drawableSize, Win32 WM_POINTER* /
    //             GetClientRect, X11 button events.
    //   Logical:  iOS UITouch, macOS NSEvent, UWP PointerPoint /
    //             CoreWindow.Bounds, or any code that already divided by DPR.
    enum class CoordinateUnits
    {
        Physical,
        Logical,
    };

    // Transient: created when a host surface appears, destroyed when it
    // goes away. **At most one View may be attached at a time**; multiple
    // sequential Views may share one Runtime over its lifetime. To switch
    // surfaces, destroy the current View and construct a new one.
    class View
    {
    public:
        // Attach `nativeWindow` (platform-specific surface handle) to `runtime`.
        //
        // `nativeWindow` is `Babylon::Graphics::WindowT` — the same per-platform
        // typedef the Graphics layer uses (HWND, ANativeWindow*, CA::MetalLayer*,
        // X11 Window, winrt::IInspectable). The host owns the surface size; the
        // View binds the window to the Device on the first `Resize`. Hosts MUST
        // call `Resize` at least once before the first frame will be rendered.
        //
        // The first Attach+Resize on a given Runtime is the heavy step: it
        // constructs the Device, initializes GPU plugins (`Device::AddToJavaScript`,
        // `NativeEngine::Initialize`, `NativeInput::CreateForJavaScript`), flushes
        // queued `Runtime::LoadScript` calls, and opens the first frame.
        // Subsequent Attach+Resize calls on the same Runtime just rebind the
        // Device to the new surface. `~View` closes the in-flight frame; the
        // Device persists on the Runtime so the next Attach is fast.
        //
        // Device-rebind work is deferred to the first `Resize` because
        // `Device::UpdateWindow` MUST be paired with a matching `UpdateSize`.
        //
        // Must be called from the "frame thread" — the same thread that will
        // call `RenderFrame` and `Resize`.
        static std::unique_ptr<View> Attach(Runtime& runtime, Babylon::Graphics::WindowT nativeWindow);

        ~View();

        View(const View&) = delete;
        View& operator=(const View&) = delete;
        View(View&&) = delete;
        View& operator=(View&&) = delete;

        // Render exactly one frame on the frame thread. No-op if the runtime
        // is suspended. Called from the platform view/control's draw callback
        // (WM_PAINT, MTKViewDelegate::draw, View.onDraw, ...).
        void RenderFrame();

        // Resize the bound surface. Pass whatever units your view layer reports;
        // the View converts to logical internally when `units == Physical`.
        // Must be called from the frame thread.
        void Resize(uint32_t width, uint32_t height, CoordinateUnits units);

#if BABYLON_NATIVE_PLUGIN_NATIVEINPUT
        // Pointer / mouse input forwarding. Host calls these from its event
        // loop while the view exists; routed to the JS thread via NativeInput
        // and consumed by Babylon.js as PointerEvent.clientX/clientY (logical
        // pixels). Pass whatever units your event system delivers — see
        // CoordinateUnits above.
        //
        // OnPointer* / OnMouse* differ in pointerType ('touch' vs 'mouse').
        // Touch-driven hosts (Android, iOS) use OnPointer*; cursor-driven hosts
        // (Win32, macOS, UWP, X11) use OnMouse*.
        //
        // Keyboard input is not exposed; forward to JS via Runtime::RunOnJsThread.
        //
        // Safe to call from any thread.

        void OnPointerDown(int32_t pointerId, float x, float y, CoordinateUnits units);
        void OnPointerMove(int32_t pointerId, float x, float y, CoordinateUnits units);
        void OnPointerUp(int32_t pointerId, float x, float y, CoordinateUnits units);

        // Mouse events. `buttonIndex` is one of LeftMouseButton() /
        // MiddleMouseButton() / RightMouseButton(); `wheelAxis` is MouseWheelY().
        // These accessors return the matching NativeInput button/axis IDs
        // without leaking the NativeInput header into this public View.h.
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