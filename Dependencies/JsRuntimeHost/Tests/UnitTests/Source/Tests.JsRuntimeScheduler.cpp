#include <Babylon/AppRuntime.h>
#include <Babylon/JsRuntimeScheduler.h>
#include "DelayedTaskScheduler.h"

#include <arcana/threading/task.h>
#include <gsl/util>
#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <functional>
#include <future>
#include <memory>
#include <stdexcept>
#include <thread>

using namespace std::chrono_literals;

namespace
{
    template<typename T>
    T GetWithTimeout(std::future<T>& future)
    {
        if (future.wait_for(5s) != std::future_status::ready)
        {
            throw std::runtime_error{"Runtime callback timed out"};
        }
        return future.get();
    }

    // Include destruction in the deadline. A deadlocked worker may be detached,
    // so every test and runtime callback owns its state, never a caller's stack.
    template<typename CallbackT>
    void RunWithTimeout(CallbackT callback)
    {
        auto completed = std::make_shared<std::promise<void>>();
        auto future = completed->get_future();
        std::thread worker{[callback = std::move(callback), completed]() {
            try
            {
                callback();
                completed->set_value();
            }
            catch (...)
            {
                completed->set_exception(std::current_exception());
            }
        }};

        if (future.wait_for(15s) != std::future_status::ready)
        {
            worker.detach();
            FAIL() << "Runtime lifecycle did not finish within 15 seconds";
        }
        worker.join();
        EXPECT_NO_THROW(future.get());
    }

    Babylon::JsRuntimeScheduler GetScheduler(Babylon::AppRuntime& runtime)
    {
        auto ready = std::make_shared<std::promise<Babylon::JsRuntimeScheduler>>();
        auto future = ready->get_future();
        runtime.Dispatch([ready](Napi::Env env) {
            ready->set_value(Babylon::JsRuntimeScheduler{Babylon::JsRuntime::GetFromJavaScript(env)});
        });
        return GetWithTimeout(future);
    }

    struct OnDestruction
    {
        std::function<void()> Callback;
        ~OnDestruction()
        {
            Callback();
        }
    };
}

TEST(JsRuntimeScheduler, CopiesCanOutliveRuntime)
{
    RunWithTimeout([]() {
        auto runtime = std::make_unique<Babylon::AppRuntime>();
        const auto scheduler = GetScheduler(*runtime);
        auto copy = scheduler;
        auto moved = std::move(copy);
        runtime.reset();

        auto called = std::make_shared<std::atomic<bool>>(false);
        scheduler([called]() { *called = true; });
        moved([called]() { *called = true; });
        if (called->load())
        {
            throw std::runtime_error{"Callback ran after runtime destruction"};
        }
    });
}

TEST(JsRuntimeScheduler, AcceptsMutableZeroArgumentLvalue)
{
    RunWithTimeout([]() {
        Babylon::AppRuntime runtime;
        const auto scheduler = GetScheduler(runtime);
        auto completed = std::make_shared<std::promise<int>>();
        auto future = completed->get_future();
        auto callable = [completed, count = 0]() mutable { completed->set_value(++count); };
        scheduler(callable);
        if (GetWithTimeout(future) != 1)
        {
            throw std::runtime_error{"Mutable callable was not invoked"};
        }
    });
}

TEST(JsRuntimeScheduler, ForwardsEnvironmentAndAllowsNestedDispatch)
{
    RunWithTimeout([]() {
        Babylon::AppRuntime runtime;
        auto completed = std::make_shared<std::promise<bool>>();
        auto future = completed->get_future();
        runtime.Dispatch([completed](Napi::Env env) {
            const Babylon::JsRuntimeScheduler scheduler{Babylon::JsRuntime::GetFromJavaScript(env)};
            scheduler([completed, scheduler, expected = static_cast<napi_env>(env), count = 0](Napi::Env callbackEnv) mutable {
                const bool correct = static_cast<napi_env>(callbackEnv) == expected && ++count == 1;
                scheduler([completed, correct]() { completed->set_value(correct); });
            });
        });
        if (!GetWithTimeout(future))
        {
            throw std::runtime_error{"Scheduler forwarded the wrong environment"};
        }
    });
}

TEST(JsRuntimeScheduler, PrefersExistingZeroArgumentOverload)
{
    struct Callable
    {
        std::shared_ptr<std::promise<bool>> Completed;
        void operator()() const { Completed->set_value(true); }
        void operator()(Napi::Env) const { Completed->set_value(false); }
    };

    RunWithTimeout([]() {
        Babylon::AppRuntime runtime;
        auto completed = std::make_shared<std::promise<bool>>();
        auto future = completed->get_future();
        const Callable callable{completed};
        GetScheduler(runtime)(callable);
        if (!GetWithTimeout(future))
        {
            throw std::runtime_error{"Existing zero-argument overload was bypassed"};
        }
    });
}

TEST(JsRuntimeScheduler, RunsArcanaContinuation)
{
    RunWithTimeout([]() {
        Babylon::AppRuntime runtime;
        auto completed = std::make_shared<std::promise<void>>();
        auto future = completed->get_future();
        auto scheduler = GetScheduler(runtime);
        arcana::task_from_result<std::exception_ptr>().then(
            scheduler, arcana::cancellation::none(), [completed]() { completed->set_value(); });
        GetWithTimeout(future);
    });
}

