// Babylon Native JNI bridge.
//
// All native lifecycle and runtime initialization logic lives in this single
// translation unit so the AAR has no transitive dependency on the Playground
// AppContext sources. The consuming application is expected to drive script
// loading explicitly via Wrapper.loadScript / Wrapper.eval — nothing is
// auto-loaded by this layer.

#include <jni.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <android/log.h>
#include <android/asset_manager.h>

#include <Babylon/AppRuntime.h>
#include <Babylon/DebugTrace.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/PerfTrace.h>
#include <Babylon/ScriptLoader.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <Babylon/Plugins/ShaderCache.h>
#include <Babylon/Polyfills/Blob.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/Window.h>

#include <AndroidExtensions/Globals.h>

#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace
{
    constexpr const char* LOG_TAG = "BabylonNative";
    constexpr const char* SHADER_CACHE_ASSET = "shadercache.bin";

    struct Context
    {
        std::optional<Babylon::Graphics::Device> device;
        std::optional<Babylon::Graphics::DeviceUpdate> deviceUpdate;
        std::optional<Babylon::AppRuntime> runtime;
        std::optional<Babylon::ScriptLoader> scriptLoader;
    };

    std::optional<Context> g_context;

    void DebugLog(const char* msg)
    {
        __android_log_write(ANDROID_LOG_INFO, LOG_TAG, msg);
    }

    const char* GetLogLevelString(Babylon::Polyfills::Console::LogLevel logLevel)
    {
        switch (logLevel)
        {
            case Babylon::Polyfills::Console::LogLevel::Log:   return "Log";
            case Babylon::Polyfills::Console::LogLevel::Warn:  return "Warn";
            case Babylon::Polyfills::Console::LogLevel::Error: return "Error";
            default: return "";
        }
    }

    // Loads `shadercache.bin` from the embedding application's asset folder
    // and seeds the Babylon Native shader cache with it. If the asset is
    // missing the runtime still works but every shader is compiled on first
    // use.
    void LoadShaderCacheFromAssets()
    {
        AAssetManager* assetMgr = android::global::GetAssetManager();
        if (assetMgr == nullptr)
        {
            return;
        }

        AAsset* asset = AAssetManager_open(assetMgr, SHADER_CACHE_ASSET, AASSET_MODE_BUFFER);
        if (asset == nullptr)
        {
            return;
        }

        const void* data = AAsset_getBuffer(asset);
        off_t size = AAsset_getLength(asset);
        if (data != nullptr && size > 0)
        {
            std::string buf(reinterpret_cast<const char*>(data), static_cast<size_t>(size));
            std::istringstream stream(buf);
            const auto entries = Babylon::Plugins::ShaderCache::Load(stream);
            __android_log_print(ANDROID_LOG_INFO, LOG_TAG,
                "Shader cache loaded from assets: %u entries", entries);
        }
        AAsset_close(asset);
    }

    void InitializeContext(ANativeWindow* window, size_t width, size_t height)
    {
        g_context.emplace();

        Babylon::DebugTrace::EnableDebugTrace(true);
        Babylon::DebugTrace::SetTraceOutput(DebugLog);
        Babylon::PerfTrace::SetLevel(Babylon::PerfTrace::Level::Mark);

        Babylon::Graphics::Configuration graphicsConfig{};
        graphicsConfig.Window = window;
        graphicsConfig.Width = width;
        graphicsConfig.Height = height;
        graphicsConfig.MSAASamples = 4;

        g_context->device.emplace(graphicsConfig);
        g_context->deviceUpdate.emplace(g_context->device->GetUpdate("update"));

        Babylon::Plugins::ShaderCache::Enable();
        LoadShaderCacheFromAssets();

        g_context->device->StartRenderingCurrentFrame();
        g_context->deviceUpdate->Start();

        Babylon::AppRuntime::Options options{};
        options.UnhandledExceptionHandler = [](const Napi::Error& error) {
            std::ostringstream ss{};
            ss << "[Uncaught Error] " << Napi::GetErrorString(error);
            DebugLog(ss.str().c_str());
        };

        g_context->runtime.emplace(options);

        g_context->runtime->Dispatch([](Napi::Env env) {
            g_context->device->AddToJavaScript(env);

            Babylon::Polyfills::Blob::Initialize(env);
            Babylon::Polyfills::Console::Initialize(env,
                [](const char* message, Babylon::Polyfills::Console::LogLevel logLevel) {
                    std::ostringstream ss{};
                    ss << "[" << GetLogLevelString(logLevel) << "] " << message;
                    DebugLog(ss.str().c_str());
                });
            Babylon::Polyfills::Window::Initialize(env);

            Babylon::Plugins::NativeEngine::Initialize(env);
        });

        g_context->scriptLoader.emplace(*g_context->runtime);
    }

    void DestroyContext()
    {
        if (!g_context)
        {
            return;
        }

        if (g_context->device)
        {
            g_context->deviceUpdate->Finish();
            g_context->device->FinishRenderingCurrentFrame();
        }

        Babylon::Plugins::ShaderCache::Disable();

        g_context->scriptLoader.reset();
        g_context->runtime.reset();
        g_context->deviceUpdate.reset();
        g_context->device.reset();
        g_context.reset();
    }
}

