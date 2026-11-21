#include <Babylon/AppRuntime.h>
#include <Babylon/ScriptLoader.h>
#include <Babylon/Polyfills/Streams.h>
#include <gtest/gtest.h>
#include <atomic>
#include <chrono>
#include <future>
#include <iostream>
#include <string>

TEST(Streams, ReplacesPartialHostSuiteAndIsIdempotent)
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
        const auto hostReadableStream = Napi::Function::New(env, [](const Napi::CallbackInfo&) {}, "HostReadableStream");
        global.Set("ReadableStream", hostReadableStream);
        global.Set("TransformStream", env.Null());

        Babylon::Polyfills::Streams::Initialize(env);
        const auto installedReadableStream = global.Get("ReadableStream");
        const auto installedTransformStream = global.Get("TransformStream");
        EXPECT_FALSE(installedReadableStream.StrictEquals(hostReadableStream));
        if (!installedReadableStream.IsFunction() || !installedTransformStream.IsFunction())
        {
            complete("Streams::Initialize did not install a complete constructor suite.");
            return;
        }

        const auto transform = installedTransformStream.As<Napi::Function>().New({});
        EXPECT_TRUE(transform.Get("readable").As<Napi::Object>().InstanceOf(installedReadableStream.As<Napi::Function>()));

        Babylon::Polyfills::Streams::Initialize(env);
        EXPECT_TRUE(global.Get("ReadableStream").StrictEquals(installedReadableStream));
        EXPECT_TRUE(global.Get("TransformStream").StrictEquals(installedTransformStream));
        complete({});
    });

    const auto error = done.get_future().get();
    EXPECT_TRUE(error.empty()) << error;
}