TEST(JsRuntimeScheduler, PreservesConstZeroArgumentInvocation)
{
    struct Callable
    {
        std::shared_ptr<std::promise<bool>> Completed;
        void operator()() const { Completed->set_value(true); }
        void operator()() { Completed->set_value(false); }
    };

    RunWithTimeout([]() {
        Babylon::AppRuntime runtime;
        auto completed = std::make_shared<std::promise<bool>>();
        auto future = completed->get_future();
        GetScheduler(runtime)(Callable{completed});
        if (!GetWithTimeout(future))
        {
            throw std::runtime_error{"Existing const invocation was bypassed"};
        }
    });
}

TEST(JsRuntimeScheduler, DispatchCanRaceRuntimeTeardown)
{
    RunWithTimeout([]() {
        auto runtime = std::make_unique<Babylon::AppRuntime>();
        const auto scheduler = GetScheduler(*runtime);
        auto started = std::make_shared<std::promise<void>>();
        auto future = started->get_future();
        auto called = std::make_shared<std::atomic<size_t>>(0);
        auto stop = std::make_shared<std::atomic<bool>>(false);
        std::thread producer{[scheduler, started, called, stop]() {
            const auto deadline = std::chrono::steady_clock::now() + 5s;
            size_t attempts{};
            while (!stop->load() && std::chrono::steady_clock::now() < deadline)
            {
                scheduler([called]() { called->fetch_add(1); });
                if (++attempts == 100)
                {
                    started->set_value();
                }
                std::this_thread::yield();
            }
        }};
        auto joinProducer = gsl::finally([&producer, stop]() {
            *stop = true;
            producer.join();
        });
        GetWithTimeout(future);
        runtime.reset();

        const auto countAfterTeardown = called->load();
        scheduler([called]() { called->fetch_add(1); });
        if (called->load() != countAfterTeardown)
        {
            throw std::runtime_error{"Callback ran after concurrent teardown"};
        }
    });
}

TEST(JsRuntimeScheduler, ClosesBeforeDiscardingPendingWork)
{
    RunWithTimeout([]() {
        auto runtime = std::make_unique<Babylon::AppRuntime>();
        auto ready = std::make_shared<std::promise<void>>();
        auto future = ready->get_future();
        auto rejected = std::make_shared<std::atomic<bool>>(false);
        runtime->Dispatch([ready, rejected](Napi::Env env) {
            const Babylon::JsRuntimeScheduler scheduler{Babylon::JsRuntime::GetFromJavaScript(env)};
            auto probe = std::make_shared<OnDestruction>();
            probe->Callback = [scheduler, rejected]() {
                auto token = std::make_shared<int>(0);
                std::weak_ptr<int> weak = token;
                scheduler([token = std::move(token)]() {});
                *rejected = weak.expired();
            };
            // Shutdown discards this capture before clearing the runtime queue.
            Babylon::Internal::DelayedTaskScheduler::GetFromJavaScript(env)->Schedule(1h, [probe]() {});
            // Shutdown must not retrieve its state through a mutable JS property.
            env.Global().Set("savedNative", env.Global().Get("_native"));
            env.Global().Set("_native", env.Undefined());
            ready->set_value();
        });
        GetWithTimeout(future);
        runtime.reset();
        if (!rejected->load())
        {
            throw std::runtime_error{"Shutdown accepted work while discarding pending captures"};
        }
    });
}

TEST(JsRuntimeScheduler, RejectedCaptureCanRedispatchFromDestructor)
{
    RunWithTimeout([]() {
        auto runtime = std::make_unique<Babylon::AppRuntime>();
        const auto scheduler = GetScheduler(*runtime);
        runtime.reset();
        auto released = std::make_shared<std::atomic<bool>>(false);
        auto probe = std::make_shared<OnDestruction>();
        probe->Callback = [scheduler, released]() {
            scheduler([]() {});
            *released = true;
        };
        scheduler([probe = std::move(probe)]() {});
        if (!released->load())
        {
            throw std::runtime_error{"Rejected callback retained its captures"};
        }
    });
}

TEST(JsRuntimeScheduler, FinalizerReleasesDispatchCapturesOutsideLock)
{
    RunWithTimeout([]() {
        auto runtime = std::make_unique<Babylon::AppRuntime>();
        auto ready = std::make_shared<std::promise<void>>();
        auto future = ready->get_future();
        auto released = std::make_shared<std::atomic<bool>>(false);
        runtime->Dispatch([ready, released](Napi::Env env) {
            auto probe = std::make_shared<OnDestruction>();
            // An independently created JsRuntime exercises finalizer-only closure,
            // including breaking a dispatch capture that owns a scheduler copy.
            auto& nativeRuntime = Babylon::JsRuntime::CreateForJavaScript(env, [probe](auto) {});
            probe->Callback = [scheduler = Babylon::JsRuntimeScheduler{nativeRuntime}, released]() {
                scheduler([]() {});
                *released = true;
            };
            ready->set_value();
        });
        GetWithTimeout(future);
        runtime.reset();
        if (!released->load())
        {
            throw std::runtime_error{"Finalizer retained the dispatch function"};
        }
    });
}
