// Playground-specific JNI helper. Built into the same .so as the generic
// Embedding/Android JNI (added via target_sources in the Playground
// CMakeLists), so there's a single copy of Babylon::Embedding across
// the process — avoids cross-library handle-passing UB.
//
#include <Babylon/Embedding/Runtime.h>
#include <Babylon/Embedding/View.h>
#include <Babylon/Embedding/Android/RuntimeHandle.h>

#include <Shared/PlaygroundScripts.h>

#include <jni.h>
#include <napi/napi.h>

extern "C"
{

JNIEXPORT void JNICALL
Java_com_android_babylonnative_playground_PlaygroundActivity_loadBootstrapScripts(
    JNIEnv*, jclass, jlong runtimeHandle)
{
    auto* runtime = Babylon::Embedding::Android::RuntimeFromHandle(runtimeHandle);
    if (runtime == nullptr)
    {
        return;
    }

    // Idempotent process-wide setup (PerfTrace level, etc.).
    Playground::Initialize();

    // Queue bootstrap scripts; they run after the first View attach
    // completes engine init on the JS thread.
    Playground::LoadBootstrapScripts(*runtime);
}

#if BABYLON_NATIVE_PLAYGROUND_MOBILE_TESTS
JNIEXPORT void JNICALL
Java_com_library_babylonnative_BabylonView_resizeTestSurface(
    JNIEnv*, jclass, jlong viewHandle, jint width, jint height)
{
    auto* view = reinterpret_cast<Babylon::Embedding::View*>(viewHandle);
    view->Resize(width, height, Babylon::Embedding::CoordinateUnits::Logical);
}

JNIEXPORT void JNICALL
Java_com_android_babylonnative_playground_PlaygroundActivity_loadMobileTestScripts(
    JNIEnv* env, jclass, jlong runtimeHandle, jstring endpoint)
{
    auto* runtime = Babylon::Embedding::Android::RuntimeFromHandle(runtimeHandle);
    const char* value = env->GetStringUTFChars(endpoint, nullptr);
    if (value == nullptr)
    {
        return;
    }
    const std::string url{value};
    env->ReleaseStringUTFChars(endpoint, value);
    runtime->RunOnJsThread([url](Napi::Env jsEnv) {
        jsEnv.Global().Set("__mobilePlaywrightUrl", url);
    }, true);
    runtime->LoadScript("app:///Scripts/mobilePlaywright.js");
}
#endif

} // extern "C"
