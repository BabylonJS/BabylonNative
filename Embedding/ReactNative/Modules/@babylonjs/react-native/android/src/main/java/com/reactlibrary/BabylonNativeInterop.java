package com.reactlibrary;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.view.MotionEvent;
import android.view.Surface;

import com.facebook.react.bridge.ActivityEventListener;
import com.facebook.react.bridge.JavaScriptContextHolder;
import com.facebook.react.bridge.LifecycleEventListener;
import com.facebook.react.bridge.ReactContext;

import java.util.Hashtable;
import java.util.Iterator;
import java.util.Map;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.CompletionStage;

final class BabylonNativeInterop {
    // JNI interface
    static {
        System.loadLibrary("BabylonNative");
    }

    private static boolean isInitialized;
    private static final Hashtable<JavaScriptContextHolder, CompletableFuture<Long>> nativeInstances = new Hashtable<>();

    private static native void initialize(Context context);
    private static native void setCurrentActivity(Activity activity);
    private static native void pause();
    private static native void resume();
    private static native long create(long jsiRuntimeRef, Surface surface);
    private static native void refresh(long instanceRef, Surface surface);
    private static native void setPointerButtonState(long instanceRef, int pointerId, int buttonId, boolean isDown, int x, int y);
    private static native void setPointerPosition(long instanceRef, int pointerId, int x, int y);
    private static native void destroy(long instanceRef);

    // Must be called from the Android UI thread
    static void setView(ReactContext reactContext, Surface surface) {
        // This is global initialization that only needs to happen once
        if (!BabylonNativeInterop.isInitialized) {
            BabylonNativeInterop.initialize(reactContext);
            BabylonNativeInterop.isInitialized = true;
        }

        BabylonNativeInterop.destroyOldNativeInstances(reactContext);

        CompletableFuture<Long> instanceRefFuture = BabylonNativeInterop.getOrCreateFuture(reactContext);

        reactContext.runOnJSQueueThread(() -> {
            Long instanceRef = instanceRefFuture.getNow(null);
            if (instanceRef == null)
            {
                long jsiRuntimeRef = reactContext.getJavaScriptContextHolder().get();
                if (jsiRuntimeRef == 0) {
                    instanceRefFuture.complete(0L);
                } else {
                    instanceRef = BabylonNativeInterop.create(jsiRuntimeRef, surface);
                    final long finalInstanceRef = instanceRef;

                    reactContext.addLifecycleEventListener(new LifecycleEventListener() {
                        @Override
                        public void onHostResume() {
                            BabylonNativeInterop.setCurrentActivity(reactContext.getCurrentActivity());
                            BabylonNativeInterop.resume();
                        }

                        @Override
                        public void onHostPause() {
                            BabylonNativeInterop.pause();
                        }

                        @Override
                        public void onHostDestroy() {
                            BabylonNativeInterop.deinitialize();
                        }
                    });

                    reactContext.addActivityEventListener(new ActivityEventListener() {
                        @Override
                        public void onActivityResult(Activity activity, int requestCode, int resultCode, Intent data) {
                            // Nothing to do here
                        }

                        @Override
                        public void onNewIntent(Intent intent) {
                            BabylonNativeInterop.setCurrentActivity(reactContext.getCurrentActivity());
                        }
                    });

                    instanceRefFuture.complete(finalInstanceRef);
                }
            } else if (instanceRef != 0) {
                BabylonNativeInterop.refresh(instanceRef, surface);
            }
        });
    }

    static void reportMotionEvent(ReactContext reactContext, MotionEvent motionEvent) {
        CompletableFuture<Long> instanceRefFuture = BabylonNativeInterop.nativeInstances.get(reactContext.getJavaScriptContextHolder());
        if (instanceRefFuture != null) {
            Long instanceRef = instanceRefFuture.getNow(null);
            if (instanceRef != null) {
                int maskedAction = motionEvent.getActionMasked();
                boolean isPointerDown = maskedAction == MotionEvent.ACTION_DOWN || maskedAction == MotionEvent.ACTION_POINTER_DOWN;
                boolean isPointerUp = maskedAction == MotionEvent.ACTION_UP || maskedAction == MotionEvent.ACTION_POINTER_UP;
                boolean isPointerMove = maskedAction == MotionEvent.ACTION_MOVE;

                if (isPointerDown || isPointerUp) {
                    int pointerIndex = motionEvent.getActionIndex();
                    int pointerId = motionEvent.getPointerId(pointerIndex);
                    int buttonId = motionEvent.getActionButton();
                    int x = (int)motionEvent.getX(pointerIndex);
                    int y = (int)motionEvent.getY(pointerIndex);
                    BabylonNativeInterop.setPointerButtonState(instanceRef, pointerId, buttonId, isPointerDown, x, y);
                } else if (isPointerMove) {
                    for (int pointerIndex = 0; pointerIndex < motionEvent.getPointerCount(); pointerIndex++) {
                        int pointerId = motionEvent.getPointerId(pointerIndex);
                        int x = (int)motionEvent.getX(pointerIndex);
                        int y = (int)motionEvent.getY(pointerIndex);
                        BabylonNativeInterop.setPointerPosition(instanceRef, pointerId, x, y);
                    }
                }
            }
        }
    }

    // Must be called from the Android UI thread
    static CompletionStage<Long> whenInitialized(ReactContext reactContext) {
        return BabylonNativeInterop.getOrCreateFuture(reactContext);
    }

    // Must be called from the Android UI thread
    static void deinitialize() {
        BabylonNativeInterop.destroyOldNativeInstances(null);
    }

    private static CompletableFuture<Long> getOrCreateFuture(ReactContext reactContext) {
        JavaScriptContextHolder jsContext = reactContext.getJavaScriptContextHolder();
        CompletableFuture<Long> instanceRefFuture = BabylonNativeInterop.nativeInstances.get(jsContext);
        if (instanceRefFuture == null)
        {
            instanceRefFuture = new CompletableFuture<>();
            BabylonNativeInterop.nativeInstances.put(jsContext, instanceRefFuture);
        }
        return instanceRefFuture;
    }

    private static void destroyOldNativeInstances(ReactContext currentReactContext) {
        Iterator<Map.Entry<JavaScriptContextHolder, CompletableFuture<Long>>> nativeInstanceIterator = BabylonNativeInterop.nativeInstances.entrySet().iterator();
        while (nativeInstanceIterator.hasNext()) {
            Map.Entry<JavaScriptContextHolder, CompletableFuture<Long>> nativeInstanceInfo = nativeInstanceIterator.next();
            if (currentReactContext == null || nativeInstanceInfo.getKey() != currentReactContext.getJavaScriptContextHolder()) {
                Long oldInstanceRef = nativeInstanceInfo.getValue().getNow(null);
                if (oldInstanceRef != null && oldInstanceRef != 0) {
                    BabylonNativeInterop.destroy(oldInstanceRef);
                }
                nativeInstanceIterator.remove();
            }
        }
    }
}
