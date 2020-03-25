package com.android.babylonnative.playground;

import android.app.Activity;
import android.os.Bundle;
import android.os.StrictMode;
import android.view.View;

import BabylonNative.BabylonView;

public class PlaygroundActivity extends Activity implements BabylonView.ViewDelegate {
    BabylonView mView;

    // Activity life
    @Override
    protected void onCreate(Bundle icicle) {
        super.onCreate(icicle);

        // Grant thread all allowed permissions
        StrictMode.ThreadPolicy policy = new StrictMode.ThreadPolicy.Builder().permitAll().build();
        StrictMode.setThreadPolicy(policy);

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
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        if (hasFocus && mView.getVisibility() == View.GONE) {
            mView.setVisibility(View.VISIBLE);
        }
    }

    @Override
    public void onViewReady() {
        mView.loadScript("Scripts/experience.js");
    }
}
