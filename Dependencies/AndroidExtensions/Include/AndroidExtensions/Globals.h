#pragma once

#include <jni.h>
#include "JavaWrappers.h"

namespace android::global
{
    void Initialize(JavaVM* javaVM, jobject appContext, jobject mainActivity);

    JNIEnv* GetEnvForCurrentThread();

    android::content::Context GetAppContext();

    android::app::Activity GetMainActivity();
}
