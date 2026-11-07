package com.android.babylonnative.playground;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;

import com.babylonjs.embedding.BabylonNative;
import com.library.babylonnative.BabylonView;

public class PlaygroundActivity extends Activity {
    /**
     * Bridges to {@code Apps/Playground/Shared/{CommandLine,PlaygroundScripts}.cpp},
     * which hold the command-line parser and bootstrap script list shared
     * with the other Playground hosts. Implemented in PlaygroundJNI.cpp.
     *
     * @return the positional script URLs from {@code args}.
     */
    private static native String[] loadBootstrapScripts(long runtimeHandle, String[] args);

    private long mRuntimeHandle = 0;
    private BabylonView mView;

    @Override
    protected void onCreate(Bundle icicle) {
        super.onCreate(icicle);

        // Register Context/Activity with AndroidExtensions::Globals (used
        // by NativeCamera, NativeXr). Process-wide, not per-view. The JNI
        // layer guards against double-initialization.
        BabylonNative.setContext(getApplication());
        BabylonNative.setCurrentActivity(this);

        // Activity owns the Runtime lifetime; the View only borrows it.
        BabylonNative.RuntimeOptions runtimeOptions = new BabylonNative.RuntimeOptions();
        runtimeOptions.enableDebugger = true;
        runtimeOptions.enableDebugTrace = true;
        mRuntimeHandle = BabylonNative.runtimeCreate(runtimeOptions);

        // Optional desktop-style command line, e.g.
        //   adb shell am start -n com.android.babylonnative.playground/.PlaygroundActivity \
        //     --es args "app:///Scripts/validation_native.js --test-index=5"
        String argsExtra = getIntent().getStringExtra("args");
        String[] args = argsExtra == null || argsExtra.trim().isEmpty()
                ? new String[0]
                : argsExtra.trim().split("\\s+");

        // Queue the bootstrap scripts + experience (or command-line) scripts.
        // They run after the first View attach completes engine init on the
        // JS thread, in submission order.
        String[] scripts = loadBootstrapScripts(mRuntimeHandle, args);
        if (scripts.length == 0) {
            BabylonNative.runtimeLoadScript(mRuntimeHandle, "app:///Scripts/experience.js");
        } else {
            for (String script : scripts) {
                BabylonNative.runtimeLoadScript(mRuntimeHandle, script);
            }
            BabylonNative.runtimeLoadScript(mRuntimeHandle, "app:///Scripts/playground_runner.js");
        }

        mView = new BabylonView(getApplication(), mRuntimeHandle);

        // Optional fixed render size in CSS pixels (e.g. --ei width 600
        // --ei height 400 for validation tests), applied before the surface
        // is created so the first frame already has the requested size.
        int width = getIntent().getIntExtra("width", 0);
        int height = getIntent().getIntExtra("height", 0);
        if (width > 0 && height > 0) {
            applySurfaceSize(width, height);
        }

        setContentView(mView);
    }

    /** Called from native TestUtils.updateSize (any thread); sizes are in CSS pixels. */
    public void setSurfaceSize(final int width, final int height) {
        runOnUiThread(() -> applySurfaceSize(width, height));
    }

    private void applySurfaceSize(int width, int height) {
        // Same device pixel ratio as the native Device (densityDpi / 160).
        float dpr = getResources().getConfiguration().densityDpi / 160.0f;
        mView.setSurfaceSize(Math.round(width * dpr), Math.round(height * dpr));
    }

    @Override
    protected void onPause() {
        // Hide the view to stop its draw loop; onWindowFocusChanged
        // restores visibility on return.
        mView.setVisibility(View.GONE);

        // Process-wide: every Runtime auto-suspends (each subscribed in
        // runtimeCreate); cross-cutting subsystems (NativeCamera,
        // NativeXr) also hook this via AndroidExtensions::Globals.
        BabylonNative.pause();
        super.onPause();
    }

    @Override
    protected void onResume() {
        super.onResume();
        BabylonNative.resume();
    }

    @Override
    protected void onDestroy() {
        // Surface lifecycle (view detach) has already fired; just
        // release the Runtime.
        if (mRuntimeHandle != 0) {
            BabylonNative.runtimeDestroy(mRuntimeHandle);
            mRuntimeHandle = 0;
        }
        super.onDestroy();
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] results) {
        BabylonNative.requestPermissionsResult(requestCode, permissions, results);
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        if (hasFocus && mView.getVisibility() == View.GONE) {
            mView.setVisibility(View.VISIBLE);
        }
    }
}
