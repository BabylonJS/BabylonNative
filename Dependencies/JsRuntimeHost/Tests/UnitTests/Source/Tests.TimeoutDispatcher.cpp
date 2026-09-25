#include "TimeoutDispatcher.h"

#include <Babylon/AppRuntime.h>
#include <gtest/gtest.h>

#include <array>
#include <chrono>
#include <future>
#include <limits>

using namespace std::chrono_literals;

namespace Babylon::Polyfills::Internal
{
    struct TimeoutDispatcherTestAccess
    {
        static TimeoutDispatcher Create(Napi::Env env, Babylon::JsRuntime& runtime, int32_t lastTimeoutId)
        {
            return TimeoutDispatcher{env, runtime, lastTimeoutId};
        }
    };
}

TEST(TimeoutDispatcher, IdsStartAtOne)
{
    Babylon::AppRuntime runtime;
    std::promise<std::array<int32_t, 2>> idsPromise;
    runtime.Dispatch([&idsPromise](Napi::Env env) {
        Babylon::Polyfills::Internal::TimeoutDispatcher dispatcher{env, Babylon::JsRuntime::GetFromJavaScript(env)};
        idsPromise.set_value({dispatcher.Dispatch(nullptr, 1h), dispatcher.Dispatch(nullptr, 1h)});
    });

    auto idsFuture = idsPromise.get_future();
    ASSERT_EQ(idsFuture.wait_for(5s), std::future_status::ready);
    EXPECT_EQ(idsFuture.get(), (std::array<int32_t, 2>{1, 2}));
}

TEST(TimeoutDispatcher, IdsWrapBeforeSignedOverflow)
{
    constexpr auto MaxId = std::numeric_limits<int32_t>::max();
    Babylon::AppRuntime runtime;
    std::promise<std::array<int32_t, 4>> idsPromise;
    runtime.Dispatch([&idsPromise](Napi::Env env) {
        auto dispatcher = Babylon::Polyfills::Internal::TimeoutDispatcherTestAccess::Create(
            env, Babylon::JsRuntime::GetFromJavaScript(env), MaxId - 2);
        idsPromise.set_value({
            dispatcher.Dispatch(nullptr, 1h),
            dispatcher.Dispatch(nullptr, 1h),
            dispatcher.Dispatch(nullptr, 1h),
            dispatcher.Dispatch(nullptr, 1h)});
    });

    auto idsFuture = idsPromise.get_future();
    ASSERT_EQ(idsFuture.wait_for(5s), std::future_status::ready);
    EXPECT_EQ(idsFuture.get(), (std::array<int32_t, 4>{MaxId - 1, MaxId, 1, 2}));
}
