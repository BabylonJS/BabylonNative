#pragma once

namespace Babylon::Integrations
{
    // Severity levels for the optional log callback on RuntimeOptions.
    // Mirrors the levels used by `Babylon::Polyfills::Console::LogLevel`
    // but is exposed as its own enum so consumers don't have to depend
    // on the Console polyfill header just to read log output.
    enum class LogLevel
    {
        Log,
        Warn,
        Error,
    };
}
