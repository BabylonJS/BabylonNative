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
 * Playground View built on top of {@link BabylonNative}. Owns one
 * Runtime + View handle pair for its lifetime.
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

    /**
     * Native helper implemented in
     * {@code Apps/Playground/Android/BabylonNative/src/main/cpp/PlaygroundJNI.cpp}.
     * Compiled into the same {@code libBabylonNativeIntegrations.so}
     * loaded by {@link BabylonNative}; queues the Babylon.js bootstrap
     * scripts (the list lives in {@code Apps/Playground/Shared/PlaygroundScripts.cpp}
     * so all Playground hosts share it).
     */
    private static native void loadBootstrapScripts(long runtimeHandle);

    private boolean mViewReady = false;
    private final ViewDelegate mViewDelegate;
    private final SurfaceView primarySurfaceView;
    private final SurfaceView xrSurfaceView;
    private final float pixelDensityScale = getResources().getDisplayMetrics().density;

    /** Native Runtime handle (0 if not created). Owned by this view. */
    private long mRuntimeHandle = 0;

    /** Native View handle (0 if not attached). Owned by this view. */
    private long mViewHandle = 0;

    public BabylonView(Context context, ViewDelegate viewDelegate) {
        super(context);

        this.primarySurfaceView = new SurfaceView(context);
        this.primarySurfaceView.setLayoutParams(BabylonView.childViewLayoutParams);
        this.primarySurfaceView.getHolder().addCallback(this);
        this.addView(this.primarySurfaceView);

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
                if (mRuntimeHandle != 0) {
                    BabylonNative.runtimeSetXrSurface(mRuntimeHandle, holder.getSurface());
                }
            }

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {
                if (mRuntimeHandle != 0) {
                    BabylonNative.runtimeSetXrSurface(mRuntimeHandle, null);
                }
            }
        });
        this.xrSurfaceView.setVisibility(View.INVISIBLE);
        this.addView(this.xrSurfaceView);

        setWillNotDraw(false);

        // Create the Runtime up-front and queue the Babylon.js bootstrap
        // scripts. They will run after the first viewAttach completes
        // engine initialization on the JS thread.
        mRuntimeHandle = BabylonNative.runtimeCreate(/*enableDebugger*/ true);
        loadBootstrapScripts(mRuntimeHandle);
    }

    public void loadScript(String path) {
        if (mRuntimeHandle != 0) {
            BabylonNative.runtimeLoadScript(mRuntimeHandle, path);
        }
    }

    public void eval(String source, String sourceURL) {
        if (mRuntimeHandle != 0) {
            BabylonNative.runtimeEval(mRuntimeHandle, source, sourceURL);
        }
    }

    /**
     * This method is part of the SurfaceHolder.Callback interface, and is
     * not normally called or subclassed by clients of BabylonView.
     */
    public void surfaceCreated(SurfaceHolder holder) {
        android.graphics.Rect frame = holder.getSurfaceFrame();
        mViewHandle = BabylonNative.viewAttach(mRuntimeHandle, holder.getSurface(),
                frame.width(), frame.height(), this.pixelDensityScale);

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
            BabylonNative.viewResize(mViewHandle, w, h, this.pixelDensityScale);
        }
    }

    public interface ViewDelegate {
        void onViewReady();
    }

    @Override
    public boolean onTouch(View v, MotionEvent event) {
        if (mViewHandle == 0) {
            return false;
        }

        int pointerId = event.getPointerId(event.getActionIndex());
        float mX = event.getX(event.getActionIndex()) / this.pixelDensityScale;
        float mY = event.getY(event.getActionIndex()) / this.pixelDensityScale;

        switch (event.getActionMasked()) {
            case MotionEvent.ACTION_DOWN:
            case MotionEvent.ACTION_POINTER_DOWN:
                BabylonNative.viewPointerDown(mViewHandle, pointerId, mX, mY);
                break;
            case MotionEvent.ACTION_MOVE:
                BabylonNative.viewPointerMove(mViewHandle, pointerId, mX, mY);
                break;
            case MotionEvent.ACTION_UP:
            case MotionEvent.ACTION_POINTER_UP:
                BabylonNative.viewPointerUp(mViewHandle, pointerId, mX, mY);
                break;
        }
        return true;
    }

    @Override
    protected void finalize() throws Throwable {
        try {
            if (mViewHandle != 0) {
                BabylonNative.viewDetach(mViewHandle);
                mViewHandle = 0;
            }
            if (mRuntimeHandle != 0) {
                BabylonNative.runtimeDestroy(mRuntimeHandle);
                mRuntimeHandle = 0;
            }
        } finally {
            super.finalize();
        }
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
        if (mRuntimeHandle != 0 && BabylonNative.runtimeIsXrActive(mRuntimeHandle)) {
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

