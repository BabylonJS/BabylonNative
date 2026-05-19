// Playground-specific JNI helper. Built into the same .so as the generic
// Integrations/Android JNI (added via target_sources in the Playground
// CMakeLists), so there's a single copy of Babylon::Integrations across
// the process — avoids cross-library handle-passing UB.
//
// Sole purpose: surface Apps/Playground/Shared/PlaygroundScripts.{h,cpp}
// to Java so the bootstrap script list stays in one place, shared with
// the other Playground hosts.

#include <Babylon/Integrations/Runtime.h>
#include <Babylon/Integrations/Android/RuntimeHandle.h>

#include <Shared/PlaygroundScripts.h>

#include <jni.h>

extern "C"
{

JNIEXPORT void JNICALL
Java_com_android_babylonnative_playground_PlaygroundActivity_loadBootstrapScripts(
    JNIEnv*, jclass, jlong runtimeHandle)
{
    auto* runtime = Babylon::Integrations::Android::RuntimeFromHandle(runtimeHandle);
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

} // extern "C"

