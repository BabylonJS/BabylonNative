package com.babylonjs.embedding;

import android.content.Context;
import android.view.Surface;

/**
 * JVM binding for the C++ Babylon::Embedding layer. Native methods
 * mirror byte-for-byte the {@code extern "C" JNIEXPORT} entry points in
 * {@code Embedding/Android/src/main/cpp/BabylonNativeEmbedding.cpp}.
 *
 * <p>Thin facade — owns no state, exposes the C++ API as static methods.
 * Hosts typically wrap it in their own {@code View} subclass (see
 * {@link com.library.babylonnative.BabylonView}).
 *
 * <p>Lifecycle:
 * <ol>
 *   <li>{@link #setContext(Context)} once at app startup.</li>
 *   <li>Create a Runtime via {@link #runtimeCreate()} or
 *       {@link #runtimeCreate(RuntimeOptions)} and keep the handle.</li>
 *   <li>Optional: queue scripts via {@link #runtimeLoadScript(long, String)};
 *       they run after the first {@link #viewAttach(long, Surface)}.</li>
 *   <li>Attach a View and drive {@link #viewRenderFrame(long)} from your draw loop.</li>
 *   <li>Tear down with {@link #viewDetach(long)} then {@link #runtimeDestroy(long)}.</li>
 * </ol>
 */
public final class BabylonNative {
    /**
     * Construction options. Defaults match the C++
     * {@code Babylon::Embedding::RuntimeOptions}. Fields are public for
     * simple object-initializer patterns.
     */
    public static final class RuntimeOptions {
        /** Optional MSAA samples (0, 2, 4, 8, 16). */
        public Integer msaaSamples = null;

        /** Enable the JavaScript debugger (engine-dependent). */
        public boolean enableDebugger = false;

        /** Enable Babylon::DebugTrace output through the logcat sink. */
        public boolean enableDebugTrace = false;

        /** Block engine startup until a debugger attaches (engine-dependent). */
        public boolean waitForDebugger = false;

        /** Optional persistent on-disk shader cache path. */
        public String shaderCachePath = null;
    }

    static {
        System.loadLibrary("BabylonNativeEmbedding");
    }

    private BabylonNative() {}

    // -------------------------------------------------------------------
    // Process-wide platform lifecycle
    // -------------------------------------------------------------------

    /**
     * Register the application Context. Call once at app startup
     * (typically from {@code Application.onCreate}) before constructing
     * any Runtime. Repeated calls replace the existing Context global ref.
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

    /** Returns an opaque handle; release with {@link #runtimeDestroy(long)}. */
    public static native long runtimeCreate();

    /**
     * Returns an opaque handle; release with {@link #runtimeDestroy(long)}.
     * Pass {@code null} for the same defaults as {@link #runtimeCreate()}.
     *
     * <p>If {@link RuntimeOptions#shaderCachePath} is non-null, the cache
     * is loaded on first {@link #viewAttach(long, Surface)} and saved on
     * suspend / destroy. Throws {@link IllegalStateException} when
     * shaderCachePath is set but {@code BABYLON_NATIVE_PLUGIN_SHADERCACHE}
     * is disabled in the native build.
     */
    public static native long runtimeCreate(RuntimeOptions options);

    public static native void runtimeDestroy(long handle);

    public static native void runtimeLoadScript(long handle, String url);

    public static native void runtimeEval(long handle, String source, String sourceUrl);

    // No per-Runtime Suspend/Resume here: each Runtime auto-subscribes to
    // pause/resume in runtimeCreate. Hosts call those once per Activity
    // state change and every Runtime reacts. Use the C++ API for
    // finer-grained control.

    /**
     * Set the Surface that XR renders into (typically a transparent
     * SurfaceView overlay). Pass {@code null} to clear. Throws
     * {@link IllegalStateException} when {@code BABYLON_NATIVE_PLUGIN_NATIVEXR}
     * is disabled in the native build.
     */
    public static native void runtimeSetXrSurface(long handle, Surface surface);

    /**
     * Whether an XR session is active. Returns {@code false} (never
     * throws) when {@code BABYLON_NATIVE_PLUGIN_NATIVEXR} is disabled.
     */
    public static native boolean runtimeIsXrActive(long handle);

    // -------------------------------------------------------------------
    // View
    // -------------------------------------------------------------------

    /** Returns an opaque handle; release with {@link #viewDetach(long)}. */
    public static native long viewAttach(long runtimeHandle, Surface surface);

    public static native void viewDetach(long handle);

    public static native void viewRenderFrame(long handle);

    /**
     * Push the surface's new pixel-buffer dimensions. {@code width} and
     * {@code height} are in physical pixels — pass the values from
     * {@code SurfaceHolder.Callback.surfaceChanged} unchanged. The native
     * View divides by DPR internally.
     */
    public static native void viewResize(long handle, int width, int height);

    /**
     * Pointer events. Pass {@code MotionEvent.getX/getY} through
     * unchanged (physical pixels); native View converts internally
     * before forwarding to Babylon.js's clientX/clientY pipeline.
     *
     * <p>Throws {@link IllegalStateException} when
     * {@code BABYLON_NATIVE_PLUGIN_NATIVEINPUT} is disabled.
     */
    public static native void viewPointerDown(long handle, int pointerId, float x, float y);

    public static native void viewPointerMove(long handle, int pointerId, float x, float y);

    public static native void viewPointerUp(long handle, int pointerId, float x, float y);
}
