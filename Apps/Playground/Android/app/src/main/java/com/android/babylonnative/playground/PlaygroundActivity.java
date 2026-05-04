package com.android.babylonnative.playground;

import android.Manifest;
import android.app.Activity;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.view.View;

import com.babylonjs.integrations.BabylonNative;
import com.library.babylonnative.BabylonView;

public class PlaygroundActivity extends Activity implements BabylonView.ViewDelegate {
    /** {@link BabylonNative#androidGlobalInitialize} is process-wide; only call it once. */
    private static boolean sGlobalInitDone = false;

    BabylonView mView;

    // Activity life
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

        mView = new BabylonView(getApplication(), this);
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

    @Override
    public void onViewReady() {
        mView.loadScript("app:///Scripts/experience.js");
    }
}
