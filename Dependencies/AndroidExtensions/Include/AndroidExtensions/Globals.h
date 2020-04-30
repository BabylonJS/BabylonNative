#pragma once

#include <jni.h>
#include <arcana/containers/ticketed_collection.h>
#include "JavaWrappers.h"

namespace android::global
{
    void Initialize(JavaVM* javaVM, jobject appContext);

    JNIEnv* GetEnvForCurrentThread();

    android::content::Context GetAppContext();


    using AppStateChangedCallbackTicket = arcana::ticketed_collection<std::function<void()>>::ticket;

    void Pause();
    AppStateChangedCallbackTicket AddPausedCallback(std::function<void()>&&);

    void Resume();
    AppStateChangedCallbackTicket AddResumedCallback(std::function<void()>&&);
}
