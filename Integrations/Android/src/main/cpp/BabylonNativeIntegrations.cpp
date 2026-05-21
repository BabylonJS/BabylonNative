// JNI interop for Babylon::Integrations on Android.
//
// This file is the C++ side of the Android interop layer. It exposes
// `extern "C" JNIEXPORT` entry points that any JVM-language host can call
// via matching `external fun` / `native` declarations.
//
// We deliberately ship no Kotlin/Java class library on top — interop ends
// at "Kotlin/Java can call into Babylon".
//
// Convention: opaque C++ pointers cross JNI as `jlong`.
// `unique_ptr::release()` transfers ownership to the JVM side; the
// matching `*Destroy` function `delete`s the raw pointer.
//
// Symbols here follow `Java_com_babylonjs_integrations_BabylonNative_*`.
// Hosts can use any class name as long as Java signatures match.

#include <Babylon/Integrations/Runtime.h>
#include <Babylon/Integrations/View.h>
#include <Babylon/Integrations/Android/RuntimeHandle.h>

#include <AndroidExtensions/Globals.h>

#include <android/log.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>

#include <jni.h>

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace
{
    using Babylon::Integrations::LogLevel;
    using Babylon::Integrations::Runtime;
    using Babylon::Integrations::RuntimeOptions;
    using Babylon::Integrations::View;

    // Wraps a Runtime plus two `android::global` event tickets that
    // auto-Suspend/Resume on Activity lifecycle. Tickets are declared
    // AFTER the Runtime so they're destroyed BEFORE it — guaranteeing no
    // callback fires on a dead Runtime during teardown.
    //
    // Runtime is held by unique_ptr so its address is stable for the
    // ticket lambdas (the tickets aren't move-assignable, so we have to
    // know the address before constructing the wrapper).
    //
    // Runtime::Suspend/Resume are refcounted, so this composes safely
    // with explicit host-side runtimeSuspend / runtimeResume calls.
    struct AndroidRuntime
    {
        std::unique_ptr<Runtime> runtime;
        android::global::AppStateChangedCallbackTicket pauseTicket;
        android::global::AppStateChangedCallbackTicket resumeTicket;
    };

    AndroidRuntime* AsAndroidRuntime(jlong handle) { return reinterpret_cast<AndroidRuntime*>(handle); }
    Runtime* AsRuntime(jlong handle) { return AsAndroidRuntime(handle)->runtime.get(); }
    View* AsView(jlong handle) { return reinterpret_cast<View*>(handle); }

    int LogPriorityFor(LogLevel level)
    {
        switch (level)
        {
            case LogLevel::Verbose: return ANDROID_LOG_VERBOSE;
            case LogLevel::Log:     return ANDROID_LOG_INFO;
            case LogLevel::Warn:    return ANDROID_LOG_WARN;
            case LogLevel::Error:   return ANDROID_LOG_ERROR;
            case LogLevel::Fatal:   return ANDROID_LOG_FATAL;
        }
        return ANDROID_LOG_INFO;
    }

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

    // Throws a Java IllegalStateException. Used when a plugin wasn't
    // compiled in, so the misconfiguration surfaces as a clean Java
    // exception rather than UnsatisfiedLinkError or silent no-op.
    // [[maybe_unused]]: when every plugin flag is enabled no `#else`
    // branch references this and -Werror=unused-function would fail.
    [[maybe_unused]] void ThrowPluginNotEnabled(JNIEnv* env, const char* message)
    {
        jclass excClass = env->FindClass("java/lang/IllegalStateException");
        if (excClass != nullptr)
        {
            env->ThrowNew(excClass, message);
        }
    }

    bool ApplyJavaRuntimeOptions(JNIEnv* env, jobject javaOptions, RuntimeOptions& options)
    {
        if (javaOptions == nullptr)
        {
            return true;
        }

        jclass optionsClass = env->GetObjectClass(javaOptions);
        if (optionsClass == nullptr)
        {
            return false;
        }

        auto readBoolean = [&](const char* name, bool& target) {
            jfieldID field = env->GetFieldID(optionsClass, name, "Z");
            if (field == nullptr)
            {
                return false;
            }
            target = env->GetBooleanField(javaOptions, field) == JNI_TRUE;
            return true;
        };

        if (!readBoolean("enableDebugger", options.enableDebugger) ||
            !readBoolean("enableDebugTrace", options.enableDebugTrace) ||
            !readBoolean("waitForDebugger", options.waitForDebugger))
        {
            env->DeleteLocalRef(optionsClass);
            return false;
        }

        jfieldID msaaSamplesField = env->GetFieldID(optionsClass, "msaaSamples", "Ljava/lang/Integer;");
        if (msaaSamplesField == nullptr)
        {
            env->DeleteLocalRef(optionsClass);
            return false;
        }

        jobject msaaSamples = env->GetObjectField(javaOptions, msaaSamplesField);
        if (msaaSamples != nullptr)
        {
            jclass integerClass = env->GetObjectClass(msaaSamples);
            if (integerClass == nullptr)
            {
                env->DeleteLocalRef(msaaSamples);
                env->DeleteLocalRef(optionsClass);
                return false;
            }

            jmethodID intValue = env->GetMethodID(integerClass, "intValue", "()I");
            if (intValue == nullptr)
            {
                env->DeleteLocalRef(integerClass);
                env->DeleteLocalRef(msaaSamples);
                env->DeleteLocalRef(optionsClass);
                return false;
            }

            const jint value = env->CallIntMethod(msaaSamples, intValue);
            options.msaaSamples = value >= 0 && value <= 255 ? static_cast<uint8_t>(value) : 0;
            env->DeleteLocalRef(integerClass);
            env->DeleteLocalRef(msaaSamples);
        }

        jfieldID shaderCachePathField = env->GetFieldID(optionsClass, "shaderCachePath", "Ljava/lang/String;");
        if (shaderCachePathField == nullptr)
        {
            env->DeleteLocalRef(optionsClass);
            return false;
        }

        auto shaderCachePath = static_cast<jstring>(env->GetObjectField(javaOptions, shaderCachePathField));
        if (shaderCachePath != nullptr)
        {
#if BABYLON_NATIVE_PLUGIN_SHADERCACHE
            options.shaderCachePath = ToStdString(env, shaderCachePath);
            env->DeleteLocalRef(shaderCachePath);
            if (env->ExceptionCheck())
            {
                env->DeleteLocalRef(optionsClass);
                return false;
            }
#else
            env->DeleteLocalRef(shaderCachePath);
            env->DeleteLocalRef(optionsClass);
            ThrowPluginNotEnabled(env,
                "shaderCachePath was provided but BABYLON_NATIVE_PLUGIN_SHADERCACHE "
                "was not enabled at native build time.");
            return false;
#endif
        }

        env->DeleteLocalRef(optionsClass);
        return true;
    }

    // Shared body for the runtimeCreate JNI overloads. Installs the
    // default logcat sink, constructs the Runtime, and registers
    // Activity-lifecycle auto-Suspend/Resume tickets.
    jlong MakeRuntimeHandle(RuntimeOptions options)
    {
        options.log = [](LogLevel level, std::string_view message) {
            // logcat needs a NUL-terminated C string.
            std::string text{message};
            __android_log_write(LogPriorityFor(level), "BabylonNative", text.c_str());
        };

        auto runtime = std::make_unique<Runtime>(std::move(options));
        Runtime* runtimePtr = runtime.get();

        // Auto-Suspend/Resume on process-wide Activity lifecycle. Hosts
        // call androidGlobalPause/Resume once per state change; every
        // Runtime in the process reacts. Refcounted, so composes with
        // any host-side explicit suspend/resume.
        auto pauseTicket = android::global::AddPauseCallback([runtimePtr]() {
            runtimePtr->Suspend();
        });
        auto resumeTicket = android::global::AddResumeCallback([runtimePtr]() {
            runtimePtr->Resume();
        });

        auto wrapper = std::unique_ptr<AndroidRuntime>{new AndroidRuntime{
            std::move(runtime),
            std::move(pauseTicket),
            std::move(resumeTicket),
        }};

        // Ownership transfers to the JVM side via the returned jlong.
        return reinterpret_cast<jlong>(wrapper.release());
    }
}

