#pragma once

namespace Babylon::Integrations
{
    // Severity levels for the optional log callback on RuntimeOptions.
    //
    // `Verbose` is used for low-priority diagnostic output: currently
    // `Babylon::DebugTrace` messages, which are produced only when the
    // host opts in via `RuntimeOptions::enableDebugTrace`. Hosts that
    // want a quieter log can filter on `level > LogLevel::Verbose`.
    //
    // The next three (Log / Warn / Error) mirror
    // `Babylon::Polyfills::Console::LogLevel` and are used for
    // `console.log` / `console.warn` / `console.error` calls.
    //
    // `Fatal` is used for **uncaught** JavaScript exceptions that
    // propagated past every JS-side handler. The engine state may be
    // inconsistent after a Fatal; a host that wants to terminate the
    // process on uncaught errors can do so from inside its log
    // callback (e.g. `if (level == LogLevel::Fatal) std::quick_exit(1);`).
    //
    // Values are ordered by increasing severity so hosts can compare
    // them (`level >= LogLevel::Warn` etc.) for level-threshold
    // filtering.
    enum class LogLevel
    {
        Verbose,
        Log,
        Warn,
        Error,
        Fatal,
    };
}