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

    // Wraps a Runtime with two `android::global` event tickets that
    // auto-Suspend/Resume the Runtime in response to process-wide
    // Activity lifecycle notifications. Member declaration order matters:
    // tickets are declared *after* the Runtime so they're destroyed
    // *before* the Runtime, which guarantees no callback can fire on a
    // dead Runtime during teardown.
    //
    // Suspend/Resume on Babylon::Integrations::Runtime is reference-counted,
    // so the auto-suspend composes safely with explicit host-side
    // runtimeSuspend / runtimeResume calls.
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
            case LogLevel::Log:   return ANDROID_LOG_INFO;
            case LogLevel::Warn:  return ANDROID_LOG_WARN;
            case LogLevel::Error: return ANDROID_LOG_ERROR;
            case LogLevel::Fatal: return ANDROID_LOG_FATAL;
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

    // Throws a Java `IllegalStateException` with the given message.
    // Used by interop entry points whose underlying plugin was not
    // compiled into the native library, so the misconfiguration
    // surfaces as a clean Java exception instead of an
    // UnsatisfiedLinkError or silent no-op.
    //
    // `[[maybe_unused]]` is intentional: when every plugin flag is
    // enabled, no `#else` branch in any JNI entry point invokes this
    // helper, and -Werror=unused-function would otherwise fail the
    // build.
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

    // Shared body for the `runtimeCreate` JNI overloads. Wires up the
    // Android-default logcat log sink, constructs the Runtime, attaches
    // the Activity-lifecycle auto-Suspend/Resume tickets, and returns
    // the opaque jlong handle the JVM side holds onto.
    jlong MakeRuntimeHandle(RuntimeOptions options)
    {
        options.log = [](LogLevel level, std::string_view message) {
            // logcat takes a NUL-terminated C string; copy the view.
            std::string text{message};
            __android_log_write(LogPriorityFor(level), "BabylonNative", text.c_str());
        };

        // Construct in two phases because the AppStateChangedCallbackTicket
        // is neither default-constructible nor move-assignable: we need the
        // Runtime pointer in hand before we can register the callbacks, and
        // we register the callbacks before the wrapper itself exists.
        auto runtime = Runtime::Create(std::move(options));
        Runtime* runtimePtr = runtime.get();

        // Auto-Suspend/Resume on Activity lifecycle. Hosts call
        // pause / resume from their Activity's onPause / onResume;
        // every Runtime in the process gets suspended and resumed
        // automatically. Since Runtime::Suspend/Resume are refcounted,
        // this composes safely with any explicit runtimeSuspend /
        // runtimeResume calls the host might make for finer-grained
        // reasons (e.g. modal dialogs).
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
// helpers in the same `libBabylonNativeIntegrations.so` (e.g. the
// Playground's PlaygroundJNI.cpp for the Babylon.js bootstrap script
// list) call this to get back a Runtime* from the opaque jlong returned
// by `runtimeCreate`. Direct `reinterpret_cast<Runtime*>(handle)` is
// wrong because each Runtime is wrapped in `AndroidRuntime` to hold
// Activity-lifecycle tickets.
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
// Android-specific platform lifecycle (platform interop layer surface).
//
// These don't belong on the cross-platform Runtime/View API — they exist
// because Babylon Native plugins like NativeCamera require the host to
// register the JavaVM + current Activity via AndroidExtensions::Globals.
// See SimplifiedAPI.md §4.2 "Interop layer responsibilities".
// =====================================================================

// Set the application Context. Hosts call this once at app startup
// (typically from `Application.onCreate` or the host Activity's
// `onCreate`), before constructing any Runtime. Calling more than
// once is harmless — `android::global::Initialize` deletes any
// existing Context global ref and installs the new one.
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
    // Default Android consumers want logcat output; route Console
    // polyfill output and uncaught JS exceptions there. DebugTrace is routed
    // there when enabled by RuntimeOptions. Hosts that need different behavior
    // can construct a Runtime in C++ directly with their own RuntimeOptions.
    RuntimeOptions options{};
    return MakeRuntimeHandle(std::move(options));
}

JNIEXPORT jlong JNICALL
Java_com_babylonjs_integrations_BabylonNative_runtimeCreate__Lcom_babylonjs_integrations_BabylonNative_00024RuntimeOptions_2(
    JNIEnv* env, jclass, jobject javaOptions)
{
    // RuntimeOptions is intentionally a Java object so Java and Kotlin callers
    // can use a stable construction API as RuntimeOptions grows.
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
    // Member dtor order (reverse declaration): resumeTicket → pauseTicket
    // → runtime. The tickets unsubscribe before the Runtime is destroyed,
    // so no callback can fire on a dead Runtime during teardown.
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

// Note: there is intentionally no per-Runtime Suspend/Resume on the JNI
// surface. Activity-lifecycle Suspend/Resume is wired up automatically
// inside runtimeCreate above (each Runtime subscribes to
// android::global pause/resume callbacks). Hosts only call
// `androidGlobalPause` / `androidGlobalResume` once per Activity state
// change; every Runtime in the process reacts. Hosts that need
// finer-grained control should use the C++ API directly.

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
    // State query: "no XR session is active" is the correct answer
    // when XR isn't compiled in, so this is intentionally a
    // non-throwing path.
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
    auto view = View::Attach(*AsRuntime(runtimeHandle), window);
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
    // Java callers pass `MotionEvent.getX/getY`, which are in physical pixels.
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
