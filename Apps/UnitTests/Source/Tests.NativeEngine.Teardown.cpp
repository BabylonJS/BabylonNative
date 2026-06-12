#include <gtest/gtest.h>

#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Plugins/NativeEngine.h>

#include <chrono>
#include <future>
#include <iostream>
#include <thread>

extern Babylon::Graphics::Configuration g_deviceConfig;

namespace Babylon::Plugins
{
    // Test-only accessors defined in NativeEngine.cpp (compiled in when BABYLON_NATIVE_BUILD_APPS
    // is set). The native engine exposes a JS method "_disposeDrainTestSchedule" that schedules a
    // tracked threadpool task which sets "started", sleeps, then sets "finished".
    bool NativeEngineDisposeDrainTestTaskStarted();
    bool NativeEngineDisposeDrainTestTaskFinished();
}

// Regression test for the loadTexture async-teardown race. NativeEngine::Dispose must drain
// in-flight threadpool tasks (created via TrackAsyncTask, the same mechanism the async texture
// and shader loaders use) before teardown frees the resources those tasks touch.
//
// The native engine's test-only "_disposeDrainTestSchedule" schedules such a tracked task that
// signals start, sleeps, then signals finish. We let it get in flight, dispose the engine, and
// then check whether it had finished by the time Dispose returned:
//   * With the drain, Dispose blocks until the task finishes      -> finished == true.
//   * Without it, Dispose returns while the task is still running -> finished == false.
// The task touches no graphics resources, so the test is deterministic and never relies on
// undefined behaviour.
TEST(NativeEngineTeardown, DisposeDrainsInFlightAsyncTask)
{
    Babylon::Graphics::Device device{g_deviceConfig};
    device.StartRenderingCurrentFrame();

    Babylon::AppRuntime::Options options{};
    options.UnhandledExceptionHandler = [](const Napi::Error& error) {
        std::cerr << "[Uncaught Error] " << Napi::GetErrorString(error) << std::endl;
        std::quick_exit(1);
    };

    Babylon::AppRuntime runtime{options};

    // Create a native engine and schedule the in-flight tracked task.
    runtime.Dispatch([&device](Napi::Env env) {
        device.AddToJavaScript(env);
        Babylon::Plugins::NativeEngine::Initialize(env);

        auto nativeObject = env.Global().Get("_native").As<Napi::Object>();
        auto engine = nativeObject.Get("Engine").As<Napi::Function>().New({});
        env.Global().Set("_testEngine", engine);
        engine.Get("_disposeDrainTestSchedule").As<Napi::Function>().Call(engine, {});
    });

    // Wait until the task is actually in flight (running on the threadpool).
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(30);
    while (!Babylon::Plugins::NativeEngineDisposeDrainTestTaskStarted() && std::chrono::steady_clock::now() < deadline)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    ASSERT_TRUE(Babylon::Plugins::NativeEngineDisposeDrainTestTaskStarted())
        << "async task never started; cannot exercise the teardown drain";
    ASSERT_FALSE(Babylon::Plugins::NativeEngineDisposeDrainTestTaskFinished())
        << "async task finished before dispose; increase the task delay";

    // Dispose the engine on the JS thread while the task is in flight. With the drain in place,
    // dispose() blocks until the task completes before returning.
    std::promise<void> disposed;
    runtime.Dispatch([&disposed](Napi::Env env) {
        auto engine = env.Global().Get("_testEngine").As<Napi::Object>();
        engine.Get("dispose").As<Napi::Function>().Call(engine, {});
        disposed.set_value();
    });
    disposed.get_future().wait();

    // If Dispose drained the in-flight task, it has finished by the time dispose() returned.
    EXPECT_TRUE(Babylon::Plugins::NativeEngineDisposeDrainTestTaskFinished())
        << "NativeEngine::Dispose returned while an async task was still in flight (drain missing)";

    device.FinishRenderingCurrentFrame();
}
