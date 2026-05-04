// Playground-specific JNI helper.
//
// This file lives alongside the generic Integrations/Android JNI in the
// same shared library (`libBabylonNativeIntegrations.so`) — the
// Playground's CMakeLists adds it via `target_sources(...)`. Keeping
// everything in one .so means a single copy of Babylon::Integrations
// across the whole process; cross-library handle passing UB is avoided.
//
// The only purpose of this helper is to surface
// `Apps/Playground/Shared/PlaygroundScripts.{h,cpp}` to Java so that the
// Babylon.js bootstrap script list stays in one place (shared with the
// other Playground hosts: Win32, iOS, macOS, …) rather than being
// duplicated on the Java side.

#include <Babylon/Integrations/Runtime.h>

#include <Shared/PlaygroundScripts.h>

#include <jni.h>

extern "C"
{

JNIEXPORT void JNICALL
Java_com_library_babylonnative_BabylonView_loadBootstrapScripts(
    JNIEnv*, jclass, jlong runtimeHandle)
{
    if (runtimeHandle == 0)
    {
        return;
    }
    auto* runtime = reinterpret_cast<Babylon::Integrations::Runtime*>(runtimeHandle);

    // Process-wide one-shot Playground setup (PerfTrace level, etc.).
    // Re-calling is idempotent; safe even if multiple BabylonView
    // instances queue bootstrap scripts.
    Playground::Initialize();

    // Queues each Babylon.js bootstrap script (ammo / babylon.max /
    // loaders / materials / gui / meshwriter / serializers) onto the
    // runtime; they run after the first View::Attach completes engine
    // initialization on the JS thread.
    Playground::LoadBootstrapScripts(*runtime);
}

} // extern "C"
