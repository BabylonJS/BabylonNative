#include <jni.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <memory>
#include <android/native_window.h> // requires ndk r5 or newer
#include <android/native_window_jni.h> // requires ndk r5 or newer
#include <android/log.h>

#include <AndroidExtensions/Globals.h>
#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics.h>
#include <Babylon/ScriptLoader.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <Babylon/Plugins/NativeXr.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/Window.h>
#include <Babylon/Polyfills/XMLHttpRequest.h>
#include <InputManager.h>

namespace
{
    std::unique_ptr<Babylon::Graphics> g_graphics{};
    std::unique_ptr<Babylon::AppRuntime> g_runtime{};
    std::unique_ptr<InputManager<Babylon::AppRuntime>::InputBuffer> g_inputBuffer{};
    std::unique_ptr<Babylon::ScriptLoader> g_scriptLoader{};
    std::optional<Babylon::Plugins::NativeXr> g_nativeXr{};
    bool g_isXrActive{};
}

extern "C"
{
    JNIEXPORT void JNICALL
    Java_BabylonNative_Wrapper_initEngine(JNIEnv* env, jclass clazz)
    {
    }

    JNIEXPORT void JNICALL
    Java_BabylonNative_Wrapper_finishEngine(JNIEnv* env, jclass clazz)
    {
        if (g_graphics)
        {
            g_graphics->FinishRenderingCurrentFrame();
        }

        g_nativeXr.reset();
        g_scriptLoader.reset();
        g_inputBuffer.reset();
        g_runtime.reset();
        g_graphics.reset();

        g_isXrActive = false;
    }

    JNIEXPORT void JNICALL
    Java_BabylonNative_Wrapper_surfaceCreated(JNIEnv* env, jclass clazz, jobject surface, jobject context)
    {
        if (!g_runtime)
        {
            JavaVM* javaVM{};
            if (env->GetJavaVM(&javaVM) != JNI_OK)
            {
                throw std::runtime_error("Failed to get Java VM");
            }

            android::global::Initialize(javaVM, context);

            ANativeWindow* window = ANativeWindow_fromSurface(env, surface);
            int32_t width  = ANativeWindow_getWidth(window);
            int32_t height = ANativeWindow_getHeight(window);

            Babylon::WindowConfiguration graphicsConfig{};
            graphicsConfig.WindowPtr = window;
            graphicsConfig.Width = static_cast<size_t>(width);
            graphicsConfig.Height = static_cast<size_t>(height);
            g_graphics = Babylon::Graphics::CreateGraphics(graphicsConfig);
            g_graphics->StartRenderingCurrentFrame();

            g_runtime = std::make_unique<Babylon::AppRuntime>();
            g_inputBuffer = std::make_unique<InputManager<Babylon::AppRuntime>::InputBuffer>(*g_runtime);

            g_runtime->Dispatch([](Napi::Env env)
            {
                g_graphics->AddToJavaScript(env);

                Babylon::Polyfills::Console::Initialize(env, [](const char* message, Babylon::Polyfills::Console::LogLevel level)
                {
                    switch (level)
                    {
                    case Babylon::Polyfills::Console::LogLevel::Log:
                        __android_log_write(ANDROID_LOG_INFO, "BabylonNative", message);
                        break;
                    case Babylon::Polyfills::Console::LogLevel::Warn:
                        __android_log_write(ANDROID_LOG_WARN, "BabylonNative", message);
                        break;
                    case Babylon::Polyfills::Console::LogLevel::Error:
                        __android_log_write(ANDROID_LOG_ERROR, "BabylonNative", message);
                        break;
                    }
                });

                Babylon::Plugins::NativeEngine::Initialize(env);

                g_nativeXr.emplace(Babylon::Plugins::NativeXr::Initialize(env));
                g_nativeXr->SetSessionStateChangedCallback([](bool isXrActive){ g_isXrActive = isXrActive; });

                Babylon::Polyfills::Window::Initialize(env);

                Babylon::Polyfills::XMLHttpRequest::Initialize(env);

                InputManager<Babylon::AppRuntime>::Initialize(env, *g_inputBuffer);
            });

            g_scriptLoader = std::make_unique<Babylon::ScriptLoader>(*g_runtime);
            g_scriptLoader->Eval("document = {}", "");
            g_scriptLoader->LoadScript("app:///Scripts/ammo.js");
            g_scriptLoader->LoadScript("app:///Scripts/recast.js");
            g_scriptLoader->LoadScript("app:///Scripts/babylon.max.js");
            g_scriptLoader->LoadScript("app:///Scripts/babylonjs.loaders.js");
            g_scriptLoader->LoadScript("app:///Scripts/babylonjs.materials.js");
            g_scriptLoader->LoadScript("app:///Scripts/babylon.gui.js");
        }
    }

    JNIEXPORT void JNICALL
    Java_BabylonNative_Wrapper_surfaceChanged(JNIEnv* env, jclass clazz, jint width, jint height, jobject surface)
    {
        if (g_runtime)
        {
            ANativeWindow *window = ANativeWindow_fromSurface(env, surface);
            g_runtime->Dispatch([window, width = static_cast<size_t>(width), height = static_cast<size_t>(height)](auto env) {
                Babylon::WindowConfiguration graphicsConfig{};
                graphicsConfig.WindowPtr = window;
                graphicsConfig.Width = width;
                graphicsConfig.Height = height;
                g_graphics->UpdateWindow(graphicsConfig);
                g_graphics->UpdateSize(width, height);
            });
        }
    }

    JNIEXPORT void JNICALL
    Java_BabylonNative_Wrapper_setCurrentActivity(JNIEnv* env, jclass clazz, jobject currentActivity)
    {
        android::global::SetCurrentActivity(currentActivity);
    }

    JNIEXPORT void JNICALL
    Java_BabylonNative_Wrapper_activityOnPause(JNIEnv* env, jclass clazz)
    {
        android::global::Pause();
        if (g_runtime)
        {
            g_runtime->Suspend();
        }
    }

    JNIEXPORT void JNICALL
    Java_BabylonNative_Wrapper_activityOnResume(JNIEnv* env, jclass clazz)
    {
        if (g_runtime)
        {
            g_runtime->Resume();
        }
        android::global::Resume();
    }

    JNIEXPORT void JNICALL
    Java_BabylonNative_Wrapper_activityOnRequestPermissionsResult(JNIEnv* env, jclass clazz, jint requestCode, jobjectArray permissions, jintArray grantResults)
    {
        std::vector<std::string> nativePermissions{};
        for (int i = 0; i < env->GetArrayLength(permissions); i++)
        {
            jstring permission = (jstring)env->GetObjectArrayElement(permissions, i);
            const char* utfString{env->GetStringUTFChars(permission, nullptr)};
            nativePermissions.push_back(utfString);
            env->ReleaseStringUTFChars(permission, utfString);
        }

        auto grantResultElements{env->GetIntArrayElements(grantResults, nullptr)};
        auto grantResultElementCount = env->GetArrayLength(grantResults);
        std::vector<int32_t> nativeGrantResults{grantResultElements, grantResultElements + grantResultElementCount};
        env->ReleaseIntArrayElements(grantResults, grantResultElements, 0);

        android::global::RequestPermissionsResult(requestCode, nativePermissions, nativeGrantResults);
    }

    JNIEXPORT void JNICALL
    Java_BabylonNative_Wrapper_loadScript(JNIEnv* env, jclass clazz, jstring path)
    {
        if (g_scriptLoader)
        {
            g_scriptLoader->LoadScript(env->GetStringUTFChars(path, nullptr));
        }
    }

    JNIEXPORT void JNICALL
    Java_BabylonNative_Wrapper_eval(JNIEnv* env, jclass clazz, jstring source, jstring sourceURL)
    {
        if (g_runtime)
        {
            std::string url = env->GetStringUTFChars(sourceURL, nullptr);
            std::string src = env->GetStringUTFChars(source, nullptr);
            g_scriptLoader->Eval(std::move(src), std::move(url));
        }
    }

    JNIEXPORT void JNICALL
    Java_BabylonNative_Wrapper_setTouchInfo(JNIEnv* env, jclass clazz, jfloat x, jfloat y, jboolean down)
    {
        if (g_inputBuffer != nullptr)
        {
            g_inputBuffer->SetPointerPosition(x, y);
            g_inputBuffer->SetPointerDown(down);
        }
    }

    JNIEXPORT void JNICALL
    Java_BabylonNative_Wrapper_renderFrame(JNIEnv* env, jclass clazz)
    {
        if (g_graphics)
        {
            g_graphics->FinishRenderingCurrentFrame();
            g_graphics->StartRenderingCurrentFrame();
        }
    }

    JNIEXPORT void JNICALL
    Java_BabylonNative_Wrapper_xrSurfaceChanged(JNIEnv* env, jclass clazz, jobject surface)
    {
        if (g_nativeXr)
        {
            ANativeWindow* window{};
            if (surface)
            {
                window = ANativeWindow_fromSurface(env, surface);
            }
            g_nativeXr->UpdateWindow(window);
        }
    }

    JNIEXPORT jboolean JNICALL
    Java_BabylonNative_Wrapper_isXRActive(JNIEnv* env, jclass clazz)
    {
        return g_isXrActive;
    }
}
