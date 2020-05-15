package com.reactlibrary;

import android.os.Handler;
import android.os.Looper;

import androidx.annotation.NonNull;

import com.facebook.react.bridge.Promise;
import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactContextBaseJavaModule;
import com.facebook.react.bridge.ReactMethod;

public final class BabylonModule extends ReactContextBaseJavaModule {

    BabylonModule(ReactApplicationContext reactContext) {
        super(reactContext);
    }

    @NonNull
    @Override
    public String getName() {
        return "BabylonModule";
    }

    @Override
    public void onCatalystInstanceDestroy() {
        new Handler(Looper.getMainLooper()).post(BabylonNativeInterop::deinitialize);
    }

    @ReactMethod
    public void initialize(Promise promise) {
        // Ideally we'd do all the initialization here that is scoped to a javascript engine instance, but this is tied up in the view initialization in Babylon Native currently.
        // For now, just await initialization by the first EngineView that is created.
        BabylonNativeInterop.whenInitialized(this.getReactApplicationContext()).thenAccept(instanceRef -> promise.resolve(instanceRef != 0));
    }

    @ReactMethod
    public void whenInitialized(Promise promise) {
        BabylonNativeInterop.whenInitialized(this.getReactApplicationContext()).thenAccept(instanceRef -> promise.resolve(instanceRef != 0));
    }
}
