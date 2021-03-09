package com.babylonreactnative;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.view.MotionEvent;
import android.view.Surface;

import com.facebook.react.bridge.ActivityEventListener;
import com.facebook.react.bridge.LifecycleEventListener;
import com.facebook.react.bridge.ReactContext;
import com.facebook.react.turbomodule.core.interfaces.CallInvokerHolder;

public final class BabylonNativeInterop {

    private static class BabylonNative {
        static {
            System.loadLibrary("BabylonNative");
        }

        public static native void initialize(Context context, long jsiRuntimeRef, CallInvokerHolder jsCallInvokerHolder);
        public static native void setCurrentActivity(Activity activity);
        public static native void pause();
        public static native void resume();
        public static native void updateView(Surface surface);
        public static native void renderView();
        public static native void resetView();
        public static native void setTouchButtonState(int pointerId, boolean isDown, int x, int y);
        public static native void setTouchPosition(int pointerId, int x, int y);
    }

    private static LifecycleEventListener lifeCycleEventListener;
    private static ActivityEventListener activityEventListener;

    public static void initialize(ReactContext reactContext) {
        long jsiRuntimeRef = reactContext.getJavaScriptContextHolder().get();
        CallInvokerHolder jsCallInvokerHolder = reactContext.getCatalystInstance().getJSCallInvokerHolder();
        BabylonNative.initialize(reactContext, jsiRuntimeRef, jsCallInvokerHolder);

        if (BabylonNativeInterop.lifeCycleEventListener != null) {
            reactContext.removeLifecycleEventListener(lifeCycleEventListener);
        }

        BabylonNativeInterop.lifeCycleEventListener = new LifecycleEventListener() {
            @Override
            public void onHostResume() {
                BabylonNative.setCurrentActivity(reactContext.getCurrentActivity());
                BabylonNative.resume();
            }

            @Override
            public void onHostPause() {
                BabylonNative.pause();
            }

            @Override
            public void onHostDestroy() {
            }
        };

        reactContext.addLifecycleEventListener(lifeCycleEventListener);

        if (BabylonNativeInterop.activityEventListener != null) {
            reactContext.removeActivityEventListener(BabylonNativeInterop.activityEventListener);
        }

        BabylonNativeInterop.activityEventListener = new ActivityEventListener() {
            @Override
            public void onActivityResult(Activity activity, int requestCode, int resultCode, Intent data) {
                // Nothing to do here
            }

            @Override
            public void onNewIntent(Intent intent) {
                BabylonNative.setCurrentActivity(reactContext.getCurrentActivity());
            }
        };

        reactContext.addActivityEventListener(BabylonNativeInterop.activityEventListener);
    }

    public static void updateView(Surface surface) {
        BabylonNative.updateView(surface);
    }

    public static void renderView() {
        BabylonNative.renderView();
    }

    public static void resetView() {
        BabylonNative.resetView();
    }

    public static void reportMotionEvent(MotionEvent motionEvent) {
        int maskedAction = motionEvent.getActionMasked();
        boolean isPointerDown = maskedAction == MotionEvent.ACTION_DOWN || maskedAction == MotionEvent.ACTION_POINTER_DOWN;
        boolean isPointerUp = maskedAction == MotionEvent.ACTION_UP || maskedAction == MotionEvent.ACTION_POINTER_UP;
        boolean isPointerMove = maskedAction == MotionEvent.ACTION_MOVE;

        if (isPointerDown || isPointerUp) {
            int pointerIndex = motionEvent.getActionIndex();
            int pointerId = motionEvent.getPointerId(pointerIndex);
            int x = (int)motionEvent.getX(pointerIndex);
            int y = (int)motionEvent.getY(pointerIndex);
            BabylonNative.setTouchButtonState(pointerId, isPointerDown, x, y);
        } else if (isPointerMove) {
            for (int pointerIndex = 0; pointerIndex < motionEvent.getPointerCount(); pointerIndex++) {
                int pointerId = motionEvent.getPointerId(pointerIndex);
                int x = (int)motionEvent.getX(pointerIndex);
                int y = (int)motionEvent.getY(pointerIndex);
                BabylonNative.setTouchPosition(pointerId, x, y);
            }
        }
    }
}
