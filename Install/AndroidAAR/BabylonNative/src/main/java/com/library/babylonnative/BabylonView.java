package com.library.babylonnative;

import android.app.Activity;
import android.content.Context;
import android.graphics.Canvas;
import android.util.Log;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.FrameLayout;

public class BabylonView extends FrameLayout implements SurfaceHolder.Callback2, View.OnTouchListener {
    private static final FrameLayout.LayoutParams childViewLayoutParams = new FrameLayout.LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT);
    private static final String TAG = "BabylonView";
    private boolean mViewReady = false;
    private final ViewDelegate mViewDelegate;
    private Activity mCurrentActivity;
    private final SurfaceView primarySurfaceView;
    private final SurfaceView xrSurfaceView;
    private final float pixelDensityScale = getResources().getDisplayMetrics().density;
    public BabylonView(Context context, ViewDelegate viewDelegate) {
        this(context, viewDelegate, (Activity)viewDelegate);
    }

    public BabylonView(Context context, ViewDelegate viewDelegate, Activity currentActivity) {
        super(context);

        this.primarySurfaceView = new SurfaceView(context);
        this.primarySurfaceView.setLayoutParams(BabylonView.childViewLayoutParams);
        this.primarySurfaceView.getHolder().addCallback(this);
        this.addView(this.primarySurfaceView);

        this.mCurrentActivity = currentActivity;
        SurfaceHolder holder = this.primarySurfaceView.getHolder();
        holder.addCallback(this);
        setOnTouchListener(this);
        this.mViewDelegate = viewDelegate;

        this.xrSurfaceView = new SurfaceView(context);
        this.xrSurfaceView.setLayoutParams(childViewLayoutParams);
        this.xrSurfaceView.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(SurfaceHolder holder) {
                // surfaceChanged is also called when the surface is created, so just do all the handling there
            }

            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
                Wrapper.xrSurfaceChanged(holder.getSurface());
            }

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {
                Wrapper.xrSurfaceChanged(null);
            }
        });
        this.xrSurfaceView.setVisibility(View.INVISIBLE);
        this.addView(this.xrSurfaceView);

        setWillNotDraw(false);

        Wrapper.initEngine();
    }

    public void setCurrentActivity(Activity currentActivity)
    {
        if (currentActivity != this.mCurrentActivity) {
            this.mCurrentActivity = currentActivity;
            Wrapper.setCurrentActivity(this.mCurrentActivity);
        }
    }

    public void loadScript(String path) {
        Wrapper.loadScript(path);
    }

    public void eval(String source, String sourceURL) {
        Wrapper.eval(source, sourceURL);
    }

    public void onPause() {
        setVisibility(View.GONE);
        Wrapper.activityOnPause();
    }

    public void onResume() {
        Wrapper.activityOnResume();
    }

    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] results) {
        Wrapper.activityOnRequestPermissionsResult(requestCode, permissions, results);
    }

    /**
     * This method is part of the SurfaceHolder.Callback interface, and is
     * not normally called or subclassed by clients of BabylonView.
     */
    public void surfaceCreated(SurfaceHolder holder) {
        Wrapper.surfaceCreated(holder.getSurface(), this.getContext());
        Wrapper.setCurrentActivity(this.mCurrentActivity);
        if (!this.mViewReady) {
            this.mViewDelegate.onViewReady();
            this.mViewReady = true;
        }
    }

    /**
     * This method is part of the SurfaceHolder.Callback interface, and is
     * not normally called or subclassed by clients of BabylonView.
     */
    public void surfaceDestroyed(SurfaceHolder holder) {
    }

    /**
     * This method is part of the SurfaceHolder.Callback interface, and is
     * not normally called or subclassed by clients of BabylonView.
     */
    public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {
        Wrapper.surfaceChanged((int)(w / this.pixelDensityScale), (int)(h / this.pixelDensityScale), holder.getSurface());
    }

    public interface ViewDelegate {
        void onViewReady();
    }

    @Override
    public boolean onTouch(View v, MotionEvent event) {
        int pointerId = event.getPointerId(event.getActionIndex());
        float mX = event.getX(event.getActionIndex()) / this.pixelDensityScale;
        float mY = event.getY(event.getActionIndex()) / this.pixelDensityScale;

        switch (event.getActionMasked()) {
            case MotionEvent.ACTION_DOWN:
            case MotionEvent.ACTION_POINTER_DOWN:
                Wrapper.setTouchInfo(pointerId, mX, mY, true, 1);
                break;
            case MotionEvent.ACTION_MOVE:
                Wrapper.setTouchInfo(pointerId, mX, mY, false, 0);
                break;
            case MotionEvent.ACTION_UP:
            case MotionEvent.ACTION_POINTER_UP:
                Wrapper.setTouchInfo(pointerId, mX, mY, true, 0);
                break;
        }
        return true;
    }

    @Override
    protected void finalize() throws Throwable {
        Wrapper.finishEngine();
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
        if (Wrapper.isXRActive()) {
            this.xrSurfaceView.setVisibility(View.VISIBLE);
        } else {
            this.xrSurfaceView.setVisibility(View.INVISIBLE);
        }

        Wrapper.renderFrame();
        invalidate();
    }
}
