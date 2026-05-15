#pragma once

namespace Babylon::Integrations
{
    // Severity levels for the optional log callback on RuntimeOptions.
    //
    // The first three (Log / Warn / Error) mirror
    // `Babylon::Polyfills::Console::LogLevel` and are used for
    // `console.log` / `console.warn` / `console.error` calls and for
    // `Babylon::DebugTrace` output.
    //
    // `Fatal` is used for **uncaught** JavaScript exceptions that
    // propagated past every JS-side handler. The engine state may be
    // inconsistent after a Fatal; a host that wants to terminate the
    // process on uncaught errors can do so from inside its log
    // callback (e.g. `if (level == LogLevel::Fatal) std::quick_exit(1);`).
    enum class LogLevel
    {
        Log,
        Warn,
        Error,
        Fatal,
    };
}