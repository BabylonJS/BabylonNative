#include <Babylon/AppRuntime.h>
#include <gtest/gtest.h>
#include <Babylon/ScriptLoader.h>
#include <Babylon/Polyfills/Scheduling.h>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <future>

// The unit test host's UnhandledExceptionHandler fails the whole JavaScript
// suite, so a throwing timer callback cannot be exercised from the script tests. This
// covers it natively instead.
TEST(Scheduling, IntervalSurvivesThrowingCallback)
{
    // Regression: repeating timeouts are re-armed after their callback returns
    // rather than before it runs, so an exception escaping a tick must not
    // silently stop the interval. Browsers keep the interval running and report
    // the error, and that is also what this dispatcher did previously.
    std::promise<int32_t> tickCountPromise;
    std::atomic<int32_t> unhandledErrorCount{0};

    Babylon::AppRuntime::Options options{};
    options.UnhandledExceptionHandler = [&unhandledErrorCount](const Napi::Error&) {
        ++unhandledErrorCount;
    };

    Babylon::AppRuntime runtime{options};

    runtime.Dispatch([&tickCountPromise](Napi::Env env) {
        Babylon::Polyfills::Scheduling::Initialize(env);

        auto reportTicks = Napi::Function::New(
            env, [&tickCountPromise](const Napi::CallbackInfo& info) {
                tickCountPromise.set_value(info[0].As<Napi::Number>().Int32Value());
            },
            "reportTicks");
        env.Global().Set("reportTicks", reportTicks);
    });

    Babylon::ScriptLoader loader{runtime};
    loader.Eval(R"(
        var ticks = 0;
        var id = setInterval(function () {
            ticks++;
            if (ticks === 3) {
                clearInterval(id);
                reportTicks(ticks);
                return;
            }
            throw new Error('tick failed');
        }, 1);
    )",
        "");

    auto tickCountFuture{tickCountPromise.get_future()};
    ASSERT_EQ(tickCountFuture.wait_for(std::chrono::seconds(10)), std::future_status::ready)
        << "the interval stopped after a tick threw";
    EXPECT_EQ(tickCountFuture.get(), 3);

    // The first two ticks threw, and those errors must still be surfaced.
    EXPECT_EQ(unhandledErrorCount.load(), 2);
}
