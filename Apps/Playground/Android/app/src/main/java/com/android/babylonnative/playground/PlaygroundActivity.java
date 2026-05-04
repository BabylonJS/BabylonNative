package com.android.babylonnative.playground;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;

import com.babylonjs.integrations.BabylonNative;
import com.library.babylonnative.BabylonView;

public class PlaygroundActivity extends Activity {
    /** {@link BabylonNative#androidGlobalInitialize} is process-wide; only call it once. */
    private static boolean sGlobalInitDone = false;

    /**
     * Native helper bridging to {@code Apps/Playground/Shared/PlaygroundScripts.cpp},
     * which holds the Babylon.js bootstrap script list shared with the
     * other Playground hosts (Win32, iOS, macOS, …). Implemented in
     * {@code Apps/Playground/Android/BabylonNative/src/main/cpp/PlaygroundJNI.cpp}.
     */
    private static native void loadBootstrapScripts(long runtimeHandle);

    private long mRuntimeHandle = 0;
    private BabylonView mView;

    @Override
    protected void onCreate(Bundle icicle) {
        super.onCreate(icicle);

        // Process-wide one-shot init for AndroidExtensions::Globals
        // (used by NativeCamera, NativeXr, etc.). Belongs at the
        // Activity/Application level — not on a per-view basis — because
        // it broadcasts to process-wide handlers that aren't refcounted.
        if (!sGlobalInitDone) {
            BabylonNative.androidGlobalInitialize(getApplication());
            sGlobalInitDone = true;
        }
        BabylonNative.androidGlobalSetCurrentActivity(this);

        // Owner of the Runtime lifetime: created here, destroyed in
        // onDestroy. The View only borrows the handle for its surface
        // bindings.
        mRuntimeHandle = BabylonNative.runtimeCreate(/*enableDebugger*/ true);

        // Queue the Babylon.js bootstrap scripts, then the playground
        // experience script. Both happen synchronously from this thread;
        // the Runtime queues them internally and runs them after the
        // first View::Attach completes engine initialization on the JS
        // thread, in submission order.
        loadBootstrapScripts(mRuntimeHandle);
        BabylonNative.runtimeLoadScript(mRuntimeHandle, "app:///Scripts/experience.js");

        mView = new BabylonView(getApplication(), mRuntimeHandle);
        setContentView(mView);
    }

    @Override
    protected void onPause() {
        // Hide the view to suppress its draw loop while paused. Visibility
        // is restored by onWindowFocusChanged below when the Activity
        // returns to the foreground.
        mView.setVisibility(View.GONE);

        // Process-wide notification: every Runtime in this process
        // auto-suspends because they each subscribed to this event in
        // BabylonNative.runtimeCreate. Same for cross-cutting subsystems
        // (NativeCamera, NativeXr) that hook AndroidExtensions::Globals.
        BabylonNative.androidGlobalPause();
        super.onPause();
    }

    @Override
    protected void onResume() {
        super.onResume();
        BabylonNative.androidGlobalResume();
    }

    @Override
    protected void onDestroy() {
        // Surface lifecycle (view detach) has already fired by the time
        // we get here; just release the Runtime.
        if (mRuntimeHandle != 0) {
            BabylonNative.runtimeDestroy(mRuntimeHandle);
            mRuntimeHandle = 0;
        }
        super.onDestroy();
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] results) {
        BabylonNative.androidGlobalRequestPermissionsResult(requestCode, permissions, results);
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        if (hasFocus && mView.getVisibility() == View.GONE) {
            mView.setVisibility(View.VISIBLE);
        }
    }
}
