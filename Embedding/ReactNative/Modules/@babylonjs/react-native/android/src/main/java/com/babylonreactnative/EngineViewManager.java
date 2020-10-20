package com.babylonreactnative;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.facebook.react.bridge.ReadableArray;
import com.facebook.react.common.MapBuilder;
import com.facebook.react.uimanager.SimpleViewManager;
import com.facebook.react.uimanager.ThemedReactContext;

import java.util.Map;

public final class EngineViewManager extends SimpleViewManager<EngineView> {
    public static final int COMMAND_TAKE_SNAPSHOT = 0;
    public static final String COMMAND_TAKE_SNAPSHOT_NAME = "takeSnapshot";

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

    @Override
    public Map<String,Integer> getCommandsMap() {
        return MapBuilder.of(
            COMMAND_TAKE_SNAPSHOT_NAME,
            COMMAND_TAKE_SNAPSHOT
        );
    }

    @Override
    public void receiveCommand(final EngineView view, String commandId, ReadableArray args) {
        // This will be called whenever a command is sent from react-native.
        switch (commandId) {
            case COMMAND_TAKE_SNAPSHOT_NAME:
                view.takeSnapshot();
                break;
            default:
                throw new IllegalArgumentException(
                    String.format("Invalid command %s specified for EngineView.  Supported Commands: takeSnapshot", commandId));
        }
    }

    @Override
    public Map getExportedCustomDirectEventTypeConstants() {
        return MapBuilder.builder()
                .put(SnapshotDataReturnedEvent.EVENT_NAME,
                    MapBuilder.of("registrationName", SnapshotDataReturnedEvent.EVENT_NAME))
                .build();
    }
}