#include <jni.h>
#include <android/log.h>
#include <AndroidExtensions/Globals.h>
#include <AndroidExtensions/JavaWrappers.h>
#include "Babylon/DebugTrace.h"
#include "Babylon/StandardStreamLogger.h"
#include <Shared/Shared.h>

extern "C" JNIEXPORT jint JNICALL
Java_com_jsruntimehost_unittests_Native_javaScriptTests(JNIEnv* env, jclass clazz, jobject context) {
    JavaVM* javaVM{};
    if (env->GetJavaVM(&javaVM) != JNI_OK)
    {
        throw std::runtime_error{"Failed to get Java VM"};
    }

    if (!Babylon::StandardStreamLogger::Start())
    {
        __android_log_write(ANDROID_LOG_ERROR, "JsRuntimeHost", "Failed to start standard-stream forwarding.");
        return -1;
    }

    jclass webSocketClass{env->FindClass("com/jsruntimehost/unittests/WebSocket")};
    java::websocket::WebSocketClient::InitializeJavaWebSocketClass(webSocketClass, env);

    android::global::Initialize(javaVM, context);

    Babylon::DebugTrace::EnableDebugTrace(true);
    Babylon::DebugTrace::SetTraceOutput([](const char* trace) { printf("%s\n", trace); fflush(stdout); });

    auto testResult = RunTests();

    const bool loggerStopped = Babylon::StandardStreamLogger::Stop();
        if (!loggerStopped)
        {
            __android_log_write(ANDROID_LOG_ERROR, "JsRuntimeHost",
                "Failed to stop standard-stream forwarding (restore or drain timeout).");
        }

        java::websocket::WebSocketClient::DestructJavaWebSocketClass(env);
        return loggerStopped ? testResult : -1;
    }
