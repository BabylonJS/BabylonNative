package com.babylonreactnative;

import android.annotation.TargetApi;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.os.Build;
import android.os.Handler;
import android.os.HandlerThread;
import android.util.Base64;
import android.view.MotionEvent;
import android.view.PixelCopy;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.FrameLayout;

import com.facebook.react.bridge.ReactContext;
import com.facebook.react.uimanager.UIManagerModule;
import com.facebook.react.uimanager.events.EventDispatcher;

import java.io.ByteArrayOutputStream;

public final class EngineView extends FrameLayout implements SurfaceHolder.Callback, View.OnTouchListener {
    private static final FrameLayout.LayoutParams childViewLayoutParams = new FrameLayout.LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT);
    private final SurfaceView primarySurfaceView;
    private final SurfaceView xrSurfaceView;
    private final EventDispatcher reactEventDispatcher;

    public EngineView(ReactContext reactContext) {
        super(reactContext);

        this.primarySurfaceView = new SurfaceView(reactContext);
        this.primarySurfaceView.setLayoutParams(EngineView.childViewLayoutParams);
        this.primarySurfaceView.getHolder().addCallback(this);
        this.addView(this.primarySurfaceView);

        this.xrSurfaceView = new SurfaceView(reactContext);
        this.xrSurfaceView.setLayoutParams(childViewLayoutParams);
        this.xrSurfaceView.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(SurfaceHolder holder) {
                // surfaceChanged is also called when the surface is created, so just do all the handling there
            }

            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
                BabylonNativeInterop.updateXRView(holder.getSurface());
            }

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {
                BabylonNativeInterop.updateXRView(null);
            }
        });
        this.xrSurfaceView.setVisibility(View.INVISIBLE);
        this.addView(this.xrSurfaceView);

        this.setOnTouchListener(this);

        this.setWillNotDraw(false);

        this.reactEventDispatcher = reactContext.getNativeModule(UIManagerModule.class).getEventDispatcher();
    }

    @Override
    public void surfaceCreated(SurfaceHolder surfaceHolder) {
        // surfaceChanged is also called when the surface is created, so just do all the handling there
    }

    @Override
    public void surfaceChanged(SurfaceHolder surfaceHolder, int i, int width, int height) {
        BabylonNativeInterop.updateView(surfaceHolder.getSurface());
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder surfaceHolder) {
    }

    @Override
    public boolean onTouch(View view, MotionEvent motionEvent) {
        BabylonNativeInterop.reportMotionEvent(motionEvent);
        return true;
    }

    @Override
    protected void onDraw(Canvas canvas) {
        if (BabylonNativeInterop.isXRActive()) {
            this.xrSurfaceView.setVisibility(View.VISIBLE);
        } else {
            this.xrSurfaceView.setVisibility(View.INVISIBLE);
        }

        BabylonNativeInterop.renderView();
        invalidate();
    }

    @TargetApi(24)
    public void takeSnapshot() {
        // Only supported on API level 24 and up, return a blank image.
        if (android.os.Build.VERSION.SDK_INT < Build.VERSION_CODES.N) {
            SnapshotDataReturnedEvent snapshotEvent = new SnapshotDataReturnedEvent(this.getId(), "");
            reactEventDispatcher.dispatchEvent(snapshotEvent);
        }

        // Create a bitmap that matches the width and height of the EngineView.
        final Bitmap bitmap = Bitmap.createBitmap(
                getWidth(),
                getHeight(),
                Bitmap.Config.ARGB_8888);

        // Offload the snapshot worker to a helper thread.
        final HandlerThread helperThread = new HandlerThread("ScreenCapture",-1);
        helperThread.start();
        final Handler helperThreadHandler = new Handler(helperThread.getLooper());

        SurfaceView surfaceView = this.primarySurfaceView;
        if (BabylonNativeInterop.isXRActive()) {
            surfaceView = this.xrSurfaceView;
        }

        // Request the pixel copy.
        PixelCopy.request(surfaceView, bitmap, (copyResult) ->  {
            // If the pixel copy was a success then convert the image to a base 64 encoded jpeg and fire the event.
            String encoded = "";
            if (copyResult == PixelCopy.SUCCESS) {
                ByteArrayOutputStream byteArrayStream = new ByteArrayOutputStream();
                bitmap.compress(Bitmap.CompressFormat.JPEG, 100, byteArrayStream);
                byte[] byteArray = byteArrayStream.toByteArray();
                bitmap.recycle();
                encoded = Base64.encodeToString(byteArray, Base64.DEFAULT);
            }

            SnapshotDataReturnedEvent snapshotEvent = new SnapshotDataReturnedEvent(this.getId(), encoded);
            reactEventDispatcher.dispatchEvent(snapshotEvent);
            helperThread.quitSafely();
        }, helperThreadHandler);
    }
}
