#include <gtest/gtest.h>

#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Polyfills/XMLHttpRequest.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/Window.h>
#include <Babylon/Polyfills/Canvas.h>
#include <Babylon/Polyfills/Blob.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <Babylon/Plugins/NativeEncoding.h>
#include <Babylon/ScriptLoader.h>

#include <atomic>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

extern Babylon::Graphics::Configuration g_deviceConfig;

namespace
{
    const char* EnumToString(Babylon::Polyfills::Console::LogLevel logLevel)
    {
        switch (logLevel)
        {
            case Babylon::Polyfills::Console::LogLevel::Log:
                return "log";
            case Babylon::Polyfills::Console::LogLevel::Warn:
                return "warn";
            case Babylon::Polyfills::Console::LogLevel::Error:
                return "error";
        }

        return "unknown";
    }

    // Watchdog: aborts the test with a core dump if the JS thread stops producing console
    // output for too long. Replaces opaque CI job timeouts with an immediate, debuggable
    // failure pointing at the last test that ran. Threshold tunable via BN_WATCHDOG_TIMEOUT_S
    // (set to 0 to disable).
    std::atomic<std::int64_t> g_lastConsoleActivityNs{0};
    std::mutex g_lastMessageMutex;
    std::string g_lastMessage;

    std::int64_t NowNs()
    {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(
                   std::chrono::steady_clock::now().time_since_epoch())
            .count();
    }

    void RecordConsoleActivity(const char* message)
    {
        g_lastConsoleActivityNs.store(NowNs(), std::memory_order_relaxed);
        std::lock_guard lock{g_lastMessageMutex};
        g_lastMessage.assign(message ? message : "");
    }

    int GetWatchdogTimeoutSeconds()
    {
        if (const char* env = std::getenv("BN_WATCHDOG_TIMEOUT_S"))
        {
            try
            {
                return std::stoi(env);
            }
            catch (...)
            {
            }
        }
        return 60;
    }
}

TEST(JavaScript, All)
{
    // Change this to true to wait for the JavaScript debugger to attach (only applies to V8)
    constexpr const bool waitForDebugger = false;

    Babylon::Graphics::Device device{g_deviceConfig};

    std::optional<Babylon::Polyfills::Canvas> nativeCanvas;

    Babylon::AppRuntime::Options options{};

    options.UnhandledExceptionHandler = [](const Napi::Error& error) {
        std::cerr << "[Uncaught Error] " << Napi::GetErrorString(error) << std::endl;
        std::quick_exit(1);
    };

    if (waitForDebugger)
    {
        std::cout << "Waiting for debugger..." << std::endl;
        options.WaitForDebugger = true;
    }

    Babylon::AppRuntime runtime{options};

    std::promise<int32_t> exitCodePromise;

    runtime.Dispatch([&exitCodePromise, &device, &nativeCanvas](Napi::Env env) {
        device.AddToJavaScript(env);

        Babylon::Polyfills::XMLHttpRequest::Initialize(env);
        Babylon::Polyfills::Console::Initialize(env, [](const char* message, Babylon::Polyfills::Console::LogLevel logLevel) {
            RecordConsoleActivity(message);
            std::cout << "[" << EnumToString(logLevel) << "] " << message << std::endl;
        });
        Babylon::Polyfills::Window::Initialize(env);
        Babylon::Polyfills::Blob::Initialize(env);
        nativeCanvas.emplace(Babylon::Polyfills::Canvas::Initialize(env));
        Babylon::Plugins::NativeEngine::Initialize(env);
        Babylon::Plugins::NativeEncoding::Initialize(env);

        auto setExitCodeCallback = Napi::Function::New(
            env, [&exitCodePromise](const Napi::CallbackInfo& info) {
                Napi::Env env = info.Env();
                exitCodePromise.set_value(info[0].As<Napi::Number>().Int32Value());
            },
            "setExitCode");
        env.Global().Set("setExitCode", setExitCodeCallback);
    });

    Babylon::ScriptLoader loader{runtime};
    loader.Eval("location = { href: '' };", ""); // Required for Mocha.js as we do not have a location in Babylon Native
    loader.LoadScript("app:///Assets/babylon.max.js");
    loader.LoadScript("app:///Assets/babylonjs.materials.js");
    loader.LoadScript("app:///Assets/tests.javaScript.all.js");

    device.StartRenderingCurrentFrame();
    device.FinishRenderingCurrentFrame();

    // Start the JS-progress watchdog. Anchor "last activity" to now so we don't fire
    // before the JS engine produces its first console output.
    g_lastConsoleActivityNs.store(NowNs(), std::memory_order_relaxed);
    const int watchdogTimeoutSeconds = GetWatchdogTimeoutSeconds();
    std::atomic<bool> watchdogStop{false};
    std::thread watchdogThread;
    if (watchdogTimeoutSeconds > 0)
    {
        watchdogThread = std::thread{[&watchdogStop, watchdogTimeoutSeconds]() {
            const std::int64_t thresholdNs = std::int64_t{watchdogTimeoutSeconds} * 1'000'000'000LL;
            while (!watchdogStop.load(std::memory_order_relaxed))
            {
                std::this_thread::sleep_for(std::chrono::seconds{1});
                const std::int64_t idleNs = NowNs() - g_lastConsoleActivityNs.load(std::memory_order_relaxed);
                if (idleNs >= thresholdNs)
                {
                    std::string lastMessage;
                    {
                        std::lock_guard lock{g_lastMessageMutex};
                        lastMessage = g_lastMessage;
                    }
                    std::cerr << "\n=== WATCHDOG: no console output for "
                              << (idleNs / 1'000'000'000LL) << "s ==="
                              << "\n=== Last console message: " << lastMessage << " ==="
                              << "\n=== Aborting to produce a core dump for post-mortem analysis ==="
                              << "\n=== (raise BN_WATCHDOG_TIMEOUT_S or set to 0 to disable) ===\n"
                              << std::flush;
                    std::abort();
                }
            }
        }};
    }

    // Pump frames while JS tests run — tests use RAF internally and
    // SubmitCommands requires an active frame.
    auto exitCodeFuture = exitCodePromise.get_future();
    while (exitCodeFuture.wait_for(std::chrono::milliseconds(16)) != std::future_status::ready)
    {
        device.StartRenderingCurrentFrame();
        device.FinishRenderingCurrentFrame();
    }

    watchdogStop.store(true, std::memory_order_relaxed);
    if (watchdogThread.joinable())
    {
        watchdogThread.join();
    }

    // Keep the frame open during shutdown so any pending JS work
    // (e.g., SubmitCommands acquiring a FrameCompletionScope) can complete.
    device.StartRenderingCurrentFrame();

    auto exitCode = exitCodeFuture.get();
    EXPECT_EQ(exitCode, 0);

    // Runtime destructor joins the JS thread; must happen before Finish.
    nativeCanvas.reset();

    device.FinishRenderingCurrentFrame();
}
