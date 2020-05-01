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
    AppStateChangedCallbackTicket AddPauseCallback(std::function<void()>&&);

    void Resume();
    AppStateChangedCallbackTicket AddResumeCallback(std::function<void()>&&);

    using RequestPermissionsResultCallbackTicket = arcana::ticketed_collection<std::function<void(int32_t, const std::vector<std::string>&, const std::vector<int32_t>&)>>::ticket;

    void RequestPermissionsResult(int32_t, const std::vector<std::string>&, const std::vector<int32_t>&);
    RequestPermissionsResultCallbackTicket AddRequestPermissionsResultCallback(std::function<void(int32_t, const std::vector<std::string>&, const std::vector<int32_t>&)>&&);
}
