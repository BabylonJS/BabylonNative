package com.babylonjs.integrations;

import android.content.Context;
import android.view.Surface;

/**
 * JVM binding for the C++ Babylon::Integrations layer. The native methods
 * here mirror, byte-for-byte, the {@code extern "C" JNIEXPORT} entry points
 * declared in {@code Integrations/Android/src/main/cpp/BabylonNativeIntegrations.cpp}.
 *
 * <p>This class is a thin facade — it owns no state and exposes the C++
 * API as static methods. Hosts typically wrap it in their own {@code View}
 * subclass (see {@link com.library.babylonnative.BabylonView} for an example).
 *
 * <p>Lifecycle:
 * <ol>
 *   <li>Call {@link #androidGlobalInitialize(Context)} once at app startup
 *       (typically from {@code Application.onCreate}).</li>
 *   <li>Create a Runtime via {@link #runtimeCreate(boolean)} and remember
 *       the returned {@code long} handle.</li>
 *   <li>Optional: queue scripts via {@link #runtimeLoadScript(long, String)}
 *       — they run after the first {@link #viewAttach(long, Surface, int, int, float)}.</li>
 *   <li>Attach a View via {@link #viewAttach(long, Surface, int, int, float)};
 *       call {@link #viewRenderFrame(long)} from your draw loop.</li>
 *   <li>Tear down with {@link #viewDetach(long)} then {@link #runtimeDestroy(long)}.</li>
 * </ol>
 */
public final class BabylonNative {
    static {
        System.loadLibrary("BabylonNativeIntegrations");
    }

    private BabylonNative() {}

    // -------------------------------------------------------------------
    // Process-wide platform lifecycle
    // -------------------------------------------------------------------

    /** Call once at app startup before any other method. */
    public static native void androidGlobalInitialize(Context context);

    public static native void androidGlobalSetCurrentActivity(Object activity);

    public static native void androidGlobalPause();

    public static native void androidGlobalResume();

    public static native void androidGlobalRequestPermissionsResult(
            int requestCode, String[] permissions, int[] grantResults);

    // -------------------------------------------------------------------
    // Runtime
    // -------------------------------------------------------------------

    /** Returns an opaque handle owned by the caller; release with {@link #runtimeDestroy(long)}. */
    public static native long runtimeCreate(boolean enableDebugger);

    public static native void runtimeDestroy(long handle);

    public static native void runtimeLoadScript(long handle, String url);

    public static native void runtimeEval(long handle, String source, String sourceUrl);

    // Note: there is intentionally no per-Runtime Suspend/Resume on the
    // Java surface. Each Runtime auto-subscribes to androidGlobalPause /
    // androidGlobalResume in runtimeCreate, so the host Activity calls
    // those once per state change and every Runtime in the process
    // reacts. Hosts needing finer-grained control should use the C++ API.

    // Compiled into the native library only when BABYLON_NATIVE_PLUGIN_NATIVEXR
    // is enabled. Calling these without that flag will produce an UnsatisfiedLinkError.
    public static native void runtimeSetXrSurface(long handle, Surface surface);

    public static native boolean runtimeIsXrActive(long handle);

    // -------------------------------------------------------------------
    // View
    // -------------------------------------------------------------------

    /**
     * Returns an opaque handle owned by the caller; release with {@link #viewDetach(long)}.
     * {@code width} and {@code height} are the surface size in physical
     * pixels (Android {@code Surface.getSurfaceFrame()} or
     * {@code View.onSizeChanged} units). The Device queries the screen
     * device-pixel-ratio internally; the host does not need to compute
     * or pass it.
     */
    public static native long viewAttach(long runtimeHandle, Surface surface, int width, int height);

    public static native void viewDetach(long handle);

    public static native void viewRenderFrame(long handle);

    public static native void viewResize(long handle, int width, int height);

    /**
     * Pointer events. {@code physicalX}/{@code physicalY} are the raw
     * coordinates from {@code MotionEvent.getX/getY} (physical pixels);
     * the native layer converts them to logical (CSS) pixels using
     * the underlying Device's queried device-pixel-ratio before
     * forwarding to NativeInput, matching the browser's
     * {@code PointerEvent.clientX/clientY} convention that Babylon.js
     * consumes.
     */
    public static native void viewPointerDown(long handle, int pointerId, float physicalX, float physicalY);

    public static native void viewPointerMove(long handle, int pointerId, float physicalX, float physicalY);

    public static native void viewPointerUp(long handle, int pointerId, float physicalX, float physicalY);
}
