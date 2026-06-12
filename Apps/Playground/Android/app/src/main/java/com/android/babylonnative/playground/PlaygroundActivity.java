package com.android.babylonnative.playground;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;

import com.babylonjs.embedding.BabylonNative;
import com.library.babylonnative.BabylonView;

public class PlaygroundActivity extends Activity {
    /**
     * Bridges to {@code Apps/Playground/Shared/PlaygroundScripts.cpp},
     * which holds the bootstrap script list shared with the other
     * Playground hosts. Implemented in PlaygroundJNI.cpp.
     */
    private static native void loadBootstrapScripts(long runtimeHandle);

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

        // Queue the bootstrap scripts + experience script. They run after
        // the first View attach completes engine init on the JS thread, in
        // submission order.
        loadBootstrapScripts(mRuntimeHandle);
        BabylonNative.runtimeLoadScript(mRuntimeHandle, "app:///Scripts/experience.js");

        mView = new BabylonView(getApplication(), mRuntimeHandle);
        setContentView(mView);
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
