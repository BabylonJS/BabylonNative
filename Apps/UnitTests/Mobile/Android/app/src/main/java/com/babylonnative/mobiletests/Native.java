package com.babylonnative.mobiletests;

import android.content.Context;
import android.view.Surface;

public final class Native {
    static {
        System.loadLibrary(BuildConfig.NATIVE_TEST_LIBRARY);
    }

    public static native int run(Context context, Surface surface, String directory);
}
