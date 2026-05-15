#pragma once

#include "CommandLine.h"

namespace Babylon::Integrations
{
    class Runtime;
}

namespace Playground
{
    // Apply process-wide settings shared by every Playground host:
    // currently PerfTrace. Call before queuing scripts and attaching
    // the first view.
    void Initialize(const PlaygroundOptions& options = {});

    // Queue the standard Babylon.js bootstrap scripts (Babylon core,
    // loaders, materials, GUI, serializers, plus a few common extras)
    // onto `runtime` in dependency order.
    //
    // These were historically loaded by `AppContext`'s constructor; the
    // `Babylon::Integrations` layer no longer bundles script loading
    // (each host decides between the multi-UMD route this helper
    // implements and a single pre-bundled `bundle.js` route — see
    // SimplifiedAPI.md §4.1 "Loading Babylon.js: two supported routes").
    // We keep the list here so every Playground host stays in sync as
    // the bundle list evolves.
    //
    // Calls to `LoadScript` made before the first `View::Attach` are
    // queued on the runtime and dispatched after engine initialization
    // completes; this helper relies on that, so it's safe to call
    // immediately after `Runtime::Create`.
    void LoadBootstrapScripts(Babylon::Integrations::Runtime& runtime);
}