extern "C"
{
    JNIEXPORT void JNICALL
    Java_com_library_babylonnative_Wrapper_initEngine(JNIEnv*, jclass)
    {
    }

    JNIEXPORT void JNICALL
    Java_com_library_babylonnative_Wrapper_finishEngine(JNIEnv*, jclass)
    {
        DestroyContext();
    }

    JNIEXPORT void JNICALL
    Java_com_library_babylonnative_Wrapper_surfaceCreated(JNIEnv* env, jclass, jobject surface, jobject jniContext)
    {
        if (g_context)
        {
            return;
        }

        JavaVM* javaVM{};
        if (env->GetJavaVM(&javaVM) != JNI_OK)
        {
            throw std::runtime_error("Failed to get Java VM");
        }

        android::global::Initialize(javaVM, jniContext);

        ANativeWindow* window = ANativeWindow_fromSurface(env, surface);
        const int32_t width  = ANativeWindow_getWidth(window);
        const int32_t height = ANativeWindow_getHeight(window);

        InitializeContext(window, static_cast<size_t>(width), static_cast<size_t>(height));
    }

    JNIEXPORT void JNICALL
    Java_com_library_babylonnative_Wrapper_surfaceChanged(JNIEnv* env, jclass, jint width, jint height, jobject surface)
    {
        if (g_context && g_context->runtime)
        {
            ANativeWindow* window = ANativeWindow_fromSurface(env, surface);
            g_context->runtime->Dispatch(
                [window, w = static_cast<size_t>(width), h = static_cast<size_t>(height)](auto) {
                    g_context->device->UpdateWindow(window);
                    g_context->device->UpdateSize(w, h);
                });
        }
    }

    JNIEXPORT void JNICALL
    Java_com_library_babylonnative_Wrapper_setCurrentActivity(JNIEnv*, jclass, jobject currentActivity)
    {
        android::global::SetCurrentActivity(currentActivity);
    }

    JNIEXPORT void JNICALL
    Java_com_library_babylonnative_Wrapper_activityOnPause(JNIEnv*, jclass)
    {
        android::global::Pause();
        if (g_context && g_context->runtime)
        {
            g_context->runtime->Suspend();
        }
    }

    JNIEXPORT void JNICALL
    Java_com_library_babylonnative_Wrapper_activityOnResume(JNIEnv*, jclass)
    {
        if (g_context && g_context->runtime)
        {
            g_context->runtime->Resume();
        }
        android::global::Resume();
    }

    JNIEXPORT void JNICALL
    Java_com_library_babylonnative_Wrapper_activityOnRequestPermissionsResult(
        JNIEnv* env, jclass, jint requestCode, jobjectArray permissions, jintArray grantResults)
    {
        std::vector<std::string> nativePermissions{};
        const jsize permissionCount = env->GetArrayLength(permissions);
        for (jsize i = 0; i < permissionCount; i++)
        {
            jstring permission = static_cast<jstring>(env->GetObjectArrayElement(permissions, i));
            const char* utfString = env->GetStringUTFChars(permission, nullptr);
            nativePermissions.emplace_back(utfString);
            env->ReleaseStringUTFChars(permission, utfString);
        }

        jint* grantResultElements = env->GetIntArrayElements(grantResults, nullptr);
        const jsize grantResultCount = env->GetArrayLength(grantResults);
        std::vector<int32_t> nativeGrantResults{grantResultElements, grantResultElements + grantResultCount};
        env->ReleaseIntArrayElements(grantResults, grantResultElements, 0);

        android::global::RequestPermissionsResult(requestCode, nativePermissions, nativeGrantResults);
    }

    JNIEXPORT void JNICALL
    Java_com_library_babylonnative_Wrapper_loadScript(JNIEnv* env, jclass, jstring path)
    {
        if (g_context && g_context->scriptLoader)
        {
            const char* nativePath = env->GetStringUTFChars(path, nullptr);
            g_context->scriptLoader->LoadScript(nativePath);
            env->ReleaseStringUTFChars(path, nativePath);
        }
    }

    JNIEXPORT void JNICALL
    Java_com_library_babylonnative_Wrapper_eval(JNIEnv* env, jclass, jstring source, jstring sourceURL)
    {
        if (g_context && g_context->scriptLoader)
        {
            const char* nativeURL = env->GetStringUTFChars(sourceURL, nullptr);
            const char* nativeSource = env->GetStringUTFChars(source, nullptr);
            g_context->scriptLoader->Eval(std::string{nativeSource}, std::string{nativeURL});
            env->ReleaseStringUTFChars(source, nativeSource);
            env->ReleaseStringUTFChars(sourceURL, nativeURL);
        }
    }

    // No NativeInput linked — touch events are ignored at this layer.
    JNIEXPORT void JNICALL
    Java_com_library_babylonnative_Wrapper_setTouchInfo(JNIEnv*, jclass, jint, jfloat, jfloat, jboolean, jint)
    {
    }

    JNIEXPORT void JNICALL
    Java_com_library_babylonnative_Wrapper_renderFrame(JNIEnv*, jclass)
    {
        if (g_context && g_context->device)
        {
            g_context->deviceUpdate->Finish();
            g_context->device->FinishRenderingCurrentFrame();
            g_context->device->StartRenderingCurrentFrame();
            g_context->deviceUpdate->Start();
        }
    }

    // No NativeXr linked — XR surface changes are ignored at this layer.
    JNIEXPORT void JNICALL
    Java_com_library_babylonnative_Wrapper_xrSurfaceChanged(JNIEnv*, jclass, jobject)
    {
    }

    JNIEXPORT jboolean JNICALL
    Java_com_library_babylonnative_Wrapper_isXRActive(JNIEnv*, jclass)
    {
        return JNI_FALSE;
    }
}
