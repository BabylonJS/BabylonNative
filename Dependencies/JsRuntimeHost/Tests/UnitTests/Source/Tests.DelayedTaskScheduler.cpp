#include "DelayedTaskScheduler.h"

#include <Babylon/AppRuntime.h>
#include <Babylon/Polyfills/Scheduling.h>

#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <future>
#include <limits>
#include <memory>
#include <stdexcept>
#include <thread>

using namespace std::chrono_literals;

namespace Babylon::Internal
{
    struct DelayedTaskSchedulerTestAccess
    {
        static DelayedTaskScheduler Create(DelayedTaskScheduler::Id lastId)
        {
            return DelayedTaskScheduler{lastId};
        }
    };
}

using Scheduler = Babylon::Internal::DelayedTaskScheduler;

TEST(DelayedTaskScheduler, IdsStartAtOne)
{
    Scheduler scheduler;
    EXPECT_EQ(scheduler.Schedule(1h, [] {}), 1);
    EXPECT_EQ(scheduler.Schedule(1h, [] {}), 2);
}

TEST(DelayedTaskScheduler, IdsWrapBeforeSignedOverflow)
{
    constexpr auto MaxId = std::numeric_limits<Scheduler::Id>::max();
    auto scheduler = Babylon::Internal::DelayedTaskSchedulerTestAccess::Create(MaxId - 2);
    EXPECT_EQ(scheduler.Schedule(1h, [] {}), MaxId - 1);
    EXPECT_EQ(scheduler.Schedule(1h, [] {}), MaxId);
    EXPECT_EQ(scheduler.Schedule(1h, [] {}), 1);
    EXPECT_EQ(scheduler.Schedule(1h, [] {}), 2);
}

TEST(DelayedTaskScheduler, RunsAtOrAfterRequestedTime)
{
    Scheduler scheduler;
    std::promise<Scheduler::TimePoint> ranPromise;
    const auto requestedTime = std::chrono::time_point_cast<std::chrono::microseconds, std::chrono::steady_clock>(
        std::chrono::steady_clock::now() + 10ms);

    scheduler.Schedule(requestedTime, [&ranPromise]() {
        ranPromise.set_value(std::chrono::time_point_cast<std::chrono::microseconds, std::chrono::steady_clock>(
            std::chrono::steady_clock::now()));
    });

    auto ranFuture = ranPromise.get_future();
    ASSERT_EQ(ranFuture.wait_for(5s), std::future_status::ready);
    EXPECT_GE(ranFuture.get(), requestedTime);
}

TEST(DelayedTaskScheduler, CancelDoesNotWaitForExtractedCallback)
{
    Scheduler scheduler;
    std::promise<void> enteredPromise;
    std::promise<void> releasePromise;
    auto release = releasePromise.get_future().share();

    const auto id = scheduler.Schedule(0ms, [&enteredPromise, release]() {
        enteredPromise.set_value();
        release.wait();
    });

    ASSERT_EQ(enteredPromise.get_future().wait_for(5s), std::future_status::ready);
    const auto cancelStarted = std::chrono::steady_clock::now();
    scheduler.Cancel(id);
    EXPECT_LT(std::chrono::steady_clock::now() - cancelStarted, 1s);
    releasePromise.set_value();
}

TEST(DelayedTaskScheduler, CancelRemovesQueuedCallback)
{
    Scheduler scheduler;
    std::promise<void> calledPromise;
    auto calledFuture = calledPromise.get_future();

    const auto id = scheduler.Schedule(100ms, [&calledPromise]() {
        calledPromise.set_value();
    });
    scheduler.Cancel(id);

    EXPECT_EQ(calledFuture.wait_for(200ms), std::future_status::timeout);
}

TEST(DelayedTaskScheduler, ShutdownWaitsForRunningCallbackAndRejectsNewWork)
{
    Scheduler scheduler;
    std::promise<void> enteredPromise;
    std::promise<void> releasePromise;
    auto release = releasePromise.get_future().share();

    scheduler.Schedule(0ms, [&enteredPromise, release]() {
        enteredPromise.set_value();
        release.wait();
    });
    ASSERT_EQ(enteredPromise.get_future().wait_for(5s), std::future_status::ready);

    auto shutdownFuture = std::async(std::launch::async, [&scheduler]() {
        scheduler.Shutdown();
    });
    EXPECT_EQ(shutdownFuture.wait_for(20ms), std::future_status::timeout);
    releasePromise.set_value();
    EXPECT_EQ(shutdownFuture.wait_for(5s), std::future_status::ready);
    EXPECT_THROW(scheduler.Schedule(0ms, [] {}), std::runtime_error);
}

TEST(DelayedTaskScheduler, AssociationDoesNotDependOnJsRuntimeNativeObject)
{
    Babylon::AppRuntime runtime;
    std::promise<bool> lifecyclePromise;

    runtime.Dispatch([&lifecyclePromise](Napi::Env env) {
        auto* const scheduler = Scheduler::GetFromJavaScript(env);
        if (scheduler == nullptr)
        {
            lifecyclePromise.set_value(false);
            return;
        }

        const auto nativeObject = env.Global().Get("_native");
        env.Global().Set("_native", env.Undefined());
        const bool independentOfJsRuntime = Scheduler::GetFromJavaScript(env) == scheduler;
        Scheduler::ClearFromJavaScript(env);
        const bool cleared = Scheduler::GetFromJavaScript(env) == nullptr;
        Scheduler::SetForJavaScript(env, *scheduler);
        const bool associated = Scheduler::GetFromJavaScript(env) == scheduler;
        env.Global().Set("_native", nativeObject);
        lifecyclePromise.set_value(
            independentOfJsRuntime &&
            cleared &&
            associated);
    });

    auto lifecycleFuture = lifecyclePromise.get_future();
    ASSERT_EQ(lifecycleFuture.wait_for(5s), std::future_status::ready);
    EXPECT_TRUE(lifecycleFuture.get());
}

TEST(SchedulingLifecycle, UsesOwnedSchedulerWithoutRegistration)
{
    Babylon::AppRuntime runtime;
    std::promise<void> timerPromise;

    runtime.Dispatch([&timerPromise](Napi::Env env) {
        Scheduler::ClearFromJavaScript(env);
        Babylon::Polyfills::Scheduling::Initialize(env);
        env.Global().Set(
            "timerComplete",
            Napi::Function::New(env, [&timerPromise](const Napi::CallbackInfo&) {
                timerPromise.set_value();
            }));
        env.Global().Get("setTimeout").As<Napi::Function>().Call(
            env.Global(),
            {env.Global().Get("timerComplete"), Napi::Number::New(env, 1)});
    });

    auto timerFuture = timerPromise.get_future();
    ASSERT_EQ(timerFuture.wait_for(5s), std::future_status::ready);
}

TEST(SchedulingLifecycle, RuntimeShutdownCancelsBorrowedTimers)
{
    std::atomic_bool called{};
    auto destroyFuture = std::async(std::launch::async, [&called]() {
        auto runtime = std::make_unique<Babylon::AppRuntime>();
        std::promise<void> initializedPromise;
        runtime->Dispatch([&called, &initializedPromise](Napi::Env env) {
            Babylon::Polyfills::Scheduling::Initialize(env);
            auto callback = Napi::Function::New(env, [&called](const Napi::CallbackInfo&) {
                called = true;
            });
            env.Global().Get("setTimeout").As<Napi::Function>().Call(
                env.Global(),
                {callback, Napi::Number::New(env, 60000)});
            initializedPromise.set_value();
        });

        if (initializedPromise.get_future().wait_for(5s) != std::future_status::ready)
        {
            throw std::runtime_error{"Scheduling initialization timed out"};
        }
        runtime.reset();
    });

    ASSERT_EQ(destroyFuture.wait_for(5s), std::future_status::ready);
    EXPECT_NO_THROW(destroyFuture.get());
    EXPECT_FALSE(called.load());
}
