package BabylonNative;

import android.content.res.AssetManager;
import android.view.Surface;

import java.nio.ByteBuffer;

public class BabylonNativeWrapper {
    // JNI interface
    static {
        System.loadLibrary("BabylonNativeJNI");
    }

    public static native void initEngine(AssetManager assetManager, String path);
    public static native void finishEngine();
    public static native void surfaceCreated(Surface surface);
    public static native void surfaceChanged(int width, int height, Surface surface);
    public static native void activityOnPause();
    public static native void activityOnResume();
    public static native void openFile(String path);
    public static native void openStream(ByteBuffer bytes);
    public static native void setTouchInfo(float dx, float dy, boolean down);
    public static native void setPinchInfo(float zoom);
    public static native void step();
}