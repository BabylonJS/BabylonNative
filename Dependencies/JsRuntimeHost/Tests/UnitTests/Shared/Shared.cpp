#include "Shared.h"
#include <Babylon/AppRuntime.h>
#include <Babylon/ScriptLoader.h>
#include <Babylon/Polyfills/AbortController.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/Performance.h>
#include <Babylon/Polyfills/Scheduling.h>
#include <Babylon/Polyfills/URL.h>
#include <Babylon/Polyfills/WebSocket.h>
#include <Babylon/Polyfills/XMLHttpRequest.h>
#include <Babylon/Polyfills/Blob.h>
#include <Babylon/Polyfills/TextDecoder.h>
#include <gtest/gtest.h>
#include <arcana/threading/blocking_concurrent_queue.h>
#include <atomic>
#include <chrono>
#include <future>
#include <iostream>
#include <thread>

namespace
{
    const char* EnumToString(Babylon::Polyfills::Console::LogLevel logLevel)
    {
        switch (logLevel)
        {
            case Babylon::Polyfills::Console::LogLevel::Log:
                return "log";
            case Babylon::Polyfills::Console::LogLevel::Warn:
                return "warn";
            case Babylon::Polyfills::Console::LogLevel::Error:
                return "error";
        }

        return "unknown";
    }
}

TEST(JavaScript, All)
{
    // Change this to true to wait for the JavaScript debugger to attach (only applies to V8)
    constexpr const bool waitForDebugger = false;

    std::promise<int32_t> exitCodePromise;

    Babylon::AppRuntime::Options options{};

    options.UnhandledExceptionHandler = [&exitCodePromise](const Napi::Error& error) {
        std::cerr << "[Uncaught Error] " << Napi::GetErrorString(error) << std::endl;
        std::cerr.flush();

        exitCodePromise.set_value(-1);
    };

    if (waitForDebugger)
    {
        std::cout << "Waiting for debugger..." << std::endl;
        options.WaitForDebugger = true;
    }

    Babylon::AppRuntime runtime{options};

    runtime.Dispatch([&exitCodePromise](Napi::Env env) mutable {
        Babylon::Polyfills::Console::Initialize(env, [](const char* message, Babylon::Polyfills::Console::LogLevel logLevel) {
            std::cout << "[" << EnumToString(logLevel) << "] " << message << std::endl;
            std::cout.flush();
        });

        Babylon::Polyfills::AbortController::Initialize(env);
        Babylon::Polyfills::Performance::Initialize(env);
        Babylon::Polyfills::Scheduling::Initialize(env);
        Babylon::Polyfills::URL::Initialize(env);
        Babylon::Polyfills::WebSocket::Initialize(env);
        Babylon::Polyfills::XMLHttpRequest::Initialize(env);
        Babylon::Polyfills::Blob::Initialize(env);
        Babylon::Polyfills::TextDecoder::Initialize(env);

        auto setExitCodeCallback = Napi::Function::New(
            env, [&exitCodePromise](const Napi::CallbackInfo& info) {
                Napi::Env env = info.Env();
                exitCodePromise.set_value(info[0].As<Napi::Number>().Int32Value());
            },
            "setExitCode");
        env.Global().Set("setExitCode", setExitCodeCallback);

        env.Global().Set("hostPlatform", Napi::Value::From(env, JSRUNTIMEHOST_PLATFORM));

#ifdef JSRUNTIMEHOST_SANITIZERS_ENABLED
        env.Global().Set("sanitizersEnabled", Napi::Value::From(env, true));
#else
        env.Global().Set("sanitizersEnabled", Napi::Value::From(env, false));
#endif
    });

    Babylon::ScriptLoader loader{runtime};
    loader.Eval("location = { href: '' };", ""); // Required for Mocha.js as we do not have a location
    loader.LoadScript("app:///Scripts/tests.js");

    auto exitCode{exitCodePromise.get_future().get()};

    EXPECT_EQ(exitCode, 0);
}

TEST(Console, Log)
{
    Babylon::AppRuntime runtime{};

    runtime.Dispatch([](Napi::Env env) mutable {
        Babylon::Polyfills::Console::Initialize(env, [](const char* message, Babylon::Polyfills::Console::LogLevel logLevel) {
            const char* test = "foo bar";
            if (strcmp(message, test) != 0)
            {
                std::cout << "Expected: " << test << std::endl;
                std::cout << "Received: " << message << std::endl;
                std::cout.flush();
                ADD_FAILURE();
            }
        });
    });

    std::promise<void> done;

    Babylon::ScriptLoader loader{runtime};
    loader.Eval("console.log('foo', 'bar')", "");
    loader.Dispatch([&done](auto) {
        done.set_value();
    });

    done.get_future().get();
}

TEST(AppRuntime, DestroyDoesNotDeadlock)
{
    // Deterministic test for the race condition in the AppRuntime destructor.
    //
    // A global hook sleeps WHILE HOLDING the queue mutex, right before
    // condition_variable::wait(). We synchronize so the worker is definitely
    // in the hook before triggering destruction.
    //
    // Old (broken) code: cancel() + notify_all() fire without the mutex,
    //   so the notification is lost while the worker sleeps → deadlock.
    // Fixed code: cancel() + Append(no-op), where push() NEEDS the mutex,
    //   so it blocks until the worker enters wait() → notification delivered.

    // Shared state for hook synchronization
    std::atomic<bool> hookEnabled{false};
    std::atomic<bool> hookSignaled{false};
    std::promise<void> workerInHook;

    // Set the callback. It checks hookEnabled so we control
    // when it actually sleeps.
    arcana::set_before_wait_callback([&]() {
        if (hookEnabled.load() && !hookSignaled.exchange(true))
        {
            workerInHook.set_value();
        }
        if (hookEnabled.load())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
    });

    auto runtime = std::make_unique<Babylon::AppRuntime>();

    // Dispatch work and wait for completion
    std::promise<void> ready;
    runtime->Dispatch([&ready](Napi::Env) {
        ready.set_value();
    });
    ready.get_future().wait();

    // Enable the hook and dispatch a no-op to wake the worker,
    // ensuring it cycles through the hook on its way back to idle
    hookEnabled.store(true);
    runtime->Dispatch([](Napi::Env) {});

    // Wait for the worker to be in the hook (holding mutex, sleeping)
    auto hookStatus = workerInHook.get_future().wait_for(std::chrono::seconds(5));
    if (hookStatus == std::future_status::timeout)
    {
        // Hook didn't fire — no deadlock risk, clean up normally
        arcana::set_before_wait_callback([]() {});
        FAIL() << "Worker thread did not enter before-wait hook";
    }

    // Worker is in the hook (holding mutex, sleeping). Destroy on a
    // detachable thread so the test doesn't hang if the destructor deadlocks.
    auto runtimePtr = std::make_shared<std::unique_ptr<Babylon::AppRuntime>>(std::move(runtime));
    std::promise<void> destroyDone;
    auto destroyFuture = destroyDone.get_future();
    std::thread destroyThread([runtimePtr, &destroyDone]() {
        runtimePtr->reset();
        destroyDone.set_value();
    });

    auto status = destroyFuture.wait_for(std::chrono::seconds(5));
    if (status == std::future_status::timeout)
    {
        destroyThread.detach();
    }
    else
    {
        destroyThread.join();
    }

    arcana::set_before_wait_callback([]() {});

    ASSERT_NE(status, std::future_status::timeout)
        << "Deadlock detected: AppRuntime destructor did not complete within 5 seconds";
}

int RunTests()
{
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
