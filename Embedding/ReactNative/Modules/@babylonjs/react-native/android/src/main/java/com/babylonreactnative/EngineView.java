package com.babylonreactnative;

import android.annotation.TargetApi;
import android.graphics.Bitmap;
import android.os.Build;
import android.os.Handler;
import android.os.HandlerThread;
import android.util.Base64;
import android.view.MotionEvent;
import android.view.PixelCopy;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;

import com.facebook.react.bridge.ReactContext;
import com.facebook.react.uimanager.UIManagerModule;
import com.facebook.react.uimanager.events.EventDispatcher;

import java.io.ByteArrayOutputStream;

public final class EngineView extends SurfaceView implements SurfaceHolder.Callback, View.OnTouchListener {
    private final ReactContext reactContext;
    private final EventDispatcher reactEventDispatcher;

    public EngineView(ReactContext reactContext) {
        super(reactContext);
        this.reactContext = reactContext;
        this.getHolder().addCallback(this);
        this.setOnTouchListener(this);
        this.reactEventDispatcher = this.reactContext.getNativeModule(UIManagerModule.class).getEventDispatcher();
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

        // Request the pixel copy.
        PixelCopy.request(this, bitmap, (copyResult) ->  {
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
