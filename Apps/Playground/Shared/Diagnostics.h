#pragma once

#include <cstdarg>

namespace Diagnostics
{
    // Process-wide crash + assert handler that produces a callstack on stderr
    // and exits with code 3 on hard failures. Idempotent. On Windows: also
    // suppresses UCRT/abort/invalid-parameter modal dialogs and only breaks
    // when a debugger is attached (otherwise EXCEPTION_BREAKPOINT corrupts the
    // exit code).
    void InstallCrashHandler();

    // Captures start time, enables ANSI on Windows console, installs the crash
    // handler, registers atexit/at_quick_exit hooks for the finish line.
    // Idempotent. Call once at the top of main / wWinMain.
    void Initialize();

    // Records the exit code that the next finish-line will reflect. Call
    // before any std::quick_exit / _Exit / return from main.
    void SetExitCode(int code);

    // Prints "Playground: Finished in <time>. (exit <code>)" colored by exit
    // code. Idempotent (only first call produces output).
    void PrintFinishLine();

    // Records that forward progress was made (e.g. a test produced output).
    // Resets the stall watchdog. Cheap and lock-free; safe to call from any
    // thread. No-op unless the watchdog is enabled (sanitizer builds, or
    // BN_WATCHDOG_SECONDS set).
    void NotifyProgress();

    // Emits a bx-style banner with native callstack and build info to both
    // stderr and bx::getDebugOut(). Format:
    //
    //     --- BN: <CATEGORY> ---
    //
    //     <file>(<line>): <message>
    //
    //     Callstack (N):
    //          0: ...
    //
    //     Build info: ...
    //
    //     --- END ---
    //
    // Pass file=nullptr / line=0 to omit the source-location prefix.
    // skipFrames is added on top of the helpers internal to DumpFailure.
    void DumpFailure(const char* category, const char* file, int line, unsigned int skipFrames, const char* fmt, ...);
    void DumpFailureV(const char* category, const char* file, int line, unsigned int skipFrames, const char* fmt, va_list args);
}