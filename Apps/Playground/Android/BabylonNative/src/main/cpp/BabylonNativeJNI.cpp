#include <jni.h>
#include <android/native_window.h> // requires ndk r5 or newer
#include <android/native_window_jni.h> // requires ndk r5 or newer
#include <android/log.h>

#include <cstdio>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include <Shared/AppContext.h>
#include <AndroidExtensions/Globals.h>
#include <Babylon/Plugins/NativeInput.h>
#include <napi/napi.h>

namespace
{
    std::optional<AppContext> appContext{};
    JavaVM* sJavaVM{};
}

extern "C"
{
    JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void*)
    {
        sJavaVM = vm;
        return JNI_VERSION_1_6;
    }

    JNIEXPORT void JNICALL
    Java_com_library_babylonnative_Wrapper_initEngine(JNIEnv* env, jclass clazz)
    {
    }

    JNIEXPORT void JNICALL
    Java_com_library_babylonnative_Wrapper_finishEngine(JNIEnv* env, jclass clazz)
    {
        appContext.reset();
    }

    JNIEXPORT void JNICALL
    Java_com_library_babylonnative_Wrapper_surfaceCreated(JNIEnv* env, jclass clazz, jobject surface, jobject jniContext)
    {
        if (!appContext)
        {
            char pointerLog[256]{};
            std::snprintf(pointerLog, sizeof(pointerLog), "surfaceCreated env=%p sJavaVM(before)=%p", static_cast<void*>(env), static_cast<void*>(sJavaVM));
            __android_log_write(ANDROID_LOG_INFO, "BabylonNative", pointerLog);

            if (sJavaVM == nullptr && env != nullptr)
            {
                JavaVM* javaVM{};
                if (env->GetJavaVM(&javaVM) == JNI_OK)
                {
                    sJavaVM = javaVM;
                    std::snprintf(pointerLog, sizeof(pointerLog), "surfaceCreated sJavaVM(from env)=%p", static_cast<void*>(sJavaVM));
                    __android_log_write(ANDROID_LOG_INFO, "BabylonNative", pointerLog);
                }
            }

            if (sJavaVM == nullptr)
            {
                throw std::runtime_error{"Failed to get Java VM"};
            }

            android::global::Initialize(sJavaVM, jniContext);

            ANativeWindow* window = ANativeWindow_fromSurface(env, surface);
            int32_t width  = ANativeWindow_getWidth(window);
            int32_t height = ANativeWindow_getHeight(window);

            appContext.emplace(
                window,
                static_cast<size_t>(width),
                static_cast<size_t>(height),
                [](const char* message) {
                    __android_log_write(ANDROID_LOG_INFO, "BabylonNative", message);
                },
                [](Napi::Env env) {
                    Napi::HandleScope scope{env};
                    auto statusCallback = Napi::Function::New(env, [](const Napi::CallbackInfo& info) {
                        if (info.Length() > 0)
                        {
                            std::string message{};
                            if (info[0].IsString())
                            {
                                message = info[0].As<Napi::String>().Utf8Value();
                            }
                            else
                            {
                                message = info[0].ToString().Utf8Value();
                            }

                            const auto taggedMessage = std::string{"[Playground] "} + message;
                            __android_log_write(ANDROID_LOG_INFO, "BabylonNative", taggedMessage.c_str());
                        }
                    });
                    env.Global().Set("__nativePlaygroundStatus", statusCallback);
                });
        }
    }

    JNIEXPORT void JNICALL
    Java_com_library_babylonnative_Wrapper_surfaceChanged(JNIEnv* env, jclass clazz, jint width, jint height, jobject surface)
    {
        if (appContext)
        {
            ANativeWindow* window = ANativeWindow_fromSurface(env, surface);
            appContext->Runtime().Dispatch([window, width = static_cast<size_t>(width), height = static_cast<size_t>(height)](auto) {
                appContext->Device().UpdateWindow(window);
                appContext->Device().UpdateSize(width, height);
            });
        }
    }

    JNIEXPORT void JNICALL
    Java_com_library_babylonnative_Wrapper_setCurrentActivity(JNIEnv* env, jclass clazz, jobject currentActivity)
    {
        android::global::SetCurrentActivity(currentActivity);
    }

    JNIEXPORT void JNICALL
    Java_com_library_babylonnative_Wrapper_activityOnPause(JNIEnv* env, jclass clazz)
    {
        android::global::Pause();
        if (appContext)
        {
            appContext->Runtime().Suspend();
        }
    }

    JNIEXPORT void JNICALL
    Java_com_library_babylonnative_Wrapper_activityOnResume(JNIEnv* env, jclass clazz)
    {
        if (appContext)
        {
            appContext->Runtime().Resume();
        }
        android::global::Resume();
    }

    JNIEXPORT void JNICALL
    Java_com_library_babylonnative_Wrapper_activityOnRequestPermissionsResult(JNIEnv* env, jclass clazz, jint requestCode, jobjectArray permissions, jintArray grantResults)
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
    Java_com_library_babylonnative_Wrapper_loadScript(JNIEnv* env, jclass clazz, jstring path)
    {
        if (appContext)
        {
            const char* sourcePath = env->GetStringUTFChars(path, nullptr);
            appContext->ScriptLoader().LoadScript(sourcePath);
            env->ReleaseStringUTFChars(path, sourcePath);
        }
    }

    JNIEXPORT void JNICALL
    Java_com_library_babylonnative_Wrapper_eval(JNIEnv* env, jclass clazz, jstring source, jstring sourceURL)
    {
        if (appContext)
        {
            const char* sourceUrlChars = env->GetStringUTFChars(sourceURL, nullptr);
            const char* sourceChars = env->GetStringUTFChars(source, nullptr);
            std::string url = sourceUrlChars;
            std::string src = sourceChars;
            env->ReleaseStringUTFChars(sourceURL, sourceUrlChars);
            env->ReleaseStringUTFChars(source, sourceChars);
            appContext->ScriptLoader().Eval(std::move(src), std::move(url));
        }
    }

    JNIEXPORT void JNICALL
    Java_com_library_babylonnative_Wrapper_setTouchInfo(JNIEnv* env, jclass clazz, jint pointerId, jfloat x, jfloat y, jboolean buttonAction, jint buttonValue)
    {
        if (appContext && appContext->Input())
        {
            if (buttonAction)
            {
                if (buttonValue == 1)
                    appContext->Input()->TouchDown(pointerId, x, y);
                else
                    appContext->Input()->TouchUp(pointerId, x, y);
            }
            else {
                appContext->Input()->TouchMove(pointerId, x, y);
            }
        }
    }

    JNIEXPORT void JNICALL
    Java_com_library_babylonnative_Wrapper_renderFrame(JNIEnv* env, jclass clazz)
    {
        if (appContext)
        {
            appContext->DeviceUpdate().Finish();
            appContext->Device().FinishRenderingCurrentFrame();
            appContext->Device().StartRenderingCurrentFrame();
            appContext->DeviceUpdate().Start();
        }
    }

    JNIEXPORT void JNICALL
    Java_com_library_babylonnative_Wrapper_xrSurfaceChanged(JNIEnv* env, jclass clazz, jobject surface)
    {
        // Native XR integration is disabled in this WGPU migration spike.
    }

    JNIEXPORT jboolean JNICALL
    Java_com_library_babylonnative_Wrapper_isXRActive(JNIEnv* env, jclass clazz)
    {
        return false;
    }
}
