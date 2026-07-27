#pragma once

#include <jni.h>

namespace Babylon::Embedding::Android
{
    void RegisterWebAudioPlatform(JNIEnv* env, jobject context, JavaVM* javaVM);
}