// Public handle-decoding entry point. Hosts that ship app-specific JNI
// helpers in `libBabylonNativeIntegrations.so` must use this rather than
// `reinterpret_cast<Runtime*>(handle)`, because each Runtime is wrapped
// in `AndroidRuntime` to hold Activity-lifecycle tickets.
namespace Babylon::Integrations::Android
{
    Runtime* RuntimeFromHandle(jlong handle)
    {
        return handle == 0 ? nullptr : AsRuntime(handle);
    }
}

extern "C"
{

// =====================================================================
// Android platform lifecycle (interop layer surface).
//
// Not part of the cross-platform Runtime/View API — exists because
// plugins like NativeCamera require the host to register the JavaVM and
// current Activity via AndroidExtensions::Globals.
// =====================================================================

// Set the application Context. Hosts call this once at app startup
// (typically from Application.onCreate) before constructing any Runtime.
// Calling more than once is harmless — Initialize replaces the existing
// Context global ref.
JNIEXPORT void JNICALL
Java_com_babylonjs_integrations_BabylonNative_setContext(
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
Java_com_babylonjs_integrations_BabylonNative_setCurrentActivity(
    JNIEnv*, jclass, jobject activity)
{
    android::global::SetCurrentActivity(activity);
}

JNIEXPORT void JNICALL
Java_com_babylonjs_integrations_BabylonNative_pause(JNIEnv*, jclass)
{
    android::global::Pause();
}

JNIEXPORT void JNICALL
Java_com_babylonjs_integrations_BabylonNative_resume(JNIEnv*, jclass)
{
    android::global::Resume();
}

JNIEXPORT void JNICALL
Java_com_babylonjs_integrations_BabylonNative_requestPermissionsResult(
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
Java_com_babylonjs_integrations_BabylonNative_runtimeCreate__(JNIEnv*, jclass)
{
    // Default Android consumers want logcat output; Console polyfill,
    // uncaught JS exceptions, and (when enabled) DebugTrace all route
    // there. Hosts wanting different behavior use the C++ API directly.
    RuntimeOptions options{};
    return MakeRuntimeHandle(std::move(options));
}

JNIEXPORT jlong JNICALL
Java_com_babylonjs_integrations_BabylonNative_runtimeCreate__Lcom_babylonjs_integrations_BabylonNative_00024RuntimeOptions_2(
    JNIEnv* env, jclass, jobject javaOptions)
{
    // RuntimeOptions is a Java object so callers have a stable
    // construction API as fields are added.
    RuntimeOptions options{};
    if (!ApplyJavaRuntimeOptions(env, javaOptions, options))
    {
        return 0;
    }
    return MakeRuntimeHandle(std::move(options));
}

JNIEXPORT void JNICALL
Java_com_babylonjs_integrations_BabylonNative_runtimeDestroy(JNIEnv*, jclass, jlong handle)
{
    // Reverse declaration order: tickets unsubscribe before the Runtime
    // is destroyed, so no callback fires on a dead Runtime.
    delete AsAndroidRuntime(handle);
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

// No per-Runtime Suspend/Resume on the JNI surface: lifecycle wiring
// happens automatically in MakeRuntimeHandle via the android::global
// pause/resume tickets. Hosts call androidGlobalPause/Resume once per
// Activity state change; every Runtime in the process reacts. Use the
// C++ API for finer-grained control.

JNIEXPORT void JNICALL
Java_com_babylonjs_integrations_BabylonNative_runtimeSetXrSurface(
    JNIEnv* env, jclass, jlong handle, jobject surface)
{
#if BABYLON_NATIVE_PLUGIN_NATIVEXR
    ANativeWindow* window{nullptr};
    if (surface != nullptr)
    {
        window = ANativeWindow_fromSurface(env, surface);
    }
    AsRuntime(handle)->SetXrWindow(window);
#else
    (void)handle;
    (void)surface;
    ThrowPluginNotEnabled(env,
        "runtimeSetXrSurface was called but BABYLON_NATIVE_PLUGIN_NATIVEXR "
        "was not enabled at native build time.");
#endif
}

JNIEXPORT jboolean JNICALL
Java_com_babylonjs_integrations_BabylonNative_runtimeIsXrActive(JNIEnv*, jclass, jlong handle)
{
#if BABYLON_NATIVE_PLUGIN_NATIVEXR
    return AsRuntime(handle)->IsXrActive() ? JNI_TRUE : JNI_FALSE;
#else
    // Non-throwing: "no XR active" is correct when XR is off.
    (void)handle;
    return JNI_FALSE;
#endif
}

// =====================================================================
// View
// =====================================================================

JNIEXPORT jlong JNICALL
Java_com_babylonjs_integrations_BabylonNative_viewAttach(
    JNIEnv* env, jclass, jlong runtimeHandle, jobject surface)
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
    View* view = nullptr;
    try
    {
        view = new View{*AsRuntime(runtimeHandle), window};
    }
    catch (const std::exception&)
    {
        ANativeWindow_release(window);
        return 0;
    }

    // Kick off the first resize using the surface's current pixel
    // dimensions. Hosts will typically also wire their SurfaceHolder
    // callback's `surfaceChanged(holder, format, w, h)` to `viewResize`
    // for subsequent size changes — this initial call composes cleanly
    // with that because `View::Resize` past the first call is an
    // idempotent `Device::UpdateSize`. If the surface reports zero
    // (rare; surface not fully realized yet), skip and rely on the
    // host's `surfaceChanged` to deliver the first size.
    const int32_t surfaceWidth = ANativeWindow_getWidth(window);
    const int32_t surfaceHeight = ANativeWindow_getHeight(window);
    if (surfaceWidth > 0 && surfaceHeight > 0)
    {
        view->Resize(static_cast<uint32_t>(surfaceWidth),
                      static_cast<uint32_t>(surfaceHeight),
                      Babylon::Integrations::CoordinateUnits::Physical);
    }

    // bgfx retains its own reference on the ANativeWindow for the
    // surface-binding lifetime, so release our local acquire here.
    ANativeWindow_release(window);
    return reinterpret_cast<jlong>(view);
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
    JNIEnv*, jclass, jlong handle, jint width, jint height)
{
    // Java callers pass the SurfaceView's pixel-buffer dimensions
    // (physical pixels on Android).
    AsView(handle)->Resize(static_cast<uint32_t>(width),
                            static_cast<uint32_t>(height),
                            Babylon::Integrations::CoordinateUnits::Physical);
}

JNIEXPORT void JNICALL
Java_com_babylonjs_integrations_BabylonNative_viewPointerDown(
    JNIEnv* env, jclass, jlong handle, jint pointerId, jfloat x, jfloat y)
{
#if BABYLON_NATIVE_PLUGIN_NATIVEINPUT
    (void)env;
    // MotionEvent.getX/getY are physical pixels.
    AsView(handle)->OnPointerDown(static_cast<int32_t>(pointerId), x, y,
                                   Babylon::Integrations::CoordinateUnits::Physical);
#else
    (void)handle; (void)pointerId; (void)x; (void)y;
    ThrowPluginNotEnabled(env,
        "viewPointerDown was called but BABYLON_NATIVE_PLUGIN_NATIVEINPUT "
        "was not enabled at native build time.");
#endif
}

JNIEXPORT void JNICALL
Java_com_babylonjs_integrations_BabylonNative_viewPointerMove(
    JNIEnv* env, jclass, jlong handle, jint pointerId, jfloat x, jfloat y)
{
#if BABYLON_NATIVE_PLUGIN_NATIVEINPUT
    (void)env;
    AsView(handle)->OnPointerMove(static_cast<int32_t>(pointerId), x, y,
                                   Babylon::Integrations::CoordinateUnits::Physical);
#else
    (void)handle; (void)pointerId; (void)x; (void)y;
    ThrowPluginNotEnabled(env,
        "viewPointerMove was called but BABYLON_NATIVE_PLUGIN_NATIVEINPUT "
        "was not enabled at native build time.");
#endif
}

JNIEXPORT void JNICALL
Java_com_babylonjs_integrations_BabylonNative_viewPointerUp(
    JNIEnv* env, jclass, jlong handle, jint pointerId, jfloat x, jfloat y)
{
#if BABYLON_NATIVE_PLUGIN_NATIVEINPUT
    (void)env;
    AsView(handle)->OnPointerUp(static_cast<int32_t>(pointerId), x, y,
                                 Babylon::Integrations::CoordinateUnits::Physical);
#else
    (void)handle; (void)pointerId; (void)x; (void)y;
    ThrowPluginNotEnabled(env,
        "viewPointerUp was called but BABYLON_NATIVE_PLUGIN_NATIVEINPUT "
        "was not enabled at native build time.");
#endif
}

} // extern "C"
