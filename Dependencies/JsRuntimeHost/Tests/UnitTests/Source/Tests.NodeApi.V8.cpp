#include <Babylon/AppRuntime.h>
#include <gtest/gtest.h>
#include <napi/env.h>

#include <chrono>
#include <future>
#include <utility>

TEST(NodeApi, V8FinalizersRunBeforeShutdown)
{
    constexpr size_t count = 32;
    size_t finalized = 0;
    std::promise<size_t> observed;
    auto completed = observed.get_future();
    Babylon::AppRuntime runtime{};
    runtime.Dispatch([&](Napi::Env env) {
        {
            Napi::HandleScope scope{env};
            for (size_t i = 0; i < count; ++i)
            {
                Napi::External<size_t>::New(env, &finalized, [](Napi::Env, size_t* finalized) {
                    ++(*finalized);
                });
            }
        }
        Napi::GetContext(env)->GetIsolate()->LowMemoryNotification();
        // Collecting the externals posted a task that runs their finalizers ahead of this one.
        runtime.Dispatch([&](Napi::Env) {
            observed.set_value(finalized);
        });
    });
    ASSERT_EQ(completed.wait_for(std::chrono::seconds{30}), std::future_status::ready);
    EXPECT_EQ(completed.get(), count);
}

TEST(NodeApi, V8AttachRetainsStandaloneFinalizerBehavior)
{
    constexpr size_t count = 32;
    size_t finalized = 0;
    std::promise<std::pair<size_t, size_t>> observed;
    auto completed = observed.get_future();
    Babylon::AppRuntime runtime{};
    runtime.Dispatch([&](Napi::Env runtimeEnv) {
        Napi::Env (*attach)(v8::Local<v8::Context>) = &Napi::Attach;
        auto env = attach(Napi::GetContext(runtimeEnv));
        {
            Napi::HandleScope scope{env};
            for (size_t i = 0; i < count; ++i)
            {
                Napi::External<size_t>::New(env, &finalized, [](Napi::Env, size_t* finalized) {
                    ++(*finalized);
                });
            }
        }
        Napi::GetContext(env)->GetIsolate()->LowMemoryNotification();
        const auto beforeDetach = finalized;
        Napi::Detach(env);
        observed.set_value({beforeDetach, finalized});
    });
    ASSERT_EQ(completed.wait_for(std::chrono::seconds{30}), std::future_status::ready);
    const auto result = completed.get();
    EXPECT_EQ(result.first, 0u);
    EXPECT_EQ(result.second, count);
}
