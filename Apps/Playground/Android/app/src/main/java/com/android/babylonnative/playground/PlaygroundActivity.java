package com.android.babylonnative.playground;

import android.Manifest;
import android.app.Activity;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.view.View;

import com.library.babylonnative.BabylonView;
import com.library.babylonnative.Wrapper;

public class PlaygroundActivity extends Activity implements BabylonView.ViewDelegate {
    BabylonView mView;

    // Activity life
    @Override
    protected void onCreate(Bundle icicle) {
        super.onCreate(icicle);
        mView = new BabylonView(getApplication(), this);
        setContentView(mView);
    }

    @Override
    protected void onPause() {
        mView.onPause();
        super.onPause();
    }

    @Override
    protected void onResume() {
        super.onResume();
        mView.onResume();
    }

    @Override
    protected void onDestroy() {
        Wrapper.finishEngine();
        super.onDestroy();
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] results) {
        mView.onRequestPermissionsResult(requestCode, permissions, results);
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
        // Clear previous launch globals first so the runner cannot pick up stale
        // createScene/signal values from an older runtime. This is needed because
        // Android can recreate the native surface (and thus re-run this callback)
        // during activity lifecycle events (pause/resume, orientation change, etc.).
        // Note: navigator.gpu and _native.Canvas do NOT need clearing — they are
        // re-initialized by AppContext::Dispatch before any of these scripts run.
        mView.eval(
                "(function(){"
                        + "globalThis.createScene=undefined;"
                        + "globalThis.__babylonPlaygroundSceneFactoryReady=undefined;"
                        + "globalThis.__babylonPlaygroundWebGpuSmokeReady=undefined;"
                        + "globalThis.__webgpuSmokeDispose=undefined;"
                        + "})();",
                "app:///Scripts/playground_bootstrap_reset.js");

        // Always load the smoke script + runner when a view becomes ready.
        // The JS runner handles idempotent runtime disposal/restart and this
        // avoids gray frames when Android recreates the underlying view/runtime.
        mView.loadScript("app:///Scripts/webgpu_smoke.js");
        mView.loadScript("app:///Scripts/playground_runner.js");
    }
}
