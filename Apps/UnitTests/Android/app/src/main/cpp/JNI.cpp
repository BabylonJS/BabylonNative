#include "gtest/gtest.h"
#include <jni.h>
#include <Android/log.h>
#include <AndroidExtensions/Globals.h>
#include <AndroidExtensions/JavaWrappers.h>
#include <AndroidExtensions/StdoutLogger.h>
#include <atomic>
#include <Shared/Tests.h>

extern "C" JNIEXPORT jint JNICALL
Java_com_babylonnative_unittests_Native_javaScriptTests(JNIEnv* env, jclass clazz, jobject context) {
    JavaVM* javaVM{};
    if (env->GetJavaVM(&javaVM) != JNI_OK)
    {
        throw std::runtime_error{"Failed to get Java VM"};
    }

    android::StdoutLogger::Start();

    android::global::Initialize(javaVM, context);
    auto testResult = Run();

    android::StdoutLogger::Stop();
    return testResult;
}
