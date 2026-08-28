#include <jni.h>
#include <android/log.h>
#include <AndroidExtensions/Globals.h>
#include <AndroidExtensions/JavaWrappers.h>
#include <AndroidExtensions/StdoutLogger.h>
#include <Babylon/DebugTrace.h>
#include <gtest/gtest.h>

extern "C" JNIEXPORT jint JNICALL
Java_com_babylonnative_unittests_Native_javaScriptTests(JNIEnv* env, jclass clazz, jobject context)
{
    JavaVM* javaVM{};
    if (env->GetJavaVM(&javaVM) != JNI_OK)
    {
        throw std::runtime_error{"Failed to get Java VM"};
    }

    jclass webSocketClass{env->FindClass("com/babylonnative/unittests/WebSocket")};
    java::websocket::WebSocketClient::InitializeJavaWebSocketClass(webSocketClass, env);

    android::StdoutLogger::Start();

    android::global::Initialize(javaVM, context);

    Babylon::DebugTrace::EnableDebugTrace(true);
    Babylon::DebugTrace::SetTraceOutput([](const char* trace) { printf("%s\n", trace); fflush(stdout); });

    testing::InitGoogleTest();
    auto testResult = RUN_ALL_TESTS();

    android::StdoutLogger::Stop();

    java::websocket::WebSocketClient::DestructJavaWebSocketClass(env);
    return testResult;
}
