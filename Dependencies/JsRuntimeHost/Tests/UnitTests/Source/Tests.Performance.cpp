#include <Babylon/AppRuntime.h>
#include <Babylon/ScriptLoader.h>
#include <Babylon/Polyfills/Performance.h>
#include <gtest/gtest.h>
#include <atomic>
#include <chrono>
#include <future>
#include <iostream>
#include <string>

TEST(Performance, TimeOriginIsPerEnvironment)
{
    // Regression: performance.now() measured from one process-wide start time that every
    // Initialize() rewrote, so bringing up a second runtime moved the first one's clock back to
    // zero. Each environment keeps its own origin now, so the first clock stays monotonic.
    const auto initialize = [](Babylon::AppRuntime& runtime) {
        std::promise<void> done;
        runtime.Dispatch([&done](Napi::Env env) {
            Babylon::Polyfills::Performance::Initialize(env);
            done.set_value();
        });
        done.get_future().get();
    };
    const auto now = [](Babylon::AppRuntime& runtime) {
        std::promise<double> value;
        runtime.Dispatch([&value](Napi::Env env) {
            auto performance = env.Global().Get("performance").As<Napi::Object>();
            value.set_value(performance.Get("now").As<Napi::Function>().Call(performance, {}).As<Napi::Number>().DoubleValue());
        });
        return value.get_future().get();
    };

    Babylon::AppRuntime first{};
    initialize(first);
    std::this_thread::sleep_for(std::chrono::milliseconds{50});
    const double before = now(first);

    Babylon::AppRuntime second{};
    initialize(second);
    const double after = now(first);

    EXPECT_GE(after, before);
}
