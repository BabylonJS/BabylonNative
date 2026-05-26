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
#include <Babylon/Plugins/TestUtils.h>
#include <Babylon/ScriptLoader.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstring>
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

#if defined(_WIN32)
    // <windows.h> is already pulled in transitively by gtest on Windows; <psapi.h> brings in
    // K32GetProcessMemoryInfo and PROCESS_MEMORY_COUNTERS_EX. K32* is resident in kernel32.dll
    // so no extra link library is required (unlike the non-K32 GetProcessMemoryInfo which
    // wants psapi.lib on older SDKs).
    #include <psapi.h>
#elif defined(__APPLE__)
    #include <mach/mach.h>
#endif

namespace
{
    using PerfClock = std::chrono::steady_clock;
    using ByteCount = std::int64_t;

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

    // Process-level private byte counter. On Windows this is PrivateUsage (commit charge for
    // memory exclusively owned by this process) which captures every native-side allocator
    // path: malloc, HeapAlloc, VirtualAlloc, and whatever the JS engine uses internally
    // (Chakra/V8/QuickJS/JSC all ultimately route to one of those). This is the right metric
    // for cross-engine comparison because it does not depend on which allocator the engine
    // chose. Returns 0 when unavailable (e.g. unsupported platform or syscall failure).
    ByteCount QueryPrivateBytes()
    {
#if defined(_WIN32)
        PROCESS_MEMORY_COUNTERS_EX pmc{};
        pmc.cb = sizeof(pmc);
        if (::K32GetProcessMemoryInfo(::GetCurrentProcess(), reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&pmc), sizeof(pmc)))
        {
            return static_cast<ByteCount>(pmc.PrivateUsage);
        }
        return 0;
#elif defined(__APPLE__)
        task_vm_info_data_t info{};
        mach_msg_type_number_t count = TASK_VM_INFO_COUNT;
        if (task_info(mach_task_self(), TASK_VM_INFO, reinterpret_cast<task_info_t>(&info), &count) == KERN_SUCCESS)
        {
            // phys_footprint matches Activity Monitor's "Memory" column.
            return static_cast<ByteCount>(info.phys_footprint);
        }
        return 0;
#elif defined(__linux__) || defined(__ANDROID__)
        // VmData = anonymous + heap + stack pages, which is the closest equivalent to
        // Windows PrivateUsage. VmRSS would also work but is sensitive to OS reclaim.
        std::ifstream status{"/proc/self/status"};
        std::string line;
        while (std::getline(status, line))
        {
            if (line.rfind("VmData:", 0) == 0)
            {
                std::istringstream iss{line.substr(7)};
                std::int64_t kb{};
                std::string unit;
                iss >> kb >> unit;
                return kb * 1024;
            }
        }
        return 0;
#else
        return 0;
#endif
    }

    std::string FormatBytesDelta(ByteCount bytes)
    {
        const char sign = bytes < 0 ? '-' : '+';
        const double abs = static_cast<double>(bytes < 0 ? -bytes : bytes);
        const char* unit = "B";
        double scaled = abs;
        if (abs >= 1024.0 * 1024.0 * 1024.0) { scaled = abs / (1024.0 * 1024.0 * 1024.0); unit = "GB"; }
        else if (abs >= 1024.0 * 1024.0)     { scaled = abs / (1024.0 * 1024.0);          unit = "MB"; }
        else if (abs >= 1024.0)              { scaled = abs / 1024.0;                     unit = "KB"; }
        std::ostringstream oss;
        oss << sign;
        if (std::strcmp(unit, "B") == 0)
        {
            oss << static_cast<long long>(scaled) << " " << unit;
        }
        else
        {
            oss << std::fixed << std::setprecision(2) << scaled << " " << unit;
        }
        return oss.str();
    }

    struct PerfEntry
    {
        std::string Label;
        double Ms;
        ByteCount DeltaBytes;
        bool HasMemDelta;
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
    const auto totalStart = PerfClock::now();

    std::mutex perfMutex;
    std::vector<PerfEntry> perfEntries;
    auto logPerf = [&](std::string label, double ms, ByteCount deltaBytes, bool hasMemDelta) {
        std::scoped_lock lock{perfMutex};
        perfEntries.push_back({std::move(label), ms, deltaBytes, hasMemDelta});
    };

    // Noop renderer: no window, no width/height, no device. bgfx will pick its noop backend.
    // CPU-side rendering still runs (encoders, draw call submission); no swapchain => no vsync.
    Babylon::Graphics::Configuration config{};

    const auto deviceMem0 = QueryPrivateBytes();
    const auto deviceStart = PerfClock::now();
    Babylon::Graphics::Device device{config};
    logPerf("[C++]    Graphics::Device ctor",
            DurationMs(PerfClock::now() - deviceStart),
            QueryPrivateBytes() - deviceMem0,
            true);

    Babylon::AppRuntime::Options options{};
    options.UnhandledExceptionHandler = [](const Napi::Error& error) {
        std::cerr << "[Perf.GLBAnimation] [Uncaught Error] " << Napi::GetErrorString(error) << std::endl;
        std::quick_exit(1);
    };

    const auto runtimeMem0 = QueryPrivateBytes();
    const auto runtimeStart = PerfClock::now();
    Babylon::AppRuntime runtime{options};
    logPerf("[C++]    AppRuntime ctor",
            DurationMs(PerfClock::now() - runtimeStart),
            QueryPrivateBytes() - runtimeMem0,
            true);

    std::promise<void> donePromise;
    std::atomic<bool> failed{false};
    std::string failReason;

    // Initialize polyfills + plugins and install perf hooks. Each Initialize() call is timed
    // and memory-snapshotted independently on the JS thread, all from within a single
    // dispatch round-trip so we don't pollute numbers with per-call dispatch latency.
    const auto pluginsDispatchMem0 = QueryPrivateBytes();
    const auto pluginsDispatchStart = PerfClock::now();
    std::promise<void> pluginsReadyPromise;
    const auto perfNowBaseline = PerfClock::now();

    runtime.Dispatch([&](Napi::Env env) {
        auto stepStart = PerfClock::now();
        auto stepMem0 = QueryPrivateBytes();
        auto closeStep = [&](const char* label) {
            const auto now = PerfClock::now();
            const auto mem1 = QueryPrivateBytes();
            logPerf(label, DurationMs(now - stepStart), mem1 - stepMem0, true);
            stepStart = now;
            stepMem0 = mem1;
        };

        device.AddToJavaScript(env);
        closeStep("[init]   device.AddToJavaScript");

        Babylon::Polyfills::XMLHttpRequest::Initialize(env);
        closeStep("[init]   Polyfills::XMLHttpRequest");

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
        closeStep("[init]   Polyfills::Console");

        Babylon::Polyfills::Window::Initialize(env);
        closeStep("[init]   Polyfills::Window");

        Babylon::Plugins::NativeEngine::Initialize(env);
        closeStep("[init]   Plugins::NativeEngine");

        // TestUtils is used here only for its filesystem-listing API (listFiles). The window
        // handle is unused by listFiles, so we pass a default-constructed Graphics::WindowT --
        // the noop renderer never produces a real window anyway. Any TestUtils method that
        // actually needs the window (Exit, UpdateSize, SetTitle) is not called from the
        // benchmark.
        Babylon::Plugins::TestUtils::Initialize(env, Babylon::Graphics::WindowT{});
        closeStep("[init]   Plugins::TestUtils");

        // High-resolution clock for JS-side timings. Returns milliseconds (double) since the
        // baseline taken just before this dispatch, with sub-ms resolution.
        env.Global().Set("_perfNow", Napi::Function::New(env,
            [perfNowBaseline](const Napi::CallbackInfo& info) -> Napi::Value {
                const auto delta = PerfClock::now() - perfNowBaseline;
                return Napi::Number::New(info.Env(), DurationMs(delta));
            },
            "_perfNow"));

        // Process-level private byte counter (PrivateUsage on Win32, phys_footprint on macOS,
        // VmData on Linux/Android). Returns a Number (bytes). Cross-engine comparable since
        // it does not depend on which allocator the JS engine uses internally.
        env.Global().Set("_memBytes", Napi::Function::New(env,
            [](const Napi::CallbackInfo& info) -> Napi::Value {
                return Napi::Number::New(info.Env(), static_cast<double>(QueryPrivateBytes()));
            },
            "_memBytes"));

        env.Global().Set("_perfReport", Napi::Function::New(env,
            [&logPerf](const Napi::CallbackInfo& info) {
                auto label = info[0].As<Napi::String>().Utf8Value();
                const auto ms = info[1].As<Napi::Number>().DoubleValue();
                if (info.Length() >= 3 && info[2].IsNumber())
                {
                    const auto deltaBytes = static_cast<ByteCount>(info[2].As<Napi::Number>().DoubleValue());
                    logPerf(std::move(label), ms, deltaBytes, true);
                }
                else
                {
                    logPerf(std::move(label), ms, 0, false);
                }
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
    logPerf("[C++]    plugin-init dispatch round-trip (total)",
            DurationMs(PerfClock::now() - pluginsDispatchStart),
            QueryPrivateBytes() - pluginsDispatchMem0,
            true);

    Babylon::ScriptLoader loader{runtime};
    // Mocha-style globals expect `location.href`; not strictly needed here, but harmless.
    loader.Eval("location = { href: '' };", "");

    const auto assetsDir = GetExecutableDirectory() / "Assets";

    // Load a script split into two timings: file read (fetch) and JS eval (dispatch + Napi::Eval
    // round-trip). The latter is measured by waiting for a subsequent loader.Dispatch to settle.
    auto loadFetchAndEval = [&](const char* fileName, const char* urlForEval) {
        const auto fetchMem0 = QueryPrivateBytes();
        const auto fetchStart = PerfClock::now();
        const std::string source = ReadAssetText(assetsDir / fileName);
        const double fetchMs = DurationMs(PerfClock::now() - fetchStart);
        const auto fetchMemDelta = QueryPrivateBytes() - fetchMem0;
        logPerf(std::string{"[load]   "} + fileName + " - fetch (ifstream)",
                fetchMs, fetchMemDelta, true);

        const auto evalMem0 = QueryPrivateBytes();
        const auto evalStart = PerfClock::now();
        std::promise<void> p;
        loader.Eval(source, urlForEval);
        loader.Dispatch([&p](Napi::Env) { p.set_value(); });
        p.get_future().get();
        const double evalMs = DurationMs(PerfClock::now() - evalStart);
        const auto evalMemDelta = QueryPrivateBytes() - evalMem0;
        logPerf(std::string{"[load]   "} + fileName + " - eval",
                evalMs, evalMemDelta, true);
    };

    loadFetchAndEval("babylon.max.js", "app:///Assets/babylon.max.js");
    loadFetchAndEval("babylonjs.loaders.js", "app:///Assets/babylonjs.loaders.js");
    loadFetchAndEval("tests.perf.glbAnimation.js", "app:///Assets/tests.perf.glbAnimation.js");

    // Drive the bgfx pipeline so NativeEngine's requestAnimationFrame callbacks fire. The
    // benchmark JS posts back via _perfDone(); we busy-wait on it while ticking the device.
    const auto renderMem0 = QueryPrivateBytes();
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
    logPerf("[C++]    render loop wall time",
            DurationMs(PerfClock::now() - renderStart),
            QueryPrivateBytes() - renderMem0,
            true);

    const double totalMs = DurationMs(PerfClock::now() - totalStart);

    // Determine column widths so the summary lines up regardless of label length.
    std::size_t labelWidth = 0;
    std::size_t msWidth = 0;
    std::size_t memWidth = 0;
    {
        std::scoped_lock lock{perfMutex};
        for (const auto& e : perfEntries)
        {
            // Parenthesized to suppress the Windows <windows.h> `max` macro expansion.
            labelWidth = (std::max)(labelWidth, e.Label.size());
            msWidth = (std::max)(msWidth, FormatMs(e.Ms).size());
            if (e.HasMemDelta)
            {
                memWidth = (std::max)(memWidth, FormatBytesDelta(e.DeltaBytes).size());
            }
        }
    }

    std::ostringstream summary;
    summary << "\n==================== Perf.GLBAnimation summary ====================\n";
    {
        std::scoped_lock lock{perfMutex};
        for (const auto& e : perfEntries)
        {
            summary << "  " << std::left << std::setw(static_cast<int>(labelWidth)) << e.Label
                    << "  " << std::right << std::setw(static_cast<int>(msWidth)) << FormatMs(e.Ms);
            if (e.HasMemDelta && memWidth > 0)
            {
                summary << "   mem=" << std::right << std::setw(static_cast<int>(memWidth))
                        << FormatBytesDelta(e.DeltaBytes);
            }
            summary << "\n";
        }
    }
    summary << "-------------------------------------------------------------------\n";
    summary << "  " << std::left << std::setw(static_cast<int>(labelWidth)) << "[C++]    TOTAL wall time"
            << "  " << std::right << std::setw(static_cast<int>(msWidth)) << FormatMs(totalMs) << "\n";
    summary << "===================================================================\n";

    std::cout << summary.str() << std::endl;

    EXPECT_FALSE(failed.load()) << "JS-side benchmark failed: " << failReason;
}
