#include <Babylon/AppRuntime.h>
#include <Babylon/ScriptLoader.h>
#include <Babylon/Polyfills/Fetch.h>
#include <gtest/gtest.h>
#include <atomic>
#include <chrono>
#include <future>
#include <iostream>
#include <string>

TEST(Fetch, PreservesCompleteHostClassesAndIsIdempotent)
{
    std::promise<std::string> done;
    std::atomic_bool completed{};
    const auto complete = [&done, &completed](std::string result) {
        if (!completed.exchange(true))
        {
            done.set_value(std::move(result));
        }
    };

    Babylon::AppRuntime::Options options{};
    options.UnhandledExceptionHandler = [&complete](const Napi::Error& error) {
        complete(Napi::GetErrorString(error));
    };
    Babylon::AppRuntime runtime{options};

    runtime.Dispatch([&complete](Napi::Env env) {
        auto global = env.Global();
        const auto hostHeaders = Napi::Function::New(env, [](const Napi::CallbackInfo&) {}, "HostHeaders");
        const auto hostResponse = Napi::Function::New(env, [](const Napi::CallbackInfo&) {}, "HostResponse");
        global.Set("Headers", hostHeaders);
        global.Set("Response", hostResponse);

        Babylon::Polyfills::Fetch::Initialize(env);
        EXPECT_TRUE(global.Get("Headers").StrictEquals(hostHeaders));
        EXPECT_TRUE(global.Get("Response").StrictEquals(hostResponse));

        const auto installedFetch = global.Get("fetch");
        Babylon::Polyfills::Fetch::Initialize(env);
        EXPECT_TRUE(global.Get("Headers").StrictEquals(hostHeaders));
        EXPECT_TRUE(global.Get("Response").StrictEquals(hostResponse));
        EXPECT_TRUE(global.Get("fetch").StrictEquals(installedFetch));
        complete({});
    });

    const auto error = done.get_future().get();
    EXPECT_TRUE(error.empty()) << error;
}

TEST(Fetch, ReplacesPartialOrNullHostClassesAsACompletePair)
{
    std::promise<std::string> done;
    std::atomic_bool completed{};
    const auto complete = [&done, &completed](std::string result) {
        if (!completed.exchange(true))
        {
            done.set_value(std::move(result));
        }
    };

    Babylon::AppRuntime::Options options{};
    options.UnhandledExceptionHandler = [&complete](const Napi::Error& error) {
        complete(Napi::GetErrorString(error));
    };
    Babylon::AppRuntime runtime{options};

    runtime.Dispatch([&complete](Napi::Env env) {
        auto global = env.Global();
        const auto hostHeaders = Napi::Function::New(env, [](const Napi::CallbackInfo&) {}, "HostHeaders");
        global.Set("Headers", hostHeaders);
        global.Set("Response", env.Null());

        Babylon::Polyfills::Fetch::Initialize(env);
        const auto installedHeaders = global.Get("Headers");
        const auto installedResponse = global.Get("Response");
        EXPECT_FALSE(installedHeaders.StrictEquals(hostHeaders));
        if (!installedHeaders.IsFunction() || !installedResponse.IsFunction())
        {
            complete("Fetch::Initialize did not install a complete Headers/Response pair.");
            return;
        }

        const auto response = installedResponse.As<Napi::Function>().New({});
        EXPECT_TRUE(response.Get("headers").As<Napi::Object>().InstanceOf(installedHeaders.As<Napi::Function>()));

        Babylon::Polyfills::Fetch::Initialize(env);
        EXPECT_TRUE(global.Get("Headers").StrictEquals(installedHeaders));
        EXPECT_TRUE(global.Get("Response").StrictEquals(installedResponse));
        complete({});
    });

    const auto error = done.get_future().get();
    EXPECT_TRUE(error.empty()) << error;
}
