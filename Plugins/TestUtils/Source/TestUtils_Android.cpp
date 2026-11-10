#include "TestUtils.h"

#include <AndroidExtensions/Globals.h>
#include <android/log.h>

#include <cstdlib>

namespace Babylon::Plugins::Internal
{
    void TestUtils::Exit(const Napi::CallbackInfo& info)
    {
        auto exitCode = info[0].As<Napi::Number>().Int32Value();

        // Notify the host before terminating.
        InvokeExitCallback(exitCode);

        // An Android activity has no process exit code, so log it for test drivers reading logcat.
        __android_log_print(ANDROID_LOG_INFO, "BabylonNative", "TestUtils.exit(%d)", exitCode);
        std::quick_exit(exitCode);
    }

    void TestUtils::UpdateSize(const Napi::CallbackInfo& info)
    {
        const int32_t width = info[0].As<Napi::Number>().Int32Value();
        const int32_t height = info[1].As<Napi::Number>().Int32Value();

        // The host activity owns the surface; resize it through an optional
        // `void setSurfaceSize(int, int)` method. Hosts without one keep their size.
        // Keep the wrapper alive; it owns the reference that 'activity' borrows.
        const auto currentActivity = android::global::TryGetCurrentActivity();
        if (!currentActivity)
        {
            return;
        }
        jobject activity = *currentActivity;

        JNIEnv* env = android::global::GetEnvForCurrentThread();
        jclass activityClass = env->GetObjectClass(activity);
        jmethodID setSurfaceSize = env->GetMethodID(activityClass, "setSurfaceSize", "(II)V");
        if (setSurfaceSize == nullptr)
        {
            env->ExceptionClear();
        }
        else
        {
            env->CallVoidMethod(activity, setSurfaceSize, width, height);
        }
        env->DeleteLocalRef(activityClass);
    }

    void TestUtils::SetTitle(const Napi::CallbackInfo& /*info*/)
    {
    }

    Napi::Value TestUtils::GetOutputDirectory(const Napi::CallbackInfo& info)
    {
        auto path = "/data/data/com.android.babylonnative.playground/cache";
        return Napi::Value::From(info.Env(), path);
    }
}