#include <jni.h>
#include <android/log.h>
#include <AndroidExtensions/Globals.h>
#include <AndroidExtensions/JavaWrappers.h>
#include <android/asset_manager_jni.h>
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

    jclass contextClass = env->GetObjectClass(context);
    jmethodID getApplicationContext = env->GetMethodID(contextClass, "getApplicationContext", "()Landroid/content/Context;");
    jobject applicationContext = env->CallObjectMethod(context, getApplicationContext);
    env->DeleteLocalRef(contextClass);

    jclass appContextClass = env->GetObjectClass(applicationContext);
    jmethodID getAssets = env->GetMethodID(appContextClass, "getAssets", "()Landroid/content/res/AssetManager;");
    jobject assetManagerObj = env->CallObjectMethod(applicationContext, getAssets);
    env->DeleteLocalRef(appContextClass);

    android::global::Initialize(javaVM, applicationContext, assetManagerObj);

#if defined(NODE_API_AVAILABLE_NATIVE_TESTS)
    // Wire the in-process Node-API test harness to a native AssetManager and a writable base dir
    // derived from the (still-valid) instrumentation Context, so it does not fall back to
    // android::global::GetAppContext() during the run -- that global ref is not valid here and
    // dereferencing it aborts with "use of deleted global reference".
    if (assetManagerObj != nullptr)
    {
        AAssetManager* nativeAssetManager = AAssetManager_fromJava(env, assetManagerObj);

        jclass ctxClass = env->GetObjectClass(context);
        jmethodID getFilesDir = env->GetMethodID(ctxClass, "getFilesDir", "()Ljava/io/File;");
        jobject filesDir = env->CallObjectMethod(context, getFilesDir);
        jclass fileClass = env->GetObjectClass(filesDir);
        jmethodID getAbsolutePath = env->GetMethodID(fileClass, "getAbsolutePath", "()Ljava/lang/String;");
        auto pathString = static_cast<jstring>(env->CallObjectMethod(filesDir, getAbsolutePath));
        const char* rawPath = env->GetStringUTFChars(pathString, nullptr);
        std::filesystem::path baseDir = std::filesystem::path{rawPath} / "node_api_tests";
        env->ReleaseStringUTFChars(pathString, rawPath);
        env->DeleteLocalRef(pathString);
        env->DeleteLocalRef(fileClass);
        env->DeleteLocalRef(filesDir);
        env->DeleteLocalRef(ctxClass);

        SetNodeApiTestEnvironment(nativeAssetManager, baseDir);
    }
#endif

    if (assetManagerObj != nullptr)
    {
        env->DeleteLocalRef(assetManagerObj);
    }

    env->DeleteLocalRef(applicationContext);

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
