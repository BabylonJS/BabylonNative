package com.library.babylonnative;

import android.content.Context;
import android.graphics.Canvas;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.FrameLayout;

import com.babylonjs.integrations.BabylonNative;

/**
 * Playground View built on top of {@link BabylonNative}. Borrows a
 * Runtime handle from the host (the host is responsible for the
 * Runtime's lifetime via {@link BabylonNative#runtimeCreate(boolean)} /
 * {@link BabylonNative#runtimeDestroy(long)}); this class only owns the
 * View handle, which mirrors the underlying Surface lifecycle:
 * attach in {@code surfaceCreated}, resize in {@code surfaceChanged},
 * detach in {@code surfaceDestroyed}.
 *
 * <p>All sizes and coordinates passed to the native layer are in
 * physical pixels (Android's natural unit) — the Device queries the
 * screen device-pixel-ratio internally and applies any conversions
 * needed at the rendering layer.
 *
 * <p>Activity lifecycle: the host Activity is responsible for the
 * process-wide {@code androidGlobalInitialize}, {@code SetCurrentActivity},
 * {@code Pause}/{@code Resume}, and {@code RequestPermissionsResult}
 * notifications (see {@code PlaygroundActivity.java}). The Runtime
 * automatically subscribes to {@code androidGlobalPause / Resume} when
 * created, so the host Activity does not need to invoke any per-view
 * pause/resume method — telling the JNI layer once is enough for every
 * Runtime in the process.
 */
public class BabylonView extends FrameLayout implements SurfaceHolder.Callback2, View.OnTouchListener {
    private static final FrameLayout.LayoutParams childViewLayoutParams =
            new FrameLayout.LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT);

    private final SurfaceView primarySurfaceView;
    private final SurfaceView xrSurfaceView;

    /** Runtime handle borrowed from the host. Not owned by this view. */
    private final long mRuntimeHandle;

    /** Native View handle (0 if not attached). Owned by this view. */
    private long mViewHandle = 0;

    public BabylonView(Context context, long runtimeHandle) {
        super(context);
        mRuntimeHandle = runtimeHandle;

        this.primarySurfaceView = new SurfaceView(context);
        this.primarySurfaceView.setLayoutParams(BabylonView.childViewLayoutParams);
        this.primarySurfaceView.getHolder().addCallback(this);
        this.addView(this.primarySurfaceView);

        setOnTouchListener(this);

        this.xrSurfaceView = new SurfaceView(context);
        this.xrSurfaceView.setLayoutParams(childViewLayoutParams);
        this.xrSurfaceView.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(SurfaceHolder holder) {
                // surfaceChanged is also called when the surface is created, so just do all the handling there
            }

            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
                BabylonNative.runtimeSetXrSurface(mRuntimeHandle, holder.getSurface());
            }

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {
                BabylonNative.runtimeSetXrSurface(mRuntimeHandle, null);
            }
        });
        this.xrSurfaceView.setVisibility(View.INVISIBLE);
        this.addView(this.xrSurfaceView);

        setWillNotDraw(false);
    }

    /**
     * This method is part of the SurfaceHolder.Callback interface, and is
     * not normally called or subclassed by clients of BabylonView.
     */
    public void surfaceCreated(SurfaceHolder holder) {
        mViewHandle = BabylonNative.viewAttach(mRuntimeHandle, holder.getSurface());
    }

    /**
     * This method is part of the SurfaceHolder.Callback interface, and is
     * not normally called or subclassed by clients of BabylonView.
     */
    public void surfaceDestroyed(SurfaceHolder holder) {
        if (mViewHandle != 0) {
            BabylonNative.viewDetach(mViewHandle);
            mViewHandle = 0;
        }
    }

    /**
     * This method is part of the SurfaceHolder.Callback interface, and is
     * not normally called or subclassed by clients of BabylonView.
     */
    public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {
        if (mViewHandle != 0) {
            BabylonNative.viewResize(mViewHandle, w, h);
        }
    }

    @Override
    public boolean onTouch(View v, MotionEvent event) {
        if (mViewHandle == 0) {
            return false;
        }

        int pointerId = event.getPointerId(event.getActionIndex());
        float x = event.getX(event.getActionIndex());
        float y = event.getY(event.getActionIndex());

        switch (event.getActionMasked()) {
            case MotionEvent.ACTION_DOWN:
            case MotionEvent.ACTION_POINTER_DOWN:
                BabylonNative.viewPointerDown(mViewHandle, pointerId, x, y);
                break;
            case MotionEvent.ACTION_MOVE:
                BabylonNative.viewPointerMove(mViewHandle, pointerId, x, y);
                break;
            case MotionEvent.ACTION_UP:
            case MotionEvent.ACTION_POINTER_UP:
                BabylonNative.viewPointerUp(mViewHandle, pointerId, x, y);
                break;
        }
        return true;
    }

    /**
     * This method is part of the SurfaceHolder.Callback2 interface, and is
     * not normally called or subclassed by clients of BabylonView.
     */
    @Deprecated
    @Override
    public void surfaceRedrawNeeded(SurfaceHolder holder) {
        // Redraw happens in the bgfx thread. No need to handle it here.
    }

    @Override
    protected void onDraw(Canvas canvas) {
        if (BabylonNative.runtimeIsXrActive(mRuntimeHandle)) {
            this.xrSurfaceView.setVisibility(View.VISIBLE);
        } else {
            this.xrSurfaceView.setVisibility(View.INVISIBLE);
        }

        if (mViewHandle != 0) {
            BabylonNative.viewRenderFrame(mViewHandle);
        }
        invalidate();
    }
}
