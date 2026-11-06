#include <Babylon/AppRuntime.h>
#include <gtest/gtest.h>
#include <arcana/threading/blocking_concurrent_queue.h>
#include <atomic>
#include <chrono>
#include <future>
#include <memory>
#include <thread>

#if defined(JSRUNTIMEHOST_NAPI_ENGINE_V8)
#include <napi/env.h>
#endif

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

#if defined(JSRUNTIMEHOST_NAPI_ENGINE_V8)
TEST(AppRuntime, V8FinalizersDrainAfterDispatch)
{
    constexpr size_t ExternalCount{32};
    std::atomic<size_t> finalized{};
    std::promise<void> created;
    std::promise<void> collectionRequested;
    std::promise<size_t> observed;

    Babylon::AppRuntime runtime{};
    runtime.Dispatch([&finalized, &created](Napi::Env env) {
        for (size_t index{}; index < ExternalCount; ++index)
        {
            Napi::External<std::atomic<size_t>>::New(
                env,
                &finalized,
                [](Napi::Env, std::atomic<size_t>* count) {
                    count->fetch_add(1, std::memory_order_relaxed);
                });
        }
        created.set_value();
    });
    created.get_future().wait();

    runtime.Dispatch([&collectionRequested](Napi::Env env) {
        Napi::GetContext(env)->GetIsolate()->LowMemoryNotification();
        collectionRequested.set_value();
    });
    collectionRequested.get_future().wait();

    runtime.Dispatch([&finalized, &observed](Napi::Env) {
        observed.set_value(finalized.load(std::memory_order_relaxed));
    });

    EXPECT_EQ(observed.get_future().get(), ExternalCount);
}

TEST(AppRuntime, V8FinalizerDrainYieldsBetweenDispatcherTurns)
{
    constexpr size_t ExternalCount{16};
    std::atomic<size_t> finalized{};
    std::promise<void> created;
    std::promise<void> collectionRequested;

    Babylon::AppRuntime runtime{};
    runtime.Dispatch([&](Napi::Env env) {
        for (size_t index{}; index < ExternalCount; ++index)
        {
            Napi::External<std::atomic<size_t>>::New(
                env,
                &finalized,
                [](Napi::Env, std::atomic<size_t>* count) {
                    std::this_thread::sleep_for(std::chrono::milliseconds{2});
                    count->fetch_add(1, std::memory_order_relaxed);
                });
        }
        created.set_value();
    });
    created.get_future().wait();

    runtime.Dispatch([&](Napi::Env env) {
        Napi::GetContext(env)->GetIsolate()->LowMemoryNotification();
        collectionRequested.set_value();
    });
    collectionRequested.get_future().wait();

    const auto observeFinalized = [&]() {
        std::promise<size_t> observed;
        auto future = observed.get_future();
        runtime.Dispatch([&](Napi::Env) {
            observed.set_value(finalized.load(std::memory_order_relaxed));
        });
        return future.get();
    };

    auto observed = observeFinalized();
    EXPECT_GT(observed, 0u);
    EXPECT_LT(observed, ExternalCount);

    for (size_t turn{}; turn < ExternalCount && observed < ExternalCount; ++turn)
    {
        observed = observeFinalized();
    }
    EXPECT_EQ(observed, ExternalCount);
}
#endif
