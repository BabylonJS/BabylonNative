#include "PlaygroundScripts.h"

#include <Babylon/Integrations/Runtime.h>
#include <Babylon/PerfTrace.h>

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
}
