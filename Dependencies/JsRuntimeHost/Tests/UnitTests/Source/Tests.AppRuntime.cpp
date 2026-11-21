#include <Babylon/AppRuntime.h>
#include <gtest/gtest.h>
#include <arcana/threading/blocking_concurrent_queue.h>
#include <napi/env.h>
#include <chrono>
#include <cstdlib>
#include <future>
#include <memory>
#include <thread>
#include <string>
#include <string_view>
#include <vector>

TEST(AppRuntime, DestroyDoesNotDeadlock)
{
    // Regression test verifying AppRuntime destruction doesn't deadlock.
    // Uses a global arcana hook to sleep while holding the queue mutex
    // before wait(), ensuring the worker is in the vulnerable window
    // when the destructor fires. See #147 for details on the bug and fix.
    //
    // The entire test runs on a separate thread so the gtest thread can
    // detect a deadlock via timeout without hanging the process.
    //
    // Test flow:
    //
    //   Test Thread                    Worker Thread
    //   -----------                    -------------
    //   1. Create AppRuntime           Worker starts, enters blocking_tick
    //      Wait for init to complete
    //   2. Install hook
    //      Dispatch(no-op)             Worker wakes, runs no-op,
    //                                  returns to blocking_tick
    //                                  Hook fires:
    //                                    signal workerInHook
    //                                    sleep 200ms (holding mutex!)
    //   3. workerInHook.wait()
    //      Worker is sleeping in hook
    //   4. ~AppRuntime():
    //          cancel()
    //          Append(no-op):
    //            push() blocks ------> (worker holds mutex)
    //                                  200ms sleep ends
    //                                  wait(lock) releases mutex
    //            push() acquires mutex
    //            pushes, notifies ---> wakes up!
    //            join() waits          drains no-op, cancelled -> exit
    //            join() returns <----- thread exits
    //   5. destroy completes -> PASS

    bool hookSignaled{false};
    std::promise<void> workerInHook;
    std::promise<void> testDone;

    // Run the full lifecycle on a separate thread so the gtest thread
    // can detect a deadlock via timeout.
    std::thread testThread([&]() {
        auto runtime = std::make_unique<Babylon::AppRuntime>();

        // Wait for the runtime to fully initialize. The constructor dispatches
        // CreateForJavaScript which must complete before we install the hook
        // so the worker is idle and ready to enter the hook on the next wait.
        std::promise<void> ready;
        runtime->Dispatch([&ready](Napi::Env) {
            ready.set_value();
        });
        ready.get_future().wait();

        // JavaScriptCore on Apple and Linux waits in the thread's run loop (CFRunLoop or a GLib main
        // context) instead of blocking_tick, so the queue hook never fires there; the destructor
        // must still wake the run loop.
#if defined(__APPLE__) || (defined(__linux__) && !defined(__ANDROID__))
        constexpr bool waitsInDispatcherQueue{std::string_view{NAPI_JAVASCRIPT_ENGINE} != "JavaScriptCore"};
#else
        constexpr bool waitsInDispatcherQueue{true};
#endif
        if (waitsInDispatcherQueue)
        {
            // Install the hook and dispatch a no-op to wake the worker,
            // ensuring it cycles through the hook on its way back to idle.
            arcana::test_hooks::blocking_concurrent_queue::set_before_wait_callback([&]() {
                if (hookSignaled)
                {
                    return;
                }
                hookSignaled = true;
                workerInHook.set_value();
                // This sleep is not truly deterministic. Its purpose is to hold the
                // mutex long enough for runtime.reset() (called by the test thread
                // after workerInHook signals) to reach push() while the mutex is
                // still held. When the sleep ends, the worker enters wait() which
                // releases the mutex, allowing push() to acquire it and deliver the
                // wake-up notification. If runtime.reset() hasn't reached push()
                // by the time the sleep ends, the test still passes but doesn't
                // exercise the intended contention window.
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
            });
            runtime->Dispatch([](Napi::Env) {});

            // Wait for the worker to be in the hook (holding mutex, sleeping)
            workerInHook.get_future().wait();
        }

        // Destroy — if the fix works, the destructor completes.
        // If broken, it deadlocks and the timeout detects it.
        runtime.reset();
        testDone.set_value();
    });

    auto status = testDone.get_future().wait_for(std::chrono::seconds(5));

    arcana::test_hooks::blocking_concurrent_queue::set_before_wait_callback([]() {});

    if (status == std::future_status::timeout)
    {
        testThread.detach();
        FAIL() << "Deadlock detected: AppRuntime destructor did not complete within 5 seconds";
    }

    testThread.join();
}

