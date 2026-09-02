package com.jsruntimehost.unittests;

import android.content.Context;

public class Native {
    // JNI interface
    static {
        System.loadLibrary("UnitTestsJNI");
    }

    public static native int javaScriptTests(Context context);
}
