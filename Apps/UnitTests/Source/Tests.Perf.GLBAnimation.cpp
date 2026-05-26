// Benchmarks Babylon Native cold-start + GLB-driven animation rendering, focusing on C++ wall
// time -- not GPU work. The graphics device is constructed with an empty configuration (no
// window, no width/height, no device), which selects bgfx's noop renderer. Babylon's CPU-side
// rendering pipeline still runs (engine.beginFrame, scene.render, draw call construction, bgfx
// encoder submits), but no swapchain exists so there is no vsync wait and no GPU work.
//
// All timings are accumulated into a single ordered list and printed at the end:
//   * [C++] entries are measured on the test thread with steady_clock.
//   * [init] entries are measured on the JS thread with steady_clock (inside the plugin-init
//     dispatch lambda) -- one entry per polyfill / plugin Initialize call.
//   * [load] entries split each script into "fetch" (std::ifstream read) and "eval"
//     (loader.Eval + dispatch round-trip).
//   * [JS] entries are reported from JS via the _perfReport host callback. Frame loops report
//     min / max / avg / total per AnimationGroup using the high-resolution native _perfNow().
//
// The summary is printed to stdout at the end of the test.

#include "App.h"

#include <gtest/gtest.h>

#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/Window.h>
#include <Babylon/Polyfills/XMLHttpRequest.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <Babylon/ScriptLoader.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <future>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace
{
    using PerfClock = std::chrono::steady_clock;

    double DurationMs(PerfClock::duration d)
    {
        return std::chrono::duration<double, std::milli>(d).count();
    }

    std::string FormatMs(double ms)
    {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(3) << ms << " ms";
        return oss.str();
    }

    struct PerfEntry
    {
        std::string Label;
        double Ms;
    };

    // Read a script file synchronously from the Assets directory next to the exe. Mirrors
    // what UrlLib's LoadFileAsync does for app:// URLs on Win32 (ifstream + rdbuf), without
    // the threadpool hop -- we want the raw I/O time, not the scheduling overhead.
    std::string ReadAssetText(const std::filesystem::path& path)
    {
        std::ifstream file{path};
        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        std::ostringstream ss;
        ss << file.rdbuf();
        return ss.str();
    }
}

