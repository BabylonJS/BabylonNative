package BabylonNative;

import android.content.Context;
import android.util.Log;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;

import java.lang.ref.WeakReference;
import java.util.ArrayList;

public class BabylonView extends SurfaceView implements SurfaceHolder.Callback2, View.OnTouchListener {
    private static final String TAG = "BabylonView";
    private boolean mViewReady = false;
    private ViewDelegate mViewDelegate;
    private Context mContext;

    public BabylonView(SurfaceView xrView, Context context, ViewDelegate viewDelegate) {
        super(context);

        mContext = context;

        SurfaceHolder holder = getHolder();
        holder.addCallback(this);
        setOnTouchListener(this);
        this.mViewDelegate = viewDelegate;


        SurfaceHolder xrHolder = xrView.getHolder();
        xrHolder.addCallback(this);

        BabylonNative.Wrapper.initEngine(context.getResources().getAssets());
    }

    public void loadScript(String path) {
        BabylonNative.Wrapper.loadScript(path);
    }

    public void eval(String source, String sourceURL) {
        BabylonNative.Wrapper.eval(source, sourceURL);
    }

    public void onPause() {
        setVisibility(View.GONE);
        BabylonNative.Wrapper.activityOnPause();
    }

    public void onResume() {
        BabylonNative.Wrapper.activityOnResume();
    }

    /**
     * This method is part of the SurfaceHolder.Callback interface, and is
     * not normally called or subclassed by clients of BabylonView.
     */
    public void surfaceCreated(SurfaceHolder holder) {
        if (holder == this.getHolder()) {
            BabylonNative.Wrapper.surfaceCreated(getHolder().getSurface(), mContext);
            if (!this.mViewReady) {
                mViewDelegate.onViewReady();
                mViewReady = true;
            }
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
        if (holder == this.getHolder()) {
            BabylonNative.Wrapper.surfaceChanged(w, h, getHolder().getSurface());
        } else {
            BabylonNative.Wrapper.xrSurfaceChanged(w, h, getHolder().getSurface());
        }
    }

    public interface ViewDelegate {
        void onViewReady();
    }

    @Override
    public boolean onTouch(View v, MotionEvent event)
    {
        float mX = event.getX();
        float mY = event.getY();
        switch (event.getAction()) {
            case MotionEvent.ACTION_DOWN:
                BabylonNative.Wrapper.setTouchInfo(mX, mY, true);
                break;
            case MotionEvent.ACTION_MOVE:
                BabylonNative.Wrapper.setTouchInfo(mX, mY, true);
                break;
            case MotionEvent.ACTION_UP:
                BabylonNative.Wrapper.setTouchInfo(mX, mY, false);
                break;
        }
        return true;
    }

    @Override
    protected void finalize() throws Throwable {
        BabylonNative.Wrapper.finishEngine();
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
}
