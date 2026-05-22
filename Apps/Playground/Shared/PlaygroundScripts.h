#pragma once

#include "CommandLine.h"

#include <Babylon/Integrations/LogLevel.h>

#include <functional>
#include <string_view>

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

    // Build a `RuntimeOptions::log` callback that:
    //   1) Trims trailing newlines from `message` and forwards it to
    //      `platformSink` so the host can route to OutputDebugStringA,
    //      NSLog, __android_log_print, stdout, etc. `platformSink` is
    //      called for every level (including Verbose / Log / Warn).
    //   2) On `LogLevel::Error`: emits a `JS CONSOLE ERROR` banner via
    //      `Diagnostics::DumpFailure` (banner header + native callstack +
    //      build info). The JS callstack is already appended by the
    //      Integrations layer, so it lands inside the banner body.
    //   3) On `LogLevel::Fatal`: emits an `UNCAUGHT JS ERROR` banner,
    //      sets exit code 1, prints the finish line, and `std::quick_exit`s.
    //
    // Centralizing this matches the behavior every Playground host had
    // pre-Integrations via `AppContext` (which routed `console.error` and
    // the unhandled exception handler through `DumpFailure`).
    std::function<void(Babylon::Integrations::LogLevel, std::string_view)>
    MakeLogCallback(std::function<void(std::string_view)> platformSink);
}
