package com.babylonreactnative;

import com.facebook.react.bridge.Arguments;
import com.facebook.react.bridge.WritableMap;
import com.facebook.react.uimanager.events.Event;
import com.facebook.react.uimanager.events.RCTEventEmitter;

public class SnapshotDataReturnedEvent extends Event<SnapshotDataReturnedEvent> {
    public static final String EVENT_NAME = "onSnapshotDataReturned";
    public static final String DATA_NAME = "data";
    private final WritableMap payload;

    public SnapshotDataReturnedEvent(int viewId, String imageData) {
        super(viewId);
        this.payload = Arguments.createMap();
        this.payload.putString(DATA_NAME, imageData);
    }

    @Override
    public String getEventName() {
        return EVENT_NAME;
    }

    @Override
    public void dispatch(RCTEventEmitter rctEventEmitter) {
        rctEventEmitter.receiveEvent(getViewTag(), getEventName(), this.payload);
    }
}