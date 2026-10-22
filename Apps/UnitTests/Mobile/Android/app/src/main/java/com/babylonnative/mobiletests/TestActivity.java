package com.babylonnative.mobiletests;

import android.app.Activity;
import android.os.Bundle;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;

public final class TestActivity extends Activity implements SurfaceHolder.Callback {
    private final CountDownLatch ready = new CountDownLatch(1);
    private volatile Surface surface;

    @Override
    public void onCreate(Bundle state) {
        super.onCreate(state);
        getWindow().addFlags(android.view.WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        SurfaceView view = new SurfaceView(this);
        view.getHolder().addCallback(this);
        setContentView(view);
    }

    public Surface awaitSurface() throws InterruptedException {
        if (!ready.await(30, TimeUnit.SECONDS) || surface == null || !surface.isValid()) {
            throw new IllegalStateException("Test rendering surface did not become ready");
        }
        return surface;
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        surface = holder.getSurface();
        ready.countDown();
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {}

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        surface = null;
    }
}