#if defined(__linux__) && !defined(__ANDROID__)
#include <spawn.h>
#include <sys/wait.h>

extern char** environ;

TEST(AppRuntime, TimerDrivenCollectionAfterFirstRuntimeExits)
{
    // On Linux JavaScriptCore, the full collection timer needs WTF's process-wide main RunLoop,
    // which is set up only once per process. It must outlive the runtime whose thread set it up:
    // destroy a runtime that never collects, then let a second runtime run collection timers.
    // Run in a fresh copy of this executable so this is the first runtime and the first
    // timer-driven collection in that process.
    constexpr const char* childVariable{"JSRUNTIMEHOST_TIMER_COLLECTION_CHILD"};
    if (getenv(childVariable) == nullptr)
    {
        std::vector<std::string> variables{
            std::string{childVariable} + "=1",
            "GTEST_FILTER=AppRuntime.TimerDrivenCollectionAfterFirstRuntimeExits"};
        std::vector<char*> envp;
        for (std::string& variable : variables)
        {
            envp.push_back(variable.data());
        }
        for (char** variable = environ; *variable != nullptr; ++variable)
        {
            if (std::string_view{*variable}.rfind("GTEST_FILTER=", 0) != 0)
            {
                envp.push_back(*variable);
            }
        }
        envp.push_back(nullptr);

        char path[]{"/proc/self/exe"};
        char* argv[]{path, nullptr};
        pid_t pid{};
        ASSERT_EQ(posix_spawn(&pid, path, nullptr, nullptr, argv, envp.data()), 0);
        int status{};
        ASSERT_EQ(waitpid(pid, &status, 0), pid);
        EXPECT_TRUE(WIFEXITED(status) && WEXITSTATUS(status) == 0) << "Child status " << status;
        return;
    }

    {
        Babylon::AppRuntime first{};
        std::promise<void> ready;
        first.Dispatch([&ready](Napi::Env) { ready.set_value(); });
        ready.get_future().wait();
    }

    Babylon::AppRuntime second{};
    const auto end{std::chrono::steady_clock::now() + std::chrono::seconds(3)};
    while (std::chrono::steady_clock::now() < end)
    {
        std::promise<void> allocated;
        second.Dispatch([&allocated](Napi::Env env) {
            Napi::Eval(env, "{ const a = []; for (let i = 0; i < 100000; ++i) a.push({ i }); }", "garbage.js");
            allocated.set_value();
        });
        allocated.get_future().wait();

        // Leave the runtime's dispatch loop idle so it runs pending collection timers.
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
#endif

#if defined(JSRUNTIMEHOST_APPLE_JAVASCRIPTCORE)
TEST(AppRuntime, AsyncWebAssemblyInstantiationMakesProgress)
{
    Babylon::AppRuntime runtime{};
    std::promise<bool> completionPromise;
    auto completionFuture = completionPromise.get_future();

    runtime.Dispatch([&completionPromise](Napi::Env env) {
        env.Global().Set(
            "completeAsyncWebAssemblyTest",
            Napi::Function::New(env, [&completionPromise](const Napi::CallbackInfo& info) {
                completionPromise.set_value(info[0].ToBoolean().Value());
            }));

        Napi::Eval(env, R"(
            (() => {
                const customPayloadSize = 1024 * 1024;
                const bytes = new Uint8Array(13 + customPayloadSize);
                bytes.set([0x00, 0x61, 0x73, 0x6d, 0x01, 0x00, 0x00, 0x00]);
                bytes.set([0x00, 0x81, 0x80, 0x40, 0x00], 8);
                WebAssembly.instantiate(bytes, {}).then(
                    () => completeAsyncWebAssemblyTest(true),
                    () => completeAsyncWebAssemblyTest(false));
            })();
        )",
            "async-webassembly-test.js");
    });

    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds{5};
    while (completionFuture.wait_for(std::chrono::milliseconds{0}) != std::future_status::ready &&
           std::chrono::steady_clock::now() < deadline)
    {
        std::promise<void> pumpPromise;
        auto pumpFuture = pumpPromise.get_future();
        runtime.Dispatch([&pumpPromise](Napi::Env) {
            pumpPromise.set_value();
        });
        ASSERT_EQ(pumpFuture.wait_for(std::chrono::seconds{1}), std::future_status::ready);
        std::this_thread::sleep_for(std::chrono::milliseconds{1});
    }

    ASSERT_EQ(completionFuture.wait_for(std::chrono::milliseconds{0}), std::future_status::ready)
        << "WebAssembly.instantiate did not settle while the host runtime was being pumped";
    EXPECT_TRUE(completionFuture.get());
}
#endif
