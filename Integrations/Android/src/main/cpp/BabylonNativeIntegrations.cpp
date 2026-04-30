// JNI interop for Babylon::Integrations on Android.
//
// This file is the C++ side of the Android interop layer. It exposes
// `extern "C" JNIEXPORT` entry points that any JVM-language host (Kotlin
// or Java) can call by declaring matching `external fun` / `native`
// methods. The shape of those declarations follows directly from the
// JNI signatures here.
//
// We deliberately do not ship a Kotlin or Java class library on top —
// per the plan's non-goals (SimplifiedAPI.md §2), the interop layer's
// job ends at "Kotlin/Java can call into native Babylon code"; designing
// idiomatic high-level wrappers in the host language is left to the
// consumer.
//
// Convention: opaque C++ pointers cross the JNI boundary as `jlong`.
// `unique_ptr::release()` transfers ownership to the JVM side; the
// matching `*Destroy` function calls `delete` on the raw pointer.
//
// Java class name on the other side is `com.babylonjs.integrations.BabylonNative`
// (matching the `Java_com_babylonjs_integrations_BabylonNative_*` symbol
// names below). Hosts can choose any class name they like — the Java
// signatures must match the C++ symbols byte-for-byte.

#include <Babylon/Integrations/Runtime.h>
#include <Babylon/Integrations/View.h>

#include <AndroidExtensions/Globals.h>

#include <android/native_window.h>
#include <android/native_window_jni.h>

#include <jni.h>

#include <cstdint>
#include <string>
#include <vector>

namespace
{
    using Babylon::Integrations::Runtime;
    using Babylon::Integrations::View;
    using Babylon::Integrations::ViewDescriptor;

    Runtime* AsRuntime(jlong handle) { return reinterpret_cast<Runtime*>(handle); }
    View* AsView(jlong handle) { return reinterpret_cast<View*>(handle); }

    // Convert a jstring to std::string. Returns empty string if `jstr`
    // is null or UTF lookup fails.
    std::string ToStdString(JNIEnv* env, jstring jstr)
    {
        if (jstr == nullptr)
        {
            return {};
        }
        const char* utf = env->GetStringUTFChars(jstr, nullptr);
        if (utf == nullptr)
        {
            return {};
        }
        std::string result{utf};
        env->ReleaseStringUTFChars(jstr, utf);
        return result;
    }

    // Convert physical pixels (Android `View.onSizeChanged` units) to
    // logical pixels (the C++ ViewDescriptor convention) using the screen
    // density factor. See SimplifiedAPI.md §4.2 "Pixel units".
    ViewDescriptor MakeViewDescriptor(ANativeWindow* window, jint physicalW, jint physicalH, jfloat density)
    {
        ViewDescriptor descriptor{};
        descriptor.nativeWindow = static_cast<void*>(window);
        descriptor.width = static_cast<uint32_t>(static_cast<float>(physicalW) / density);
        descriptor.height = static_cast<uint32_t>(static_cast<float>(physicalH) / density);
        descriptor.devicePixelRatio = density;
        return descriptor;
    }
}

