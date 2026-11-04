#include <Babylon/AppRuntime.h>
#include <gtest/gtest.h>
#include <napi/env.h>

#include <chrono>
#include <future>
#include <stdexcept>

TEST(NodeApi, HermesDrainJobsReleasesWeakRefTargets)
{
    std::promise<double> externalBytes;
    auto completed = externalBytes.get_future();
    Babylon::AppRuntime runtime{};
    runtime.Dispatch([&](Napi::Env env) {
        try
        {
            for (size_t i = 0; i < 256; ++i)
            {
                {
                    Napi::HandleScope scope{env};
                    Napi::Eval(env, "new WeakRef(new Uint8Array(1024 * 1024));", "weakref-job.js");
                }
                Napi::DrainJobs(env);
            }
            externalBytes.set_value(Napi::Eval(env,
                "HermesInternal.getInstrumentedStats().js_externalBytes",
                "weakref-heap.js").As<Napi::Number>().DoubleValue());
        }
        catch (const Napi::Error& error)
        {
            externalBytes.set_exception(std::make_exception_ptr(std::runtime_error{error.Message()}));
        }
    });
    ASSERT_EQ(completed.wait_for(std::chrono::seconds{30}), std::future_status::ready);
    EXPECT_LT(completed.get(), 64 * 1024 * 1024);
}
