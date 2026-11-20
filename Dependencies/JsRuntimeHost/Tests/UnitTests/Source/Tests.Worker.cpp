#include "TestAssetRoot.h"
#include <Babylon/AppRuntime.h>
#include <Babylon/ScriptLoader.h>
#include <Babylon/Polyfills/Scheduling.h>
#include <Babylon/Polyfills/URL.h>
#if defined(JSRUNTIMEHOST_TEST_WORKER)
#include <Babylon/Polyfills/Worker.h>
#endif
#include <gtest/gtest.h>
#include <atomic>
#include <chrono>
#include <future>
#include <iostream>
#include <stdexcept>
#include <string>

#if defined(JSRUNTIMEHOST_TEST_WORKER) && !defined(__ANDROID__) && \
    (defined(JSR_NAPI_ENGINE_JAVASCRIPTCORE) || defined(JSR_NAPI_ENGINE_QUICKJS) || \
     defined(JSR_NAPI_ENGINE_V8) || defined(JSR_NAPI_ENGINE_HERMES))
TEST(Worker, ParentTeardownWithPendingDelivery)
{
    for (const bool terminateImmediately : {false, true})
    {
        for (size_t iteration = 0; iteration < 16; ++iteration)
        {
            auto completed = std::make_shared<std::promise<void>>();
            auto ready = completed->get_future();
            Babylon::AppRuntime runtime{};
            runtime.Dispatch([completed, terminateImmediately](Napi::Env env) {
                try
                {
                    Babylon::Polyfills::Worker::Initialize(env, {});
                    Napi::Eval(env,
                        terminateImmediately
                            ? "var worker = new Worker(\"data:text/javascript,postMessage('ready')\"); worker.terminate();"
                            : "var worker = new Worker(\"data:text/javascript,postMessage('ready')\");",
                        "worker-parent-teardown.js");
                    completed->set_value();
                }
                catch (const Napi::Error& error)
                {
                    completed->set_exception(std::make_exception_ptr(std::runtime_error{Napi::GetErrorString(error)}));
                }
                catch (...)
                {
                    completed->set_exception(std::current_exception());
                }
            });
            ASSERT_EQ(ready.wait_for(std::chrono::seconds{10}), std::future_status::ready);
            EXPECT_NO_THROW(ready.get());
            // Destruction must safely discard work posted by a worker during parent teardown.
        }
    }
}

TEST(Worker, PreservesHostDOMException)
{
    std::promise<void> done;
    Babylon::AppRuntime runtime{};

    runtime.Dispatch([&done](Napi::Env env) {
        auto global = env.Global();
        const auto hostDOMException =
            Napi::Function::New(env, [](const Napi::CallbackInfo&) {}, "HostDOMException");
        global.Set("DOMException", hostDOMException);

        Babylon::Polyfills::Worker::Options options{};
        options.ScriptRoot = TestAssetRoot().string();
        Babylon::Polyfills::Worker::Initialize(env, std::move(options));

        // Worker is composed with independent browser polyfills. Installing
        // its lifecycle/event glue must not invalidate exceptions created by
        // IndexedDB (or another host implementation) before Worker starts.
        EXPECT_TRUE(global.Get("DOMException").StrictEquals(hostDOMException));
        done.set_value();
    });

    done.get_future().get();
}

