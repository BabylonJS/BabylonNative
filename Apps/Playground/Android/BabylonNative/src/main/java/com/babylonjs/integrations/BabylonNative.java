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

    public static native void runtimeSuspend(long handle);

    public static native void runtimeResume(long handle);

    public static native boolean runtimeIsSuspended(long handle);

    // Compiled into the native library only when BABYLON_NATIVE_PLUGIN_NATIVEXR
    // is enabled. Calling these without that flag will produce an UnsatisfiedLinkError.
    public static native void runtimeSetXrSurface(long handle, Surface surface);

    public static native boolean runtimeIsXrActive(long handle);

    // -------------------------------------------------------------------
    // View
    // -------------------------------------------------------------------

    /**
     * Returns an opaque handle owned by the caller; release with {@link #viewDetach(long)}.
     * {@code physicalWidth} and {@code physicalHeight} are the surface size in
     * physical pixels (Android {@code View.onSizeChanged} units); {@code density}
     * is {@code DisplayMetrics.density} (the physical-to-logical pixel ratio).
     */
    public static native long viewAttach(long runtimeHandle, Surface surface,
                                          int physicalWidth, int physicalHeight, float density);

    public static native void viewDetach(long handle);

    public static native void viewRenderFrame(long handle);

    public static native void viewResize(long handle, int physicalWidth, int physicalHeight, float density);

    public static native void viewPointerDown(long handle, int pointerId, float x, float y);

    public static native void viewPointerMove(long handle, int pointerId, float x, float y);

    public static native void viewPointerUp(long handle, int pointerId, float x, float y);
}
