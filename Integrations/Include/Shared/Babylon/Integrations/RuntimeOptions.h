#pragma once

#include <Babylon/Integrations/LogLevel.h>

#include <cstdint>
#include <functional>
#include <string>
#include <string_view>

namespace Babylon::Integrations
{
    struct RuntimeOptions
    {
        // MSAA sample count for the back buffer. Valid values: 0, 2, 4, 8, 16.
        // Anything else disables MSAA.
        uint8_t msaaSamples{4};

        // Enable the JavaScript debugger. Only implemented for V8 and Chakra.
        bool enableDebugger{false};

        // Enable Babylon::DebugTrace. If a log sink is provided, DebugTrace
        // output is forwarded to it as LogLevel::Log.
        bool enableDebugTrace{false};

        // Block engine startup until a debugger has attached. Only
        // implemented for V8.
        bool waitForDebugger{false};

        // Optional log sink. Receives:
        //   - `console.{log,warn,error}` output  → LogLevel::{Log,Warn,Error}
        //   - `Babylon::DebugTrace` output      → LogLevel::Log, when
        //                                         enableDebugTrace is true
        //   - Uncaught JS exceptions             → LogLevel::Fatal
        //
        // If unset, ordinary log output is silently discarded and
        // uncaught exceptions fall back to
        // `Babylon::AppRuntime::DefaultUnhandledExceptionHandler`
        // (which writes to the program output).
        //
        // Hosts that want process termination on uncaught exceptions
        // (matching the historical AppContext behavior) can do so from
        // inside this callback, e.g.
        //
        //     if (level == LogLevel::Fatal) std::quick_exit(1);
        std::function<void(LogLevel, std::string_view)> log;

#if BABYLON_NATIVE_PLUGIN_SHADERCACHE
        // Optional path for persisting the GPU shader cache across
        // sessions. If non-empty:
        //   - Loaded synchronously during the first `View::Attach` (after
        //     `ShaderCache::Enable`). Missing or unreadable file: ignored.
        //   - Saved asynchronously during `Runtime::Suspend` (queued onto
        //     the JS thread before the suspension blocker) so the
        //     on-disk cache reflects any shaders compiled this session.
        //   - Saved synchronously during `~Runtime` so a final write
        //     happens before the JS thread is torn down.
        std::string shaderCachePath;
#endif
    };
}