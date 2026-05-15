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
 *   <li>Create a Runtime via {@link #runtimeCreate()} or {@link #runtimeCreate(RuntimeOptions)} and remember
 *       the returned {@code long} handle.</li>
 *   <li>Optional: queue scripts via {@link #runtimeLoadScript(long, String)}
 *       — they run after the first {@link #viewAttach(long, Surface)}.</li>
 *   <li>Attach a View via {@link #viewAttach(long, Surface)};
 *       call {@link #viewRenderFrame(long)} from your draw loop.</li>
 *   <li>Tear down with {@link #viewDetach(long)} then {@link #runtimeDestroy(long)}.</li>
 * </ol>
 */
public final class BabylonNative {
    /**
     * Construction options for {@link BabylonNative#runtimeCreate(RuntimeOptions)}.
     *
     * <p>The defaults match the C++ {@code Babylon::Integrations::RuntimeOptions}
     * defaults. Fields are intentionally public so Java and Kotlin callers can use
     * simple object-initializer patterns.
     */
    public static final class RuntimeOptions {
        /** Optional MSAA sample count for the back buffer. Valid values are 0, 2, 4, 8, and 16. */
        public Integer msaaSamples = null;

        /** Enable the JavaScript debugger when supported by the configured JS engine. Defaults to false. */
        public boolean enableDebugger = false;

        /** Enable Babylon::DebugTrace output through the default logcat sink. Defaults to false. */
        public boolean enableDebugTrace = false;

        /** Block engine startup until a debugger attaches when supported by the configured JS engine. Defaults to false. */
        public boolean waitForDebugger = false;

        /** Optional writable file path for a persistent on-disk GPU shader cache. */
        public String shaderCachePath = null;
    }

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
    public static native long runtimeCreate();

    /**
     * Returns an opaque handle owned by the caller; release with {@link #runtimeDestroy(long)}.
     *
     * <p>Pass {@code null} to use the same defaults as {@link #runtimeCreate()}.
     * If {@link RuntimeOptions#shaderCachePath} is non-null, the cache is loaded
     * on first {@link #viewAttach(long, Surface)} and saved on suspend and on
     * {@link #runtimeDestroy(long)}.
     *
     * <p>If {@link RuntimeOptions#shaderCachePath} is non-null but the native
     * library was built without {@code BABYLON_NATIVE_PLUGIN_SHADERCACHE}, this
     * method throws {@link IllegalStateException} so the misconfiguration surfaces
     * at construction time rather than silently dropping the cache. Passing null
     * options, or options with null {@code shaderCachePath}, is always safe
     * regardless of native build config.
     */
    public static native long runtimeCreate(RuntimeOptions options);

    public static native void runtimeDestroy(long handle);

    public static native void runtimeLoadScript(long handle, String url);

    public static native void runtimeEval(long handle, String source, String sourceUrl);

    // Note: there is intentionally no per-Runtime Suspend/Resume on the
    // Java surface. Each Runtime auto-subscribes to androidGlobalPause /
    // androidGlobalResume in runtimeCreate, so the host Activity calls
    // those once per state change and every Runtime in the process
    // reacts. Hosts needing finer-grained control should use the C++ API.

    /**
     * Set the platform Surface that XR will render into (typically a
     * separate transparent SurfaceView overlay, distinct from the main
     * View's surface). Pass {@code null} to clear the XR surface.
     *
     * <p>Throws {@link IllegalStateException} if invoked when the
     * native library was built without
     * {@code BABYLON_NATIVE_PLUGIN_NATIVEXR}.
     */
    public static native void runtimeSetXrSurface(long handle, Surface surface);

    /**
     * Returns whether an XR session is currently active. Returns
     * {@code false} (never throws) when the native library was built
     * without {@code BABYLON_NATIVE_PLUGIN_NATIVEXR} — no XR session
     * can ever be active in that build.
     */
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

    /**
     * Push the surface's new pixel-buffer dimensions. Both
     * {@code width} and {@code height} are in **physical pixels** —
     * pass the values you receive from
     * {@code SurfaceHolder.Callback.surfaceChanged} unchanged. The
     * native View divides by the device-pixel-ratio internally
     * before configuring {@code Babylon::Graphics::Device}.
     */
    public static native void viewResize(long handle, int width, int height);

    /**
     * Pointer events. Pass {@code MotionEvent.getX/getY} through
     * unchanged — Android-native physical-pixel coordinates. The
     * native View divides by the device-pixel-ratio internally
     * before forwarding to Babylon.js's
     * {@code PointerEvent.clientX/clientY} pipeline (which expects
     * logical / CSS pixels).
     *
     * <p>Throws {@link IllegalStateException} if invoked when the
     * native library was built without
     * {@code BABYLON_NATIVE_PLUGIN_NATIVEINPUT}.
     */
    public static native void viewPointerDown(long handle, int pointerId, float x, float y);

    public static native void viewPointerMove(long handle, int pointerId, float x, float y);

    public static native void viewPointerUp(long handle, int pointerId, float x, float y);
}
