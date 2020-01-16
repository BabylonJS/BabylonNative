package com.android.testapp;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;

import BabylonNative.BabylonView;

public class AndroidViewAppActivity extends Activity implements BabylonView.ViewDelegate {
    BabylonView mView;

    // Activity life
    @Override protected void onCreate(Bundle icicle)
    {
        super.onCreate(icicle);
        mView = new BabylonView(getApplication(), this);
        setContentView(mView);
    }

    @Override protected void onPause()
    {
        mView.onPause();
        super.onPause();
    }

    @Override protected void onResume()
    {
        super.onResume();
        mView.onResume();
    }

    @Override public void onWindowFocusChanged(boolean hasFocus)
    {
        super.onWindowFocusChanged(hasFocus);
        if (hasFocus && mView.getVisibility() == View.GONE)
        {
            mView.setVisibility(View.VISIBLE);
        }
    }

    @Override public void onViewReady()
    {
        mView.loadScript("Scripts/experience.js");
        mView.eval("setTimeout(function(){" +
                "var red = new BABYLON.StandardMaterial(\"red\", scene);" +
                "red.diffuseColor = new BABYLON.Color3 (128,0,0);" +
                "cube.material = red;" +
                "}, 2000);" , "testEval");
    }
}
