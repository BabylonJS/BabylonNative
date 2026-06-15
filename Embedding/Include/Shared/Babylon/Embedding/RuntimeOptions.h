#pragma once

#include <Babylon/Embedding/LogLevel.h>

#include <cstdint>
#include <functional>
#include <string>
#include <string_view>

namespace Babylon::Embedding
{
    struct RuntimeOptions
    {
        // MSAA sample count for the back buffer. Valid values: 0, 2, 4, 8, 16.
        // Anything else disables MSAA.
        uint8_t msaaSamples{4};

        // Enable the JavaScript debugger. Only implemented for V8 and Chakra.
        bool enableDebugger{false};

        // Enable Babylon::DebugTrace. If a log sink is provided, DebugTrace
        // output is forwarded to it as LogLevel::Verbose.
        bool enableDebugTrace{false};

        // Block engine startup until a debugger has attached. Only
        // implemented for V8.
        bool waitForDebugger{false};

        // Optional log sink. Receives:
        //   - `console.{log,warn,error}`     → LogLevel::{Log,Warn,Error}
        //   - `Babylon::DebugTrace` output    → LogLevel::Verbose (when
        //                                       enableDebugTrace is true)
        //   - Uncaught JS exceptions          → LogLevel::Fatal
        //
        // For Error and Fatal messages, a JS callstack is appended to the
        // message body when the underlying JS engine can produce one. This
        // is best-effort and silently omitted on engines / contexts that
        // can't (the message body just won't contain a "JS callstack:" /
        // "Stack:" trailer).
        //
        // If unset, log output is discarded and uncaught exceptions fall
        // back to `Babylon::AppRuntime::DefaultUnhandledExceptionHandler`.
        // Hosts that want process termination on uncaught exceptions can do
        // so from this callback, e.g. `if (level == Fatal) std::quick_exit(1);`.
        std::function<void(LogLevel, std::string_view)> log;

#if BABYLON_NATIVE_PLUGIN_SHADERCACHE
        // Optional path for persisting the GPU shader cache across sessions.
        // If non-empty:
        //   - Loaded synchronously during the first View attach (missing
        //     or unreadable file: ignored).
        //   - Saved synchronously during `Runtime::Suspend` (after the
        //     in-flight frame is closed, when the engine is quiescent).
        //   - Saved synchronously during `~Runtime`.
        std::string shaderCachePath;
#endif
    };
}