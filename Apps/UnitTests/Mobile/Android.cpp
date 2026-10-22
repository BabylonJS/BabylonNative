#include <AndroidExtensions/Globals.h>
#include <android/log.h>
#include <android/native_window_jni.h>
#include <gtest/gtest.h>
#include <jni.h>

#include <cstdlib>
#include <filesystem>
#include <memory>
#include <stdexcept>

#ifdef BABYLON_NATIVE_MOBILE_TESTS
#include "../Source/App.h"

std::filesystem::path GetExecutableDirectory()
{
    return std::filesystem::current_path();
}
#endif

namespace
{
    class AndroidTestLogger final : public testing::EmptyTestEventListener
    {
        void OnTestEnd(const testing::TestInfo& test) override
        {
            __android_log_print(ANDROID_LOG_INFO, "NativeUnitTests", "%s.%s: %s",
                test.test_suite_name(), test.name(),
                test.result()->Passed() ? "PASSED" : test.result()->Skipped() ? "SKIPPED" : "FAILED");
        }

        void OnTestPartResult(const testing::TestPartResult& result) override
        {
            if (!result.passed())
            {
                __android_log_print(ANDROID_LOG_ERROR, "NativeUnitTests", "%s:%d: %s",
                    result.file_name() ? result.file_name() : "", result.line_number(), result.message());
            }
        }
    };
}

extern "C" JNIEXPORT jint JNICALL
Java_com_babylonnative_mobiletests_Native_run(JNIEnv* env, jclass, jobject context, jobject surface, jstring directory)
{
    try
    {
        JavaVM* vm{};
        if (env->GetJavaVM(&vm) != JNI_OK)
        {
            throw std::runtime_error{"Cannot obtain Java VM"};
        }
        android::global::Initialize(vm, context);
        const char* path = env->GetStringUTFChars(directory, nullptr);
        if (!path)
        {
            return 1;
        }
        const std::filesystem::path workingDirectory{path};
        env->ReleaseStringUTFChars(directory, path);
        std::filesystem::current_path(workingDirectory);
        setenv("TMPDIR", workingDirectory.string().c_str(), 1);

        auto& listeners = testing::UnitTest::GetInstance()->listeners();
        listeners.Append(new AndroidTestLogger{});
        int argc = 1;
        char name[] = "MobileUnitTests";
        char* argv[]{name, nullptr};
#ifdef BABYLON_NATIVE_MOBILE_TESTS
        std::unique_ptr<ANativeWindow, decltype(&ANativeWindow_release)> window{
            ANativeWindow_fromSurface(env, surface), ANativeWindow_release};
        if (!window)
        {
            throw std::runtime_error{"Cannot obtain native test surface"};
        }
        Babylon::Graphics::Configuration config{};
        config.Window = window.get();
        config.Width = 600;
        config.Height = 400;
        const int result = RunTests(config, argc, argv);
#else
        testing::InitGoogleTest(&argc, argv);
        const int result = RUN_ALL_TESTS();
#endif
        const auto& tests = *testing::UnitTest::GetInstance();
        __android_log_print(ANDROID_LOG_INFO, "NativeUnitTests",
            "Completed: passed=%d skipped=%d failed=%d exit=%d", tests.successful_test_count(),
            tests.skipped_test_count(), tests.failed_test_count(), result);
        return tests.successful_test_count() > 0 ? result : 1;
    }
    catch (const std::exception& error)
    {
        __android_log_print(ANDROID_LOG_ERROR, "NativeUnitTests", "%s", error.what());
        return 1;
    }
}
