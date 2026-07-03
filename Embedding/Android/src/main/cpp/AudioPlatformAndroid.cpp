#include <Babylon/Embedding/Android/AudioPlatform.h>
#include <Babylon/Polyfills/AudioPlatform.h>

#include <jni.h>

namespace
{
    JavaVM* g_javaVM{nullptr};

    void RequestAndroidPlaybackFocus()
    {
        if (!g_javaVM)
        {
            return;
        }

        JNIEnv* env{nullptr};
        bool detach = false;
        const jint envStatus = g_javaVM->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);
        if (envStatus == JNI_EDETACHED)
        {
            if (g_javaVM->AttachCurrentThread(&env, nullptr) != JNI_OK)
            {
                return;
            }
            detach = true;
        }
        else if (envStatus != JNI_OK)
        {
            return;
        }

        jclass helperClass = env->FindClass("com/babylonjs/embedding/AudioFocusHelper");
        if (helperClass != nullptr)
        {
            jmethodID requestMethod = env->GetStaticMethodID(helperClass, "requestPlaybackFocus", "()V");
            if (requestMethod != nullptr)
            {
                env->CallStaticVoidMethod(helperClass, requestMethod);
            }
        }

        if (detach)
        {
            g_javaVM->DetachCurrentThread();
        }
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_babylonjs_embedding_AudioFocusHelper_nativeNotifyAudioInterruption(JNIEnv*, jclass, jboolean began)
{
    Babylon::Polyfills::Internal::NotifyAudioInterruption(began == JNI_TRUE);
}

void Babylon::Embedding::Android::RegisterWebAudioPlatform(JNIEnv* env, jobject context, JavaVM* javaVM)
{
    g_javaVM = javaVM;

    jclass helperClass = env->FindClass("com/babylonjs/embedding/AudioFocusHelper");
    if (helperClass != nullptr)
    {
        jmethodID initializeMethod = env->GetStaticMethodID(helperClass, "initialize", "(Landroid/content/Context;)V");
        if (initializeMethod != nullptr)
        {
            env->CallStaticVoidMethod(helperClass, initializeMethod, context);
        }
    }

    Babylon::Polyfills::RegisterAudioPreparePlaybackHandler([]() {
        RequestAndroidPlaybackFocus();
    });
}