TEST(Perf, GLBAnimation)
{
    // This is a wall-clock performance benchmark; running it as part of the default UnitTests
    // suite would add multi-second network/render time and noisy numbers to every dev/CI run.
    // Opt in by setting BN_RUN_PERF_TESTS=1 in the environment.
    if (const char* env = std::getenv("BN_RUN_PERF_TESTS"); !env || std::string{env} == "0")
    {
        GTEST_SKIP() << "Perf.GLBAnimation is opt-in; set BN_RUN_PERF_TESTS=1 to run it.";
    }

    const auto totalStart = PerfClock::now();

    std::mutex perfMutex;
    std::vector<PerfEntry> perfEntries;
    auto logPerf = [&](std::string label, double ms) {
        std::scoped_lock lock{perfMutex};
        perfEntries.push_back({std::move(label), ms});
    };

    // Noop renderer: no window, no width/height, no device. bgfx will pick its noop backend.
    // CPU-side rendering still runs (encoders, draw call submission); no swapchain => no vsync.
    Babylon::Graphics::Configuration config{};

    const auto deviceStart = PerfClock::now();
    Babylon::Graphics::Device device{config};
    logPerf("[C++]    Graphics::Device ctor", DurationMs(PerfClock::now() - deviceStart));

    Babylon::AppRuntime::Options options{};
    options.UnhandledExceptionHandler = [](const Napi::Error& error) {
        std::cerr << "[Perf.GLBAnimation] [Uncaught Error] " << Napi::GetErrorString(error) << std::endl;
        std::quick_exit(1);
    };

    const auto runtimeStart = PerfClock::now();
    Babylon::AppRuntime runtime{options};
    logPerf("[C++]    AppRuntime ctor", DurationMs(PerfClock::now() - runtimeStart));

    std::promise<void> donePromise;
    std::atomic<bool> failed{false};
    std::string failReason;

    // Initialize polyfills + plugins and install perf hooks. Each Initialize() call is timed
    // independently on the JS thread, all from within a single dispatch round-trip so we don't
    // pollute numbers with per-call dispatch latency.
    const auto pluginsDispatchStart = PerfClock::now();
    std::promise<void> pluginsReadyPromise;
    const auto perfNowBaseline = PerfClock::now();

    runtime.Dispatch([&](Napi::Env env) {
        auto stepStart = PerfClock::now();

        device.AddToJavaScript(env);
        {
            const auto now = PerfClock::now();
            logPerf("[init]   device.AddToJavaScript", DurationMs(now - stepStart));
            stepStart = now;
        }

        Babylon::Polyfills::XMLHttpRequest::Initialize(env);
        {
            const auto now = PerfClock::now();
            logPerf("[init]   Polyfills::XMLHttpRequest", DurationMs(now - stepStart));
            stepStart = now;
        }

        Babylon::Polyfills::Console::Initialize(env, [](const char* message, Babylon::Polyfills::Console::LogLevel level) {
            const char* prefix = "[js]";
            switch (level)
            {
                case Babylon::Polyfills::Console::LogLevel::Warn:  prefix = "[js warn]"; break;
                case Babylon::Polyfills::Console::LogLevel::Error: prefix = "[js error]"; break;
                default: break;
            }
            std::cout << prefix << " " << message << std::endl;
        });
        {
            const auto now = PerfClock::now();
            logPerf("[init]   Polyfills::Console", DurationMs(now - stepStart));
            stepStart = now;
        }

        Babylon::Polyfills::Window::Initialize(env);
        {
            const auto now = PerfClock::now();
            logPerf("[init]   Polyfills::Window", DurationMs(now - stepStart));
            stepStart = now;
        }

        Babylon::Plugins::NativeEngine::Initialize(env);
        {
            const auto now = PerfClock::now();
            logPerf("[init]   Plugins::NativeEngine", DurationMs(now - stepStart));
            stepStart = now;
        }

        // High-resolution clock for JS-side timings. Returns milliseconds (double) since the
        // baseline taken just before this dispatch, with sub-ms resolution.
        env.Global().Set("_perfNow", Napi::Function::New(env,
            [perfNowBaseline](const Napi::CallbackInfo& info) -> Napi::Value {
                const auto delta = PerfClock::now() - perfNowBaseline;
                return Napi::Number::New(info.Env(), DurationMs(delta));
            },
            "_perfNow"));

        env.Global().Set("_perfReport", Napi::Function::New(env,
            [&logPerf](const Napi::CallbackInfo& info) {
                auto label = info[0].As<Napi::String>().Utf8Value();
                const auto ms = info[1].As<Napi::Number>().DoubleValue();
                logPerf(std::move(label), ms);
            },
            "_perfReport"));

        env.Global().Set("_perfDone", Napi::Function::New(env,
            [&donePromise](const Napi::CallbackInfo&) {
                try
                {
                    donePromise.set_value();
                }
                catch (const std::future_error&)
                {
                    // Already signaled; ignore duplicate.
                }
            },
            "_perfDone"));

        env.Global().Set("_perfFail", Napi::Function::New(env,
            [&donePromise, &failed, &failReason](const Napi::CallbackInfo& info) {
                failed.store(true);
                failReason = info[0].As<Napi::String>().Utf8Value();
                try
                {
                    donePromise.set_value();
                }
                catch (const std::future_error&)
                {
                }
            },
            "_perfFail"));

        pluginsReadyPromise.set_value();
    });
    pluginsReadyPromise.get_future().get();
    logPerf("[C++]    plugin-init dispatch round-trip (total)", DurationMs(PerfClock::now() - pluginsDispatchStart));

    Babylon::ScriptLoader loader{runtime};
    // Mocha-style globals expect `location.href`; not strictly needed here, but harmless.
    loader.Eval("location = { href: '' };", "");

    const auto assetsDir = GetExecutableDirectory() / "Assets";

    // Load a script split into two timings: file read (fetch) and JS eval (dispatch + Napi::Eval
    // round-trip). The latter is measured by waiting for a subsequent loader.Dispatch to settle.
    auto loadFetchAndEval = [&](const char* fileName, const char* urlForEval) {
        const auto fetchStart = PerfClock::now();
        const std::string source = ReadAssetText(assetsDir / fileName);
        const double fetchMs = DurationMs(PerfClock::now() - fetchStart);
        logPerf(std::string{"[load]   "} + fileName + " - fetch (ifstream)", fetchMs);

        const auto evalStart = PerfClock::now();
        std::promise<void> p;
        loader.Eval(source, urlForEval);
        loader.Dispatch([&p](Napi::Env) { p.set_value(); });
        p.get_future().get();
        const double evalMs = DurationMs(PerfClock::now() - evalStart);
        logPerf(std::string{"[load]   "} + fileName + " - eval", evalMs);
    };

    loadFetchAndEval("babylon.max.js", "app:///Assets/babylon.max.js");
    loadFetchAndEval("babylonjs.loaders.js", "app:///Assets/babylonjs.loaders.js");
    loadFetchAndEval("tests.perf.glbAnimation.js", "app:///Assets/tests.perf.glbAnimation.js");

    // Drive the bgfx pipeline so NativeEngine's requestAnimationFrame callbacks fire. The
    // benchmark JS posts back via _perfDone(); we busy-wait on it while ticking the device.
    const auto renderStart = PerfClock::now();
    auto update = device.GetUpdate("update");
    device.StartRenderingCurrentFrame();
    update.Start();

    auto doneFuture = donePromise.get_future();
    while (doneFuture.wait_for(std::chrono::milliseconds(1)) != std::future_status::ready)
    {
        update.Finish();
        device.FinishRenderingCurrentFrame();
        device.StartRenderingCurrentFrame();
        update.Start();
    }
    update.Finish();
    device.FinishRenderingCurrentFrame();
    logPerf("[C++]    render loop wall time", DurationMs(PerfClock::now() - renderStart));

    const double totalMs = DurationMs(PerfClock::now() - totalStart);

    // Determine column widths so the summary lines up regardless of label length.
    std::size_t labelWidth = 0;
    {
        std::scoped_lock lock{perfMutex};
        for (const auto& e : perfEntries)
        {
            // Parenthesized to suppress the Windows <windows.h> `max` macro expansion.
            labelWidth = (std::max)(labelWidth, e.Label.size());
        }
    }

    std::ostringstream summary;
    summary << "\n==================== Perf.GLBAnimation summary ====================\n";
    {
        std::scoped_lock lock{perfMutex};
        for (const auto& e : perfEntries)
        {
            summary << "  " << std::left << std::setw(static_cast<int>(labelWidth)) << e.Label
                    << "  " << FormatMs(e.Ms) << "\n";
        }
    }
    summary << "-------------------------------------------------------------------\n";
    summary << "  " << std::left << std::setw(static_cast<int>(labelWidth)) << "[C++]    TOTAL wall time"
            << "  " << FormatMs(totalMs) << "\n";
    summary << "===================================================================\n";

    std::cout << summary.str() << std::endl;

    EXPECT_FALSE(failed.load()) << "JS-side benchmark failed: " << failReason;
}
