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

    // Queue the standard Babylon.js bootstrap scripts (core, loaders,
    // materials, GUI, serializers, etc.) onto `runtime` in dependency order.
    //
    // The `Babylon::Integrations` layer doesn't bundle script loading;
    // each host picks between this multi-UMD route and a pre-bundled
    // `bundle.js` route. Centralizing the list keeps every Playground
    // host in sync as the bundle list evolves.
    //
    // LoadScript calls made before the first View attach are queued and
    // dispatched after engine init, so this is safe to call immediately
    // after constructing the Runtime.
    void LoadBootstrapScripts(Babylon::Integrations::Runtime& runtime);
}
