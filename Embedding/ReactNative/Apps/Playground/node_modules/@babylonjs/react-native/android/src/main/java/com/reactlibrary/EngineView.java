package com.reactlibrary;

import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;

import com.facebook.react.bridge.ReactContext;

public final class EngineView extends SurfaceView implements SurfaceHolder.Callback, View.OnTouchListener {
    private final ReactContext reactContext;

    public EngineView(ReactContext reactContext) {
        super(reactContext);
        this.reactContext = reactContext;
        this.getHolder().addCallback(this);
        this.setOnTouchListener(this);
    }

    @Override
    public void surfaceCreated(SurfaceHolder surfaceHolder) {
        // surfaceChanged is also called when the surface is created, so just do all the handling there
    }

    @Override
    public void surfaceChanged(SurfaceHolder surfaceHolder, int i, int width, int height) {
        BabylonNativeInterop.setView(this.reactContext, surfaceHolder.getSurface());
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder surfaceHolder) {

    }

    @Override
    public boolean onTouch(View view, MotionEvent motionEvent) {
        BabylonNativeInterop.reportMotionEvent(this.reactContext, motionEvent);
        return true;
    }
}