extern "C"
{

// =====================================================================
// Android-specific platform lifecycle (platform interop layer surface).
//
// These don't belong on the cross-platform Runtime/View API — they exist
// because Babylon Native plugins like NativeCamera require the host to
// register the JavaVM + current Activity via AndroidExtensions::Globals.
// See SimplifiedAPI.md §4.2 "Interop layer responsibilities".
// =====================================================================

// Call once at app startup, typically from `Application.onCreate`,
// before constructing any BabylonNativeRuntime.
JNIEXPORT void JNICALL
Java_com_babylonjs_integrations_BabylonNative_androidGlobalInitialize(
    JNIEnv* env, jclass, jobject context)
{
    JavaVM* javaVM{nullptr};
    if (env->GetJavaVM(&javaVM) != JNI_OK)
    {
        return;
    }
    android::global::Initialize(javaVM, context);
}

JNIEXPORT void JNICALL
Java_com_babylonjs_integrations_BabylonNative_androidGlobalSetCurrentActivity(
    JNIEnv*, jclass, jobject activity)
{
    android::global::SetCurrentActivity(activity);
}

JNIEXPORT void JNICALL
Java_com_babylonjs_integrations_BabylonNative_androidGlobalPause(JNIEnv*, jclass)
{
    android::global::Pause();
}

JNIEXPORT void JNICALL
Java_com_babylonjs_integrations_BabylonNative_androidGlobalResume(JNIEnv*, jclass)
{
    android::global::Resume();
}

JNIEXPORT void JNICALL
Java_com_babylonjs_integrations_BabylonNative_androidGlobalRequestPermissionsResult(
    JNIEnv* env, jclass, jint requestCode, jobjectArray permissions, jintArray grantResults)
{
    std::vector<std::string> nativePermissions{};
    const jsize permissionCount = env->GetArrayLength(permissions);
    nativePermissions.reserve(static_cast<size_t>(permissionCount));
    for (jsize i = 0; i < permissionCount; ++i)
    {
        auto perm = static_cast<jstring>(env->GetObjectArrayElement(permissions, i));
        nativePermissions.push_back(ToStdString(env, perm));
        env->DeleteLocalRef(perm);
    }

    std::vector<int32_t> nativeGrantResults{};
    const jsize grantCount = env->GetArrayLength(grantResults);
    if (grantCount > 0)
    {
        jint* grantElements = env->GetIntArrayElements(grantResults, nullptr);
        nativeGrantResults.assign(grantElements, grantElements + grantCount);
        env->ReleaseIntArrayElements(grantResults, grantElements, JNI_ABORT);
    }

    android::global::RequestPermissionsResult(
        static_cast<int32_t>(requestCode),
        nativePermissions,
        nativeGrantResults);
}

// =====================================================================
// Runtime
// =====================================================================

JNIEXPORT jlong JNICALL
Java_com_babylonjs_integrations_BabylonNative_runtimeCreate(JNIEnv*, jclass)
{
    // unique_ptr::release() returns the raw pointer and gives up
    // ownership *without* deleting; the JVM side now owns it via the
    // returned jlong handle and must call runtimeDestroy() to free it.
    return reinterpret_cast<jlong>(Runtime::Create().release());
}

JNIEXPORT void JNICALL
Java_com_babylonjs_integrations_BabylonNative_runtimeDestroy(JNIEnv*, jclass, jlong handle)
{
    delete AsRuntime(handle);
}

JNIEXPORT void JNICALL
Java_com_babylonjs_integrations_BabylonNative_runtimeLoadScript(
    JNIEnv* env, jclass, jlong handle, jstring url)
{
    AsRuntime(handle)->LoadScript(ToStdString(env, url));
}

JNIEXPORT void JNICALL
Java_com_babylonjs_integrations_BabylonNative_runtimeEval(
    JNIEnv* env, jclass, jlong handle, jstring source, jstring sourceUrl)
{
    AsRuntime(handle)->Eval(ToStdString(env, source), ToStdString(env, sourceUrl));
}

JNIEXPORT void JNICALL
Java_com_babylonjs_integrations_BabylonNative_runtimeSuspend(JNIEnv*, jclass, jlong handle)
{
    AsRuntime(handle)->Suspend();
}

JNIEXPORT void JNICALL
Java_com_babylonjs_integrations_BabylonNative_runtimeResume(JNIEnv*, jclass, jlong handle)
{
    AsRuntime(handle)->Resume();
}

JNIEXPORT jboolean JNICALL
Java_com_babylonjs_integrations_BabylonNative_runtimeIsSuspended(JNIEnv*, jclass, jlong handle)
{
    return AsRuntime(handle)->IsSuspended() ? JNI_TRUE : JNI_FALSE;
}

// =====================================================================
// View
// =====================================================================

JNIEXPORT jlong JNICALL
Java_com_babylonjs_integrations_BabylonNative_viewAttach(
    JNIEnv* env, jclass, jlong runtimeHandle, jobject surface,
    jint physicalW, jint physicalH, jfloat density)
{
    if (surface == nullptr)
    {
        return 0;
    }
    ANativeWindow* window = ANativeWindow_fromSurface(env, surface);
    if (window == nullptr)
    {
        return 0;
    }
    auto view = View::Attach(*AsRuntime(runtimeHandle),
                              MakeViewDescriptor(window, physicalW, physicalH, density));
    if (!view)
    {
        ANativeWindow_release(window);
        return 0;
    }
    // The View's Device::UpdateWindow has acquired its own reference
    // on the ANativeWindow internally (bgfx retains it for the surface
    // binding lifetime). We can release our local acquire here.
    ANativeWindow_release(window);
    return reinterpret_cast<jlong>(view.release());
}

JNIEXPORT void JNICALL
Java_com_babylonjs_integrations_BabylonNative_viewDetach(JNIEnv*, jclass, jlong handle)
{
    delete AsView(handle);
}

JNIEXPORT void JNICALL
Java_com_babylonjs_integrations_BabylonNative_viewRenderFrame(JNIEnv*, jclass, jlong handle)
{
    AsView(handle)->RenderFrame();
}

JNIEXPORT void JNICALL
Java_com_babylonjs_integrations_BabylonNative_viewResize(
    JNIEnv*, jclass, jlong handle, jint physicalW, jint physicalH, jfloat density)
{
    AsView(handle)->Resize(
        static_cast<uint32_t>(static_cast<float>(physicalW) / density),
        static_cast<uint32_t>(static_cast<float>(physicalH) / density),
        density);
}

#if BABYLON_NATIVE_PLUGIN_NATIVEINPUT

JNIEXPORT void JNICALL
Java_com_babylonjs_integrations_BabylonNative_viewPointerDown(
    JNIEnv*, jclass, jlong handle, jint pointerId, jfloat x, jfloat y)
{
    AsView(handle)->OnPointerDown(static_cast<int32_t>(pointerId), x, y);
}

JNIEXPORT void JNICALL
Java_com_babylonjs_integrations_BabylonNative_viewPointerMove(
    JNIEnv*, jclass, jlong handle, jint pointerId, jfloat x, jfloat y)
{
    AsView(handle)->OnPointerMove(static_cast<int32_t>(pointerId), x, y);
}

JNIEXPORT void JNICALL
Java_com_babylonjs_integrations_BabylonNative_viewPointerUp(
    JNIEnv*, jclass, jlong handle, jint pointerId, jfloat x, jfloat y)
{
    AsView(handle)->OnPointerUp(static_cast<int32_t>(pointerId), x, y);
}

#endif

} // extern "C"
