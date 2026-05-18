#pragma once

namespace Babylon::Integrations
{
    // Severity levels for the RuntimeOptions log callback. Ordered by
    // increasing severity so hosts can do `level >= LogLevel::Warn` filtering.
    //
    //   Verbose : `Babylon::DebugTrace` messages (only when
    //             `enableDebugTrace` is set).
    //   Log/Warn/Error : `console.log/warn/error`. Mirrors
    //             `Babylon::Polyfills::Console::LogLevel`.
    //   Fatal   : uncaught JS exceptions that escaped every JS-side handler.
    //             Engine state may be inconsistent after a Fatal; hosts can
    //             terminate from the callback if desired.
    enum class LogLevel
    {
        Verbose,
        Log,
        Warn,
        Error,
        Fatal,
    };
}