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

#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace
{
    constexpr const char* LOG_TAG = "BabylonNative";

    // Parsed by parseCommandLine before the Runtime is created, consumed by
    // the runtime-option getters and loadBootstrapScripts.
    PlaygroundOptions g_options{};

    int LogPriorityFor(Babylon::Embedding::LogLevel level)
    {
        switch (level)
        {
            case Babylon::Embedding::LogLevel::Verbose: return ANDROID_LOG_VERBOSE;
            case Babylon::Embedding::LogLevel::Log:     return ANDROID_LOG_INFO;
            case Babylon::Embedding::LogLevel::Warn:    return ANDROID_LOG_WARN;
            case Babylon::Embedding::LogLevel::Error:   return ANDROID_LOG_ERROR;
            case Babylon::Embedding::LogLevel::Fatal:   return ANDROID_LOG_FATAL;
        }
        return ANDROID_LOG_INFO;
    }

    void LogUsage()
    {
        // Log line by line; logcat truncates long messages.
        std::istringstream usage{CommandLine::Usage("PlaygroundActivity")};
        for (std::string line; std::getline(usage, line);)
        {
            __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "%s", line.c_str());
        }
    }
}

extern "C"
{

// `args` uses the desktop Playground command-line syntax (e.g. --once,
// --test-index=N). Returns -1 to continue, or the exit code the activity
// should finish with: 0 for --help, 2 for a parse error. The exit code is
// logged as "Playground.exit(N)" for logcat-based drivers.
JNIEXPORT jint JNICALL
Java_com_android_babylonnative_playground_PlaygroundActivity_parseCommandLine(
    JNIEnv* env, jclass, jobjectArray args)
{
    std::vector<std::string> argStorage{"PlaygroundActivity"};
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

    g_options = CommandLine::Parse(static_cast<int>(argv.size()), argv.data());

    int exitCode = -1;
    if (g_options.ParseError)
    {
        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "Error: %s", g_options.ErrorMessage.c_str());
        LogUsage();
        exitCode = 2;
    }
    else if (g_options.ShowHelp)
    {
        LogUsage();
        exitCode = 0;
    }

    if (exitCode >= 0)
    {
        __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "Playground.exit(%d)", exitCode);
        g_options = {};
        return exitCode;
    }

    // Route Runtime logging through the shared runner callback so console
    // output feeds the --test-timeout hang watchdog, as on the desktop hosts.
    Babylon::Embedding::Android::SetLogCallback(
        [playgroundLog = Playground::MakeLogCallback({})](Babylon::Embedding::LogLevel level, std::string_view message) {
            // logcat needs a NUL-terminated C string.
            const std::string text{message};
            __android_log_write(LogPriorityFor(level), LOG_TAG, text.c_str());
            playgroundLog(level, message);
        });

    return exitCode;
}

// Runtime options derived from the parsed command line, matching the desktop hosts.
JNIEXPORT jboolean JNICALL
Java_com_android_babylonnative_playground_PlaygroundActivity_enableDebugger(JNIEnv*, jclass)
{
    return g_options.Headless ? JNI_FALSE : JNI_TRUE;
}

JNIEXPORT jboolean JNICALL
Java_com_android_babylonnative_playground_PlaygroundActivity_enableDebugTrace(JNIEnv*, jclass)
{
    return g_options.DebugTrace.value_or(true) ? JNI_TRUE : JNI_FALSE;
}

// Queues the options and bootstrap scripts parsed by parseCommandLine.
// Returns the positional script URLs, which the caller loads after the
// bootstrap scripts.
JNIEXPORT jobjectArray JNICALL
Java_com_android_babylonnative_playground_PlaygroundActivity_loadBootstrapScripts(
    JNIEnv* env, jclass, jlong runtimeHandle)
{
    jclass stringClass = env->FindClass("java/lang/String");

    auto* runtime = Babylon::Embedding::Android::RuntimeFromHandle(runtimeHandle);
    if (runtime == nullptr)
    {
        return env->NewObjectArray(0, stringClass, nullptr);
    }

    // Idempotent process-wide setup (PerfTrace level, etc.).
    Playground::Initialize(g_options);
    Playground::QueueOptions(*runtime, g_options);

    // Queue bootstrap scripts; they run after the first View attach
    // completes engine init on the JS thread.
    Playground::LoadBootstrapScripts(*runtime);

    jobjectArray scripts = env->NewObjectArray(static_cast<jsize>(g_options.Scripts.size()), stringClass, nullptr);
    for (size_t i = 0; i < g_options.Scripts.size(); ++i)
    {
        jstring script = env->NewStringUTF(g_options.Scripts[i].c_str());
        env->SetObjectArrayElement(scripts, static_cast<jsize>(i), script);
        env->DeleteLocalRef(script);
    }
    return scripts;
}

} // extern "C"