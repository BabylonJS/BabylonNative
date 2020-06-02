package com.reactlibrary;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.facebook.react.common.MapBuilder;
import com.facebook.react.uimanager.SimpleViewManager;
import com.facebook.react.uimanager.ThemedReactContext;

import java.util.Map;

public final class EngineViewManager extends SimpleViewManager<EngineView> {

    @NonNull
    @Override
    public String getName() {
        return "EngineView";
    }

    @NonNull
    @Override
    protected EngineView createViewInstance(@NonNull ThemedReactContext reactContext) {
        return new EngineView(reactContext);
    }

    @Override
    public void onDropViewInstance(@NonNull EngineView view) {
        super.onDropViewInstance(view);
        // TODO: Native view specific cleanup
    }
}