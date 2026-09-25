#include "V8ForegroundTaskRunner.h"

#include <gtest/gtest.h>

#include <chrono>
#include <future>
#include <vector>

namespace
{
    using Runner = Babylon::Internal::V8ForegroundTaskRunner;
    using Scheduler = Babylon::Internal::DelayedTaskScheduler;
    using namespace std::chrono_literals;

    class Task final : public v8::Task
    {
    public:
        void Run() override {}
    };
}

TEST(V8ForegroundTaskRunner, RoundsAbsoluteTimeUpWithoutTruncatingDelay)
{
    const auto now = std::chrono::steady_clock::time_point{std::chrono::duration_cast<std::chrono::steady_clock::duration>(1234567100ns)};
    EXPECT_EQ(Runner::GetScheduledTime(now, 0.0005), Scheduler::TimePoint{1235068us});
    EXPECT_EQ(Runner::GetScheduledTime(now, 0.0000005), Scheduler::TimePoint{1234568us});
    EXPECT_EQ(Runner::GetScheduledTime(now, 0), Scheduler::TimePoint{1234568us});

    for (const auto delay : {0.0000005, 0.0005, 0.0015})
    {
        Scheduler::TimePoint scheduled;
        Runner runner{
            [](auto) {},
            [&scheduled](auto when, auto) {
                scheduled = when;
                return 1;
            },
            [](auto) {}};
        const auto before = std::chrono::steady_clock::now();
        runner.PostDelayedTask(std::make_unique<Task>(), delay);
        const auto after = std::chrono::steady_clock::now();
        EXPECT_GE(scheduled, before + std::chrono::duration<double>{delay});
        EXPECT_LT(scheduled, after + std::chrono::duration<double>{delay} + 1us);
    }
}

TEST(V8ForegroundTaskRunner, NonpositiveDelaysRemainImmediatelyEligible)
{
    for (const auto delay : {0.0, -0.0005, -1.0})
    {
        Scheduler::TimePoint scheduled;
        Runner runner{
            [](auto) {},
            [&scheduled](auto when, auto) {
                scheduled = when;
                return 1;
            },
            [](auto) {}};
        const auto before = std::chrono::steady_clock::now();
        runner.PostDelayedTask(std::make_unique<Task>(), delay);
        const auto after = std::chrono::steady_clock::now();
        EXPECT_GE(scheduled, before);
        EXPECT_LT(scheduled, after + 1us);
    }
}

TEST(V8ForegroundTaskRunner, FractionalMillisecondTaskDoesNotDispatchEarly)
{
    Scheduler scheduler;
    std::promise<std::chrono::steady_clock::time_point> dispatched;
    Runner runner{
        [&dispatched](auto) { dispatched.set_value(std::chrono::steady_clock::now()); },
        [&scheduler](auto when, auto callback) { return scheduler.Schedule(when, std::move(callback)); },
        [&scheduler](auto id) { scheduler.Cancel(id); }};

    const auto before = std::chrono::steady_clock::now();
    runner.PostNonNestableDelayedTask(std::make_unique<Task>(), 0.0005);
    auto result = dispatched.get_future();
    ASSERT_EQ(result.wait_for(5s), std::future_status::ready);
    EXPECT_GE(result.get(), before + 500us);
}

TEST(V8ForegroundTaskRunner, CompletionBeforeScheduleReturnsDoesNotRetainId)
{
    size_t dispatched{};
    size_t cancelled{};
    {
        Runner runner{
            [&dispatched](auto) { ++dispatched; },
            [](auto, auto callback) {
                callback();
                return 1;
            },
            [&cancelled](auto) { ++cancelled; }};
        for (size_t index = 0; index < 1000; ++index)
        {
            runner.PostDelayedTask(std::make_unique<Task>(), 0);
        }
    }
    EXPECT_EQ(dispatched, 1000);
    EXPECT_EQ(cancelled, 0);
}

TEST(V8ForegroundTaskRunner, CompletionRemovesOnlyItsOwnPendingRecord)
{
    std::vector<Scheduler::Callback> callbacks;
    std::vector<Scheduler::Id> cancelled;
    Scheduler::Id nextId{};
    {
        Runner runner{
            [](auto) {},
            [&callbacks, &nextId](auto, auto callback) {
                callbacks.push_back(std::move(callback));
                return ++nextId;
            },
            [&cancelled](auto id) { cancelled.push_back(id); }};
        for (size_t index = 0; index < 1000; ++index)
        {
            runner.PostDelayedTask(std::make_unique<Task>(), 0);
            auto callback = std::move(callbacks.back());
            callback();
        }
        runner.PostDelayedTask(std::make_unique<Task>(), 60);
    }
    EXPECT_EQ(cancelled, (std::vector<Scheduler::Id>{1001}));
}

TEST(V8ForegroundTaskRunner, ExtractedCallbackDoesNotDependOnRunnerLifetime)
{
    Scheduler::Callback extracted;
    size_t dispatched{};
    size_t cancelled{};
    {
        Runner runner{
            [&dispatched](auto) { ++dispatched; },
            [&extracted](auto, auto callback) {
                extracted = std::move(callback);
                return 1;
            },
            [&cancelled](auto) { ++cancelled; }};
        runner.PostDelayedTask(std::make_unique<Task>(), 0);
    }
    ASSERT_TRUE(extracted);
    extracted();
    EXPECT_EQ(dispatched, 0);
    EXPECT_EQ(cancelled, 1);
}

TEST(V8ForegroundTaskRunner, RetainedRunnerRejectsPostsAfterShutdown)
{
    size_t dispatched{};
    size_t scheduled{};
    Runner runner{
        [&dispatched](auto) { ++dispatched; },
        [&scheduled](auto, auto) {
            ++scheduled;
            return 1;
        },
        [](auto) {}};
    runner.PostTask(std::make_unique<Task>());
    runner.PostNonNestableTask(std::make_unique<Task>());
    runner.Shutdown();
    runner.Shutdown();
    runner.PostTask(std::make_unique<Task>());
    runner.PostNonNestableTask(std::make_unique<Task>());
    runner.PostDelayedTask(std::make_unique<Task>(), 0);
    runner.PostNonNestableDelayedTask(std::make_unique<Task>(), 0);
    EXPECT_EQ(dispatched, 2);
    EXPECT_EQ(scheduled, 0);
}

TEST(V8ForegroundTaskRunner, ShutdownWaitsForInFlightDispatch)
{
    Scheduler::Callback callback;
    std::promise<void> dispatchStarted;
    std::promise<void> releaseDispatch;
    auto release = releaseDispatch.get_future();
    Runner runner{
        [&dispatchStarted, &release](auto) {
            dispatchStarted.set_value();
            release.wait();
        },
        [&callback](auto, auto work) {
            callback = std::move(work);
            return 1;
        },
        [](auto) {}};
    runner.PostDelayedTask(std::make_unique<Task>(), 0);
    auto worker = std::async(std::launch::async, [&callback]() { callback(); });
    dispatchStarted.get_future().wait();
    std::promise<void> shutdownStarted;
    auto shutdown = std::async(std::launch::async, [&runner, &shutdownStarted]() {
        shutdownStarted.set_value();
        runner.Shutdown();
    });
    shutdownStarted.get_future().wait();
    EXPECT_EQ(shutdown.wait_for(20ms), std::future_status::timeout);
    releaseDispatch.set_value();
    EXPECT_EQ(worker.wait_for(5s), std::future_status::ready);
    EXPECT_EQ(shutdown.wait_for(5s), std::future_status::ready);
}
