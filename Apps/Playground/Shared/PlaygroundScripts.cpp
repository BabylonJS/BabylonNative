#include "PlaygroundScripts.h"

#include <Babylon/Integrations/Runtime.h>
#include <Babylon/PerfTrace.h>

namespace Playground
{
    void Initialize()
    {
        // Process-wide perf-tracing configuration. Used to be done
        // inside AppContext's constructor.
        Babylon::PerfTrace::SetLevel(Babylon::PerfTrace::Level::Mark);
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
