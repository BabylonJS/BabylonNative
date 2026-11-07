// Playground-specific JNI helper. Built into the same .so as the generic
// Embedding/Android JNI (added via target_sources in the Playground
// CMakeLists), so there's a single copy of Babylon::Embedding across
// the process — avoids cross-library handle-passing UB.
//
// Sole purpose: surface Apps/Playground/Shared/{CommandLine,PlaygroundScripts}
// to Java so the command-line options and bootstrap script list stay in one
// place, shared with the other Playground hosts.

#include <Babylon/Embedding/Runtime.h>
#include <Babylon/Embedding/Android/RuntimeHandle.h>

#include <Shared/CommandLine.h>
#include <Shared/PlaygroundScripts.h>

#include <android/log.h>
#include <jni.h>

#include <string>
#include <vector>

extern "C"
{

// `args` uses the desktop Playground command-line syntax (e.g. --once,
// --test-index=N). Returns the positional script URLs, which the caller
// loads after the bootstrap scripts.
JNIEXPORT jobjectArray JNICALL
Java_com_android_babylonnative_playground_PlaygroundActivity_loadBootstrapScripts(
    JNIEnv* env, jclass, jlong runtimeHandle, jobjectArray args)
{
    jclass stringClass = env->FindClass("java/lang/String");

    auto* runtime = Babylon::Embedding::Android::RuntimeFromHandle(runtimeHandle);
    if (runtime == nullptr)
    {
        return env->NewObjectArray(0, stringClass, nullptr);
    }

    std::vector<std::string> argStorage{"Playground"};
    const jsize argCount = args != nullptr ? env->GetArrayLength(args) : 0;
    for (jsize i = 0; i < argCount; ++i)
    {
        auto arg = static_cast<jstring>(env->GetObjectArrayElement(args, i));
        const char* chars = env->GetStringUTFChars(arg, nullptr);
        argStorage.emplace_back(chars);
        env->ReleaseStringUTFChars(arg, chars);
        env->DeleteLocalRef(arg);
    }

    std::vector<const char*> argv;
    for (const auto& arg : argStorage)
    {
        argv.push_back(arg.c_str());
    }

    PlaygroundOptions options = CommandLine::Parse(static_cast<int>(argv.size()), argv.data());
    if (options.ParseError)
    {
        __android_log_print(ANDROID_LOG_ERROR, "BabylonNative", "%s", options.ErrorMessage.c_str());
        options = {};
    }

    // Idempotent process-wide setup (PerfTrace level, etc.).
    Playground::Initialize(options);
    Playground::QueueOptions(*runtime, options);

    // Queue bootstrap scripts; they run after the first View attach
    // completes engine init on the JS thread.
    Playground::LoadBootstrapScripts(*runtime);

    jobjectArray scripts = env->NewObjectArray(static_cast<jsize>(options.Scripts.size()), stringClass, nullptr);
    for (size_t i = 0; i < options.Scripts.size(); ++i)
    {
        jstring script = env->NewStringUTF(options.Scripts[i].c_str());
        env->SetObjectArrayElement(scripts, static_cast<jsize>(i), script);
        env->DeleteLocalRef(script);
    }
    return scripts;
}

} // extern "C"
