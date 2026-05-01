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

    // Resolves and preloads renderdoc.dll before bgfx initializes, then prints
    // the loaded module's path + API/file version. Resolution order:
    //   1. explicitDllPath (--renderdoc-dll). Always preloaded if non-empty.
    //   2. BN_RENDERDOC_DLL env var (only if captureRequested).
    //   3. RENDERDOC_PYTHON_PATH env var, "/renderdoc.dll" appended (only if
    //      captureRequested). Pairs with rdc-cli's expected version.
    //   4. LoadLibrary("renderdoc.dll") via PATH (only if captureRequested).
    //
    // If a different module is already loaded when an explicit path was given,
    // emits a hard error and returns without altering the existing module.
    // Returns true if a renderdoc.dll is present in the process. Idempotent.
    // No-op on non-Windows.
    bool SetupRenderDoc(const char* explicitDllPath, bool captureRequested);
}