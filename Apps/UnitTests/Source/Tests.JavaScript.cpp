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

    // Watchdog: aborts the test with a core dump if both JS console output AND
    // render-thread frame progress stop for too long. Replaces opaque CI job
    // timeouts with an immediate, debuggable failure pointing at the last test
    // that ran. Threshold tunable via BN_WATCHDOG_TIMEOUT_S (set to 0 to disable).
    //
    // Two activity sources feed the watchdog:
    //  1. JS console output (via the polyfill console handler) — surfaces test progress.
    //  2. Render-thread frame count (via the pump loop below) — distinguishes a real
    //     deadlock (render thread stuck) from slow-but-progressing software-GL shader
    //     compile (mocha silent for minutes while bgfx::frame() chews through shader
    //     variants on llvmpipe). With the heartbeat, the watchdog only fires when
    //     forward progress has actually stopped.
    std::atomic<std::int64_t> g_lastConsoleActivityNs{0};
    std::atomic<std::uint64_t> g_pumpFrameCount{0};
    std::mutex g_lastMessageMutex;
    std::string g_lastMessage;

    // Per-frame timing accumulators (nanoseconds), published by pump loop, sampled by stats thread.
    std::atomic<std::uint64_t> g_pumpStartNs{0};   // total time inside StartRenderingCurrentFrame()
    std::atomic<std::uint64_t> g_pumpFinishNs{0};  // total time inside FinishRenderingCurrentFrame()
    std::atomic<std::uint64_t> g_pumpWaitNs{0};    // total time in wait_for(16ms)

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
        // 60s default. The watchdog also resets on render-thread frame progress
        // (see g_pumpFrameCount), so slow-but-progressing software-GL shader compile
        // no longer trips it; only a real stall (render thread stuck) does.
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
            std::uint64_t lastSeenFrameCount = g_pumpFrameCount.load(std::memory_order_relaxed);
            auto lastHeartbeat = std::chrono::steady_clock::now();
            while (!watchdogStop.load(std::memory_order_relaxed))
            {
                std::this_thread::sleep_for(std::chrono::seconds{1});
                // Treat render-thread frame progress as activity — distinguishes slow
                // shader compile (frames advancing while JS console is silent) from a
                // real deadlock (frames frozen).
                const std::uint64_t currentFrameCount = g_pumpFrameCount.load(std::memory_order_relaxed);
                if (currentFrameCount != lastSeenFrameCount)
                {
                    lastSeenFrameCount = currentFrameCount;
                    g_lastConsoleActivityNs.store(NowNs(), std::memory_order_relaxed);
                    auto now = std::chrono::steady_clock::now();
                    if (now - lastHeartbeat >= std::chrono::seconds{30})
                    {
                        // Snapshot per-phase accumulators so we can tell where wall time is going
                        // on slow runners (start / finish / wait split) and emit a delta from
                        // the last heartbeat to make per-30s rates easy to read in CI logs.
                        const std::uint64_t startNs = g_pumpStartNs.load(std::memory_order_relaxed);
                        const std::uint64_t finishNs = g_pumpFinishNs.load(std::memory_order_relaxed);
                        const std::uint64_t waitNs = g_pumpWaitNs.load(std::memory_order_relaxed);
                        std::cerr << "[watchdog-heartbeat] pump_frames=" << currentFrameCount
                                  << " start_ms=" << (startNs / 1'000'000ULL)
                                  << " finish_ms=" << (finishNs / 1'000'000ULL)
                                  << " wait_ms=" << (waitNs / 1'000'000ULL)
                                  << "\n" << std::flush;
                        lastHeartbeat = now;
                    }
                }
                const std::int64_t idleNs = NowNs() - g_lastConsoleActivityNs.load(std::memory_order_relaxed);
                if (idleNs >= thresholdNs)
                {
                    std::string lastMessage;
                    {
                        std::lock_guard lock{g_lastMessageMutex};
                        lastMessage = g_lastMessage;
                    }
                    std::cerr << "\n=== WATCHDOG: no console output or pump-frame progress for "
                              << (idleNs / 1'000'000'000LL) << "s ==="
                              << "\n=== Last console message: " << lastMessage << " ==="
                              << "\n=== Last pump frame count: " << currentFrameCount << " ==="
                              << "\n=== Aborting to produce a core dump for post-mortem analysis ==="
                              << "\n=== (raise BN_WATCHDOG_TIMEOUT_S or set to 0 to disable) ===\n"
                              << std::flush;
                    std::abort();
                }
            }
        }};
    }

    // Pump frames while JS tests run — tests use RAF internally and
    // SubmitCommands requires an active frame (the FrameCompletionScope
    // gate is open only between StartRenderingCurrentFrame and
    // FinishRenderingCurrentFrame). Hold the gate OPEN during the
    // wait_for so JS-thread submits/scope-acquires can land at any
    // point, then briefly close+reopen each iteration to allow
    // bgfx::frame() to advance. Without this pattern the open window
    // collapses to ~zero between back-to-back Start/Finish calls and
    // JS-thread work has to win a scheduler race against the pump
    // thread to land its scope before the gate closes — that race is
    // lost frequently on contended CI runners and produces 4-30x
    // runtime variance for the same code. Each iteration publishes
    // forward progress to the watchdog so slow-but-progressing shader
    // compile (silent on the JS console) doesn't trip a false positive.
    auto exitCodeFuture = exitCodePromise.get_future();
    device.StartRenderingCurrentFrame();
    while (true)
    {
        const auto waitStart = std::chrono::steady_clock::now();
        const auto waitStatus = exitCodeFuture.wait_for(std::chrono::milliseconds(16));
        g_pumpWaitNs.fetch_add(
            static_cast<std::uint64_t>(
                std::chrono::duration_cast<std::chrono::nanoseconds>(
                    std::chrono::steady_clock::now() - waitStart).count()),
            std::memory_order_relaxed);
        if (waitStatus == std::future_status::ready)
        {
            break;
        }

        const auto finishBegin = std::chrono::steady_clock::now();
        device.FinishRenderingCurrentFrame();
        const auto finishEnd = std::chrono::steady_clock::now();
        g_pumpFinishNs.fetch_add(
            static_cast<std::uint64_t>(
                std::chrono::duration_cast<std::chrono::nanoseconds>(finishEnd - finishBegin).count()),
            std::memory_order_relaxed);

        device.StartRenderingCurrentFrame();
        const auto startEnd = std::chrono::steady_clock::now();
        g_pumpStartNs.fetch_add(
            static_cast<std::uint64_t>(
                std::chrono::duration_cast<std::chrono::nanoseconds>(startEnd - finishEnd).count()),
            std::memory_order_relaxed);

        g_pumpFrameCount.fetch_add(1, std::memory_order_relaxed);
    }

    watchdogStop.store(true, std::memory_order_relaxed);
    if (watchdogThread.joinable())
    {
        watchdogThread.join();
    }

    // Gate is already OPEN here (last loop op was Start, or initial Start
    // if the loop never iterated). Pending JS work can complete before
    // we tear down the runtime.

    auto exitCode = exitCodeFuture.get();
    EXPECT_EQ(exitCode, 0);

    // Runtime destructor joins the JS thread; must happen before Finish.
    nativeCanvas.reset();

    device.FinishRenderingCurrentFrame();
}
