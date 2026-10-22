package com.babylonnative.mobiletests;

import android.app.Instrumentation;
import android.content.Context;
import android.content.Intent;
import android.content.res.AssetManager;
import androidx.test.ext.junit.runners.AndroidJUnit4;
import androidx.test.platform.app.InstrumentationRegistry;
import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import org.junit.Test;
import org.junit.runner.RunWith;
import static org.junit.Assert.assertEquals;

@RunWith(AndroidJUnit4.class)
public final class NativeTests {
    private void copyAssets(AssetManager assets, String path, File destination) throws Exception {
        String[] children = assets.list(path);
        if (children.length == 0) {
            destination.getParentFile().mkdirs();
            try (InputStream input = assets.open(path);
                 FileOutputStream output = new FileOutputStream(destination)) {
                byte[] buffer = new byte[8192];
                int count;
                while ((count = input.read(buffer)) != -1) {
                    output.write(buffer, 0, count);
                }
            }
        } else {
            for (String child : children) {
                copyAssets(assets, path.isEmpty() ? child : path + "/" + child,
                    new File(destination, child));
            }
        }
    }

    @Test
    public void runNativeSuite() throws Exception {
        Instrumentation instrumentation = InstrumentationRegistry.getInstrumentation();
        Context context = instrumentation.getTargetContext();
        // Native font APIs consume filesystem paths; UrlLib still loads app:/// assets from the APK.
        for (String asset : context.getAssets().list("")) {
            copyAssets(context.getAssets(), asset, new File(context.getFilesDir(), asset));
        }
        Intent intent = new Intent(context, TestActivity.class);
        intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        TestActivity activity = (TestActivity) instrumentation.startActivitySync(intent);
        try {
            assertEquals("Native GoogleTest suite failed; inspect NativeUnitTests logcat", 0,
                Native.run(context, activity.awaitSurface(), context.getFilesDir().getAbsolutePath()));
        } finally {
            instrumentation.runOnMainSync(activity::finish);
        }
    }
}
