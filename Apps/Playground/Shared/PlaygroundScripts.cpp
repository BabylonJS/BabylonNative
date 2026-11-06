#include "PlaygroundScripts.h"

#include "Diagnostics.h"

#include <Babylon/Embedding/Runtime.h>
#include <Babylon/PerfTrace.h>

#include <napi/napi.h>

#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <mutex>
#include <string>
#include <thread>
#include <utility>

namespace
{
    // Fails the run when the runner stops logging. JavaScript timers can't
    // catch a native hang (e.g. a busy loop on the render thread), which
    // otherwise runs until the CI job timeout.
    class Watchdog
    {
    public:
        static Watchdog& Instance()
        {
            // Leaked: the detached thread may outlive static destruction.
            static Watchdog* instance = new Watchdog{};
            return *instance;
        }

        // Called when the host installs MakeLogCallback, the only path that feeds the watchdog.
        void EnableFeeding()
        {
            m_feedable = true;
        }

        void Start(std::chrono::seconds limit)
        {
            if (!m_feedable)
            {
                // Hosts that route console output elsewhere (e.g. the windowed
                // macOS BNRuntime -> os_log) would trip the watchdog while healthy.
                std::fprintf(stderr, "Playground: --test-timeout hang watchdog disabled; this host does not feed it.\n");
                return;
            }
            bool expected = false;
            if (!m_started.compare_exchange_strong(expected, true))
            {
                return;
            }
            m_limit = limit;
            Feed({});
            std::thread{[this] { Run(); }}.detach();
        }

        void Feed(std::string_view line)
        {
            std::lock_guard<std::mutex> lock{m_mutex};
            m_lastProgress = std::chrono::steady_clock::now();
            m_lastLine.assign(line.substr(0, 512));
        }

    private:
        void Run()
        {
            for (;;)
            {
                std::this_thread::sleep_for(std::chrono::seconds{5});
                std::string lastLine;
                std::chrono::steady_clock::duration idle;
                {
                    std::lock_guard<std::mutex> lock{m_mutex};
                    idle = std::chrono::steady_clock::now() - m_lastProgress;
                    lastLine = m_lastLine;
                }
                if (idle >= m_limit)
                {
                    Diagnostics::DumpFailure(
                        "HANG",
                        nullptr,
                        0,
                        0,
                        "No console output for %llds. Last line: %s",
                        static_cast<long long>(std::chrono::duration_cast<std::chrono::seconds>(idle).count()),
                        lastLine.c_str());
                    Diagnostics::SetExitCode(3);
                    Diagnostics::PrintFinishLine();
                    std::_Exit(3);
                }
            }
        }

        std::atomic<bool> m_feedable{false};
        std::atomic<bool> m_started{false};
        std::chrono::seconds m_limit{};
        std::mutex m_mutex;
        std::chrono::steady_clock::time_point m_lastProgress{};
        std::string m_lastLine;
    };
}

namespace Playground
{
    void Initialize(const PlaygroundOptions& options)
    {
        // Process-wide perf-tracing configuration.
        Babylon::PerfTrace::Level perfLevel{Babylon::PerfTrace::Level::Mark};
        if (options.PerfTrace.has_value())
        {
            const auto& value = *options.PerfTrace;
            if (value == "None" || value == "none")
            {
                perfLevel = Babylon::PerfTrace::Level::None;
            }
            else if (value == "Log" || value == "log" || value == "Detail" || value == "detail")
            {
                perfLevel = Babylon::PerfTrace::Level::Log;
            }
        }
        Babylon::PerfTrace::SetLevel(perfLevel);

        if (options.TestTimeoutSeconds.has_value())
        {
            // createScene and initial readiness each get the timeout, back to back.
            Watchdog::Instance().Start(std::chrono::seconds{2 * *options.TestTimeoutSeconds + 60});
        }
    }

    void QueueOptions(Babylon::Embedding::Runtime& runtime, const PlaygroundOptions& options)
    {
        runtime.RunOnJsThread([options](Napi::Env env) {
            auto js = Napi::Object::New(env);
            js.Set("listTests", Napi::Boolean::New(env, options.ListTests));
            js.Set("headless", Napi::Boolean::New(env, options.Headless));
            js.Set("breakOnFail", Napi::Boolean::New(env, options.BreakOnFail));
            js.Set("stopOnFirstFailure", Napi::Boolean::New(env, options.StopOnFirstFailure));
            js.Set("generateReferences", Napi::Boolean::New(env, options.GenerateReferences));
            js.Set("runOnce", Napi::Boolean::New(env, options.RunOnce));
            js.Set("includeExcluded", Napi::Boolean::New(env, options.IncludeExcluded));
            if (options.SaveResults.has_value())
            {
                js.Set("saveResults", Napi::Boolean::New(env, *options.SaveResults));
            }
            if (options.CaptureFrame.has_value())
            {
                js.Set("captureFrame", Napi::Number::New(env, *options.CaptureFrame));
            }
            if (options.TestTimeoutSeconds.has_value())
            {
                js.Set("testTimeoutMs", Napi::Number::New(env, *options.TestTimeoutSeconds * 1000.0));
            }

            auto filters = Napi::Array::New(env, options.TestFilters.size());
            for (uint32_t index = 0; index < options.TestFilters.size(); ++index)
            {
                filters[index] = Napi::String::New(env, options.TestFilters[index]);
            }
            js.Set("testFilters", filters);

            auto indices = Napi::Array::New(env, options.TestIndices.size());
            for (uint32_t index = 0; index < options.TestIndices.size(); ++index)
            {
                indices[index] = Napi::Number::New(env, options.TestIndices[index]);
            }
            js.Set("testIndices", indices);
            env.Global().Set("_playgroundOptions", js);
        });
    }

    void LoadBootstrapScripts(Babylon::Embedding::Runtime& runtime)
    {
        // UMD bundles require these globals before loading.
        runtime.LoadScript("app:///Scripts/native_env.js");
        runtime.LoadScript("app:///Scripts/ammo.js");
        // Commenting out recast.js for now because v8jsi is incompatible with asm.js.
        // runtime.LoadScript("app:///Scripts/recast.js");
        // PolygonMeshBuilder resolves `earcut` as a global at triangulation time, so without this
        // any polygon scene fails with `ReferenceError: earcut is not defined`.
        runtime.LoadScript("app:///Scripts/earcut.min.js");
        runtime.LoadScript("app:///Scripts/babylon.es5.js");
        // Load addons right after babylon.es5.js so addons init sees a fully
        // constructed BABYLON global.
        runtime.LoadScript("app:///Scripts/babylonjs.addons.es5.js");
        runtime.LoadScript("app:///Scripts/babylonjs.loaders.es5.js");
        runtime.LoadScript("app:///Scripts/babylonjs.materials.es5.js");
        // Procedural-textures library (Fire/Wood/Marble/... ProceduralTexture).
        runtime.LoadScript("app:///Scripts/babylonjs.proceduralTextures.es5.js");
        runtime.LoadScript("app:///Scripts/babylon.gui.es5.js");
        runtime.LoadScript("app:///Scripts/meshwriter.min.js");
        runtime.LoadScript("app:///Scripts/babylonjs.serializers.es5.js");
    }

    std::function<void(Babylon::Embedding::LogLevel, std::string_view)>
    MakeLogCallback(std::function<void(std::string_view)> platformSink)
    {
        Watchdog::Instance().EnableFeeding();
        return [sink = std::move(platformSink)](Babylon::Embedding::LogLevel level, std::string_view message) {
            std::string text{message};
            while (!text.empty() && (text.back() == '\n' || text.back() == '\r'))
            {
                text.pop_back();
            }

            if (sink)
            {
                sink(text);
            }

            // DebugTrace (Verbose) can come from non-JS threads; only runner output counts as progress.
            if (level != Babylon::Embedding::LogLevel::Verbose)
            {
                Watchdog::Instance().Feed(text);
            }

            // Babylon.js routes recoverable errors through console.error;
            // surface them as a grep-able banner with a native callstack.
            if (level == Babylon::Embedding::LogLevel::Error)
            {
                Diagnostics::DumpFailure(
                    "JS CONSOLE ERROR",
                    nullptr,
                    0,
                    0,
                    "%s",
                    text.c_str());
            }

            // Uncaught JS exceptions: banner + finish-line + non-zero exit.
            // The Embedding UnhandledExceptionHandler has already formatted
            // the message as "[Uncaught Error] <message + JS stack>".
            if (level == Babylon::Embedding::LogLevel::Fatal)
            {
                Diagnostics::DumpFailure(
                    "UNCAUGHT JS ERROR",
                    nullptr,
                    0,
                    0,
                    "%s",
                    text.c_str());
                Diagnostics::SetExitCode(1);
                Diagnostics::PrintFinishLine();
                std::quick_exit(1);
            }
        };
    }
}
