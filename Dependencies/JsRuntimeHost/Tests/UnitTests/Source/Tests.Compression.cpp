#include <Babylon/AppRuntime.h>
#include <Babylon/ScriptLoader.h>
#include <Babylon/Polyfills/Compression.h>
#include <gtest/gtest.h>
#include <atomic>
#include <chrono>
#include <future>
#include <iostream>
#include <string>

TEST(Compression, PreservesHostConstructorsAndIsIdempotent)
{
    Babylon::AppRuntime runtime{};
    std::promise<void> done;

    runtime.Dispatch([&done](Napi::Env env) {
        auto global = env.Global();
        const auto hostCompressionStream = Napi::Function::New(env, [](const Napi::CallbackInfo&) {}, "HostCompressionStream");
        global.Set("CompressionStream", hostCompressionStream);

        Babylon::Polyfills::Compression::Initialize(env);
        EXPECT_TRUE(global.Get("CompressionStream").StrictEquals(hostCompressionStream));
        EXPECT_TRUE(global.Get("DecompressionStream").IsFunction());

        const auto installedDecompressionStream = global.Get("DecompressionStream");
        Babylon::Polyfills::Compression::Initialize(env);
        EXPECT_TRUE(global.Get("CompressionStream").StrictEquals(hostCompressionStream));
        EXPECT_TRUE(global.Get("DecompressionStream").StrictEquals(installedDecompressionStream));
        done.set_value();
    });

    done.get_future().get();
}
