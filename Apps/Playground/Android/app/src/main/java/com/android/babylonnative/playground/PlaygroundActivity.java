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
    private static native void loadMobileTestScripts(long runtimeHandle, String endpoint);

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

        // Queue the bootstrap scripts + selected entry script. They run after
        // the first View attach completes engine init on the JS thread, in
        // submission order.
        loadBootstrapScripts(mRuntimeHandle);
        String endpoint = BuildConfig.MOBILE_VISUAL_TESTS
                ? getIntent().getStringExtra("mobileTestUrl") : null;
        if (endpoint != null) {
            android.net.Uri uri = android.net.Uri.parse(endpoint);
            if (!"http".equals(uri.getScheme()) ||
                    !("bs-local.com".equals(uri.getHost()) || "127.0.0.1".equals(uri.getHost())) ||
                    uri.getPort() < 1 || uri.getQuery() != null || uri.getUserInfo() != null ||
                    uri.getFragment() != null || uri.getPath() == null ||
                    !uri.getPath().matches("/device/[a-f0-9-]{36}")) {
                throw new IllegalArgumentException("Invalid mobile test controller endpoint");
            }
            loadMobileTestScripts(mRuntimeHandle, endpoint);
        } else {
            BabylonNative.runtimeLoadScript(mRuntimeHandle, "app:///Scripts/experience.js");
        }

        mView = new BabylonView(getApplication(), mRuntimeHandle);
        if (endpoint != null) {
            mView.setRenderSize(600, 400);
        }
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
