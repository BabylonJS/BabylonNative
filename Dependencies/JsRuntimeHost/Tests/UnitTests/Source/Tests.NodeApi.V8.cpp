#include <Babylon/AppRuntime.h>
#include <gtest/gtest.h>
#include <napi/env.h>

#include <chrono>
#include <future>

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