TEST(Worker, UndefinedTypeMeansClassic)
{
    // WorkerOptions is a WebIDL dictionary, so `{ type: undefined }` is the same as passing no type
    // (a spread of optional options produces exactly that); only a value that is present and not
    // "classic"/"module" is a TypeError.
    std::promise<void> done;
    bool undefinedTypeAccepted{false};
    bool bogusTypeRejected{false};
    Babylon::AppRuntime runtime{};

    runtime.Dispatch([&](Napi::Env env) {
        Babylon::Polyfills::Scheduling::Initialize(env);
        Babylon::Polyfills::URL::Initialize(env);

        Babylon::Polyfills::Worker::Options options{};
        options.ScriptRoot = TestAssetRoot().string();
        Babylon::Polyfills::Worker::Initialize(env, std::move(options));

        const auto workerConstructor = env.Global().Get("Worker").As<Napi::Function>();
        const auto scriptUrl = Napi::String::New(env, "app:///Assets/symlink_target.js");

        auto undefinedType = Napi::Object::New(env);
        undefinedType.Set("type", env.Undefined());
        try
        {
            auto worker = workerConstructor.New({scriptUrl, undefinedType});
            worker.Get("terminate").As<Napi::Function>().Call(worker, {});
            undefinedTypeAccepted = true;
        }
        catch (const Napi::Error&)
        {
        }

        auto bogusType = Napi::Object::New(env);
        bogusType.Set("type", Napi::String::New(env, "bogus"));
        try
        {
            workerConstructor.New({scriptUrl, bogusType});
        }
        catch (const Napi::Error& error)
        {
            bogusTypeRejected = error.Get("name").ToString().Utf8Value() == "TypeError";
        }

        done.set_value();
    });

    done.get_future().get();
    EXPECT_TRUE(undefinedTypeAccepted);
    EXPECT_TRUE(bogusTypeRejected);
}

TEST(Worker, WebPlatformTests)
{
    struct Result
    {
        bool Passed{};
        std::string Detail{};
    };

    std::promise<Result> completion;
    std::atomic_bool completed{false};

    Babylon::AppRuntime::Options runtimeOptions{};
    runtimeOptions.UnhandledExceptionHandler = [&completion, &completed](const Napi::Error& error) {
        if (!completed.exchange(true))
        {
            completion.set_value({false, Napi::GetErrorString(error)});
        }
    };

    Babylon::AppRuntime runtime{std::move(runtimeOptions)};
    runtime.Dispatch([&completion, &completed](Napi::Env env) {
        Babylon::Polyfills::Scheduling::Initialize(env);
        Babylon::Polyfills::URL::Initialize(env);

        Babylon::Polyfills::Worker::Options options{};
        options.ScriptRoot = (TestAssetRoot() / "Assets" / "WebPlatformTests").string();
        options.ConsoleCallback = [](const char* message) {
            std::cerr << "[Worker] " << message << std::endl;
        };
        Babylon::Polyfills::Worker::Initialize(env, std::move(options));

#if defined(JSR_NAPI_ENGINE_JAVASCRIPTCORE) && (!defined(__APPLE__) || defined(JSR_ENABLE_JSC_WATCHDOG))
        // System JSC exposes the execution-time-limit hook used to interrupt
        // a worker stuck in top-level evaluation. Other adapters currently
        // terminate cooperatively between dispatches, so the infinite-loop
        // WPT regression is intentionally JSC-only for now.
        env.Global().Set("__jsrhCanInterruptWorker", Napi::Boolean::New(env, true));
#else
        env.Global().Set("__jsrhCanInterruptWorker", Napi::Boolean::New(env, false));
#endif

        env.Global().Set("__jsrhWptDone", Napi::Function::New(
            env,
            [&completion, &completed](const Napi::CallbackInfo& info) {
                if (!completed.exchange(true))
                {
                    completion.set_value({
                        info[0].ToBoolean().Value(),
                        info[1].ToString().Utf8Value(),
                    });
                }
            },
            "__jsrhWptDone"));
        env.Global().Set("__jsrhWptProgress", Napi::Function::New(
            env,
            [](const Napi::CallbackInfo& info) {
                std::cerr << "[Worker WPT] " << info[0].ToString().Utf8Value() << std::endl;
            },
            "__jsrhWptProgress"));
    });

    Babylon::ScriptLoader loader{runtime};
    loader.LoadScript("app:///Assets/WebPlatformTests/runner.js");

    auto future = completion.get_future();
    ASSERT_EQ(future.wait_for(std::chrono::seconds{90}), std::future_status::ready)
        << "Worker WPT subset timed out";
    const auto result = future.get();
    EXPECT_TRUE(result.Passed) << result.Detail;
}
#endif
