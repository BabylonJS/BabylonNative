package com.babylonreactnative;

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

    @ReactMethod
    public void initialize(Promise promise) {
        this.getReactApplicationContext().runOnJSQueueThread(() -> {
            BabylonNativeInterop.initialize(this.getReactApplicationContext());
            promise.resolve(null);
        });
    }

    @ReactMethod
    public void resetView(Promise promise) {
        this.getReactApplicationContext().runOnUiQueueThread(() -> {
            BabylonNativeInterop.resetView();
            promise.resolve(null);
        });
    }
}
