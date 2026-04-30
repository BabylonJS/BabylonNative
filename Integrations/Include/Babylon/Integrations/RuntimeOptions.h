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

        // Block engine startup until a debugger has attached. Only
        // implemented for V8.
        bool waitForDebugger{false};

        // Optional log sink. If unset, log output is discarded. Wired into
        // both `Babylon::DebugTrace` and the Console polyfill.
        std::function<void(LogLevel, std::string_view)> log;

        // Optional handler for unhandled JavaScript exceptions. If unset,
        // `Babylon::AppRuntime::DefaultUnhandledExceptionHandler` is used
        // (which writes the error to the program output).
        std::function<void(std::string_view message)> onUnhandledError;
    };
}
