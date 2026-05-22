#include "PlaygroundScripts.h"

#include "Diagnostics.h"

#include <Babylon/Integrations/Runtime.h>
#include <Babylon/PerfTrace.h>

#include <cstdlib>
#include <string>
#include <utility>

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
    }

    void LoadBootstrapScripts(Babylon::Integrations::Runtime& runtime)
    {
        runtime.LoadScript("app:///Scripts/ammo.js");
        // Commenting out recast.js for now because v8jsi is incompatible with asm.js.
        // runtime.LoadScript("app:///Scripts/recast.js");
        runtime.LoadScript("app:///Scripts/babylon.max.js");
        runtime.LoadScript("app:///Scripts/babylonjs.loaders.js");
        runtime.LoadScript("app:///Scripts/babylonjs.materials.js");
        runtime.LoadScript("app:///Scripts/babylon.gui.js");
        runtime.LoadScript("app:///Scripts/meshwriter.min.js");
        runtime.LoadScript("app:///Scripts/babylonjs.serializers.js");
    }

    std::function<void(Babylon::Integrations::LogLevel, std::string_view)>
    MakeLogCallback(std::function<void(std::string_view)> platformSink)
    {
        return [sink = std::move(platformSink)](Babylon::Integrations::LogLevel level, std::string_view message) {
            std::string text{message};
            while (!text.empty() && (text.back() == '\n' || text.back() == '\r'))
            {
                text.pop_back();
            }

            if (sink)
            {
                sink(text);
            }

            // Babylon.js routes recoverable errors through console.error;
            // surface them as a grep-able banner with a native callstack.
            if (level == Babylon::Integrations::LogLevel::Error)
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
            // The Integrations UnhandledExceptionHandler has already formatted
            // the message as "[Uncaught Error] <message + JS stack>".
            if (level == Babylon::Integrations::LogLevel::Fatal)
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
