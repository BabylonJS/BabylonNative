#include <Babylon/AppRuntime.h>
#include <gtest/gtest.h>
#include <napi/env.h>

#include <chrono>
#include <future>
#include <string>

namespace
{
    void ExpectTrueInRuntime(const char* script)
    {
        std::promise<std::string> result;
        auto ready = result.get_future();
        Babylon::AppRuntime runtime{};
        runtime.Dispatch([&](Napi::Env env) {
            try
            {
                result.set_value(Napi::Eval(env, script, "date-time-format.js").As<Napi::Boolean>().Value() ? "" : "predicate returned false");
            }
            catch (const std::exception& error)
            {
                result.set_value(error.what());
            }
        });
        ASSERT_EQ(ready.wait_for(std::chrono::seconds{10}), std::future_status::ready);
        EXPECT_EQ(ready.get(), "");
    }
}

TEST(Intl, DateOnlyOptionsHonorTimeZoneAcrossMidnight)
{
    ExpectTrueInRuntime(R"js((() => {
        const instant = new Date("2024-01-16T00:05:00Z");
        const check = (timeZone, expectedDay, expectedWeekday) => {
            const options = { weekday: "short", year: "numeric", month: "long", day: "numeric", timeZone };
            const formatter = new Intl.DateTimeFormat("en-US", options);
            const parts = formatter.formatToParts(instant);
            return instant.toLocaleString("en-US", options) === formatter.format(instant) &&
                parts.some(p => p.type === "day" && p.value === expectedDay) &&
                parts.some(p => p.type === "weekday" && p.value === expectedWeekday) &&
                !parts.some(p => ["hour", "minute", "second", "timeZoneName"].includes(p.type));
        };
        return check("UTC", "16", "Tue") && check("America/Los_Angeles", "15", "Mon");
    })())js");
}

TEST(Intl, DateLocaleStringRejectsUnknownTimeZone)
{
    ExpectTrueInRuntime(R"js((() => {
        try { new Date(0).toLocaleString("en-US", { timeZone: "Not/A_Zone" }); }
        catch (error) { return error instanceof RangeError; }
        return false;
    })())js");
}
