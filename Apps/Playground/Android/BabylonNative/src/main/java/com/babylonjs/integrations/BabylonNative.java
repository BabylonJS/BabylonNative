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

    /**
     * Register the application Context. Hosts call this once at app
     * startup (typically from {@code Application.onCreate} or the host
     * Activity's {@code onCreate}), before constructing any Runtime.
     * Calling more than once is harmless — the underlying
     * {@code android::global::Initialize} replaces the existing
     * Context global ref.
     */
    public static native void setContext(Context context);

    public static native void setCurrentActivity(Object activity);

    public static native void pause();

    public static native void resume();

    public static native void requestPermissionsResult(
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
     * The View queries the surface's pixel-buffer size from the native
     * window itself, and the Device queries the screen device-pixel-ratio
     * from the system, so no dimensional info needs to be passed from Java.
     */
    public static native long viewAttach(long runtimeHandle, Surface surface);

    public static native void viewDetach(long handle);

    public static native void viewRenderFrame(long handle);

    public static native void viewResize(long handle, int width, int height);

    /**
     * Pointer events. Pass {@code MotionEvent.getX/getY} through unchanged
     * (Android-native physical-pixel coordinates). The View internally
     * normalizes to logical (CSS) pixels — the unit Babylon.js's
     * {@code PointerEvent.clientX/clientY} pipeline expects.
     */
    public static native void viewPointerDown(long handle, int pointerId, float x, float y);

    public static native void viewPointerMove(long handle, int pointerId, float x, float y);

    public static native void viewPointerUp(long handle, int pointerId, float x, float y);
}
