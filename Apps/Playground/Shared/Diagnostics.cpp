#include "Diagnostics.h"

#include <bx/debug.h>
#include <bx/error.h>
#include <bx/platform.h>
#include <bx/readerwriter.h>

#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <thread>

#if defined(_MSC_VER)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <crtdbg.h>
#include <stdlib.h>
#include <io.h>
#include <wchar.h>
#include <intrin.h>
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")
#else
#include <unistd.h>
#endif

namespace
{
    std::atomic<bool> s_installed{false};
    std::atomic<bool> s_initialized{false};
    std::atomic<bool> s_finishPrinted{false};
    std::atomic<int>  s_exitCode{0};

    // Process start time (zero until Initialize()).
    std::chrono::steady_clock::time_point s_startTime{};

    bool s_ansiEnabled{false};

    // Stall watchdog: forces an external full-process crash dump if the app
    // makes no forward progress for a configurable interval. Built to capture
    // the Win32 ASAN "EXR Loader" stall, where an allocator-lock deadlock
    // freezes the test runner (and any in-process diagnostic that allocates)
    // silently until the 60-min CI job times out -- which GitHub records as a
    // *cancellation*, so no crash dump is produced. NotifyProgress() is called
    // on every JS console line; once the gap exceeds the threshold the watchdog
    // raises __fastfail, which Windows Error Reporting turns into a full dump
    // (all thread stacks) that the existing `if: failure()` CI steps upload.
    std::atomic<bool>      s_watchdogStop{false};
    std::atomic<long long> s_lastProgressMs{0};
    std::thread            s_watchdogThread;

    long long ElapsedMs()
    {
        const auto elapsed = std::chrono::steady_clock::now() - s_startTime;
        return std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
    }

#if defined(_MSC_VER)
    void WatchdogWriteRaw(const char* msg)
    {
        // Write straight to the stderr OS handle, bypassing the CRT -- the CRT
        // allocator/lock may be exactly what's deadlocked.
        const HANDLE h = ::GetStdHandle(STD_ERROR_HANDLE);
        if (h != nullptr && h != INVALID_HANDLE_VALUE)
        {
            DWORD written = 0;
            ::WriteFile(h, msg, static_cast<DWORD>(std::strlen(msg)), &written, nullptr);
        }
    }

    void WatchdogWriteDump()
    {
        // Best-effort full-memory dump of the stalled process, written from
        // this (healthy) watchdog thread before we fast-fail. dbghelp uses the
        // Win32 heap, not the CRT/ASAN allocator, so it does not contend with a
        // blocked main thread. Named "Playground.dmp" in the run cwd so the
        // existing CI "Stage Playground Crash Dump" glob (Playground.*) uploads
        // it without any workflow change.
        const HANDLE hFile = ::CreateFileW(
            L"Playground.dmp",
            GENERIC_WRITE,
            0,
            nullptr,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            nullptr);
        if (hFile == INVALID_HANDLE_VALUE)
        {
            WatchdogWriteRaw("Watchdog: could not create Playground.dmp.\n");
            return;
        }

        const MINIDUMP_TYPE type = static_cast<MINIDUMP_TYPE>(
            MiniDumpWithThreadInfo |
            MiniDumpWithIndirectlyReferencedMemory |
            MiniDumpWithUnloadedModules |
            MiniDumpWithHandleData);

        const BOOL ok = ::MiniDumpWriteDump(
            ::GetCurrentProcess(),
            ::GetCurrentProcessId(),
            hFile,
            type,
            nullptr,
            nullptr,
            nullptr);

        ::CloseHandle(hFile);
        WatchdogWriteRaw(ok
            ? "Watchdog: wrote Playground.dmp.\n"
            : "Watchdog: MiniDumpWriteDump failed.\n");
    }
#endif

    void WatchdogLoop(int timeoutSeconds)
    {
        const long long timeoutMs = static_cast<long long>(timeoutSeconds) * 1000;
        while (!s_watchdogStop.load(std::memory_order_relaxed))
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            if (s_watchdogStop.load(std::memory_order_relaxed))
            {
                return;
            }

            const long long sinceMs = ElapsedMs() - s_lastProgressMs.load(std::memory_order_relaxed);
            if (sinceMs >= timeoutMs)
            {
#if defined(_MSC_VER)
                WatchdogWriteRaw(
                    "\n--- BN: WATCHDOG STALL ---\n"
                    "No forward progress within the watchdog interval; "
                    "writing a crash dump, then forcing __fastfail.\n");
                WatchdogWriteDump();
                // int 0x29: bypasses SEH/VEH (incl. ASAN's) and goes straight
                // to WER, so a deadlocked allocator can't swallow it.
                __fastfail(FAST_FAIL_FATAL_APP_EXIT);
#else
                std::abort();
#endif
            }
        }
    }

    void StopWatchdog()
    {
        s_watchdogStop.store(true, std::memory_order_relaxed);
        if (s_watchdogThread.joinable())
        {
            s_watchdogThread.join();
        }
    }

#if defined(_MSC_VER)
    void __cdecl OnInvalidParameter(
        const wchar_t* expression,
        const wchar_t* function,
        const wchar_t* file,
        unsigned int line,
        uintptr_t /*reserved*/)
    {
        // Format wchar_t inputs into the message body via %ls.
        Diagnostics::DumpFailure(
            "INVALID PARAMETER",
            nullptr,
            0,
            1 /* skip self */,
            "function=%ls expression=%ls (%ls:%u)",
            function != nullptr ? function : L"(null)",
            expression != nullptr ? expression : L"(null)",
            file != nullptr ? file : L"(null)",
            line);

        if (::IsDebuggerPresent())
        {
            bx::debugBreak();
        }
        Diagnostics::SetExitCode(3);
        Diagnostics::PrintFinishLine();
        std::_Exit(3);
    }

    void OnSignalAbort(int /*signal*/)
    {
        Diagnostics::DumpFailure("ABORT", nullptr, 0, 1, "SIGABRT raised.");
        if (::IsDebuggerPresent())
        {
            bx::debugBreak();
        }
        Diagnostics::SetExitCode(3);
        Diagnostics::PrintFinishLine();
        std::_Exit(3);
    }

    int OnCrtReport(int reportType, char* message, int* returnValue)
    {
        const char* kind = (reportType == _CRT_WARN)   ? "CRT WARN"
                         : (reportType == _CRT_ERROR)  ? "CRT ERROR"
                         : (reportType == _CRT_ASSERT) ? "CRT ASSERT"
                                                       : "CRT UNKNOWN";
        Diagnostics::DumpFailure(
            kind, nullptr, 0, 1,
            "%s",
            message != nullptr ? message : "(null)");

        if (returnValue != nullptr)
        {
            // Returning 1 here would trap into __debugbreak(); avoid that on
            // a no-debugger run -- it raises EXCEPTION_BREAKPOINT and exits
            // with STATUS_BREAKPOINT instead of our chosen exit code.
            *returnValue = ::IsDebuggerPresent() ? 1 : 0;
        }
        // TRUE suppresses the modal dialog.
        return TRUE;
    }
#else
    void OnSignalAbort(int /*signal*/)
    {
        Diagnostics::DumpFailure("ABORT", nullptr, 0, 1, "SIGABRT raised.");
        Diagnostics::SetExitCode(3);
        Diagnostics::PrintFinishLine();
        std::_Exit(3);
    }
#endif
}

namespace Diagnostics
{
    void InstallCrashHandler()
    {
        bool expected = false;
        if (!s_installed.compare_exchange_strong(expected, true))
        {
            return;
        }

#if defined(__SANITIZE_ADDRESS__)
        // Under ASAN the sanitizer runtime installs its own SEH /
        // _set_invalid_parameter_handler / abort hooks and provides its
        // own crash diagnostics. Letting bx install its handlers on top
        // (since bgfx commit eed706f, "Suppress MSVC CRT assert dialogs")
        // adds a _set_thread_local_invalid_parameter_handler + SEH
        // top-level filter + _set_purecall_handler that BN doesn't
        // override -- inside those, bx walks the callstack via dbghelp,
        // which can deadlock against ASAN's allocator lock when a
        // sanitizer-instrumented allocation races with handler entry.
        // BN's own _set_invalid_parameter_handler / SIGABRT / CRT report
        // hooks below cover the diagnostics paths we actually rely on.
#else
        bx::installExceptionHandler();
#endif

#if defined(_MSC_VER)
        // Route assert() to stderr instead of UCRT's modal dialog. Covers the
        // direct assert() codepath; _CrtSetReportMode below covers _CRT_*.
        _set_error_mode(_OUT_TO_STDERR);

        // Disable abort()'s retry/ignore message box.
        _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);

        _set_invalid_parameter_handler(&OnInvalidParameter);
        std::signal(SIGABRT, &OnSignalAbort);

#if defined(_DEBUG)
        // Force CRT report output to stderr and through our hook (debug CRT only).
        const int kReportTypes[] = {_CRT_WARN, _CRT_ERROR, _CRT_ASSERT};
        for (int reportType : kReportTypes)
        {
            _CrtSetReportMode(reportType, _CRTDBG_MODE_FILE);
            _CrtSetReportFile(reportType, _CRTDBG_FILE_STDERR);
        }
        _CrtSetReportHook(&OnCrtReport);
#endif
#else
        std::signal(SIGABRT, &OnSignalAbort);
#endif
    }

    void Initialize()
    {
        bool expected = false;
        if (!s_initialized.compare_exchange_strong(expected, true))
        {
            return;
        }

        s_startTime = std::chrono::steady_clock::now();

#if defined(_MSC_VER)
        // Enable ANSI VT processing so the colored finish line renders.
        const HANDLE hOut = ::GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut != INVALID_HANDLE_VALUE && hOut != nullptr)
        {
            DWORD mode = 0;
            if (::GetConsoleMode(hOut, &mode))
            {
                if (::SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING))
                {
                    s_ansiEnabled = true;
                }
            }
        }

        // Don't emit ANSI escapes when stdout is redirected to a file/pipe.
        if (!::_isatty(::_fileno(stdout)))
        {
            s_ansiEnabled = false;
        }
#else
        s_ansiEnabled = ::isatty(::fileno(stdout)) != 0;
#endif

        InstallCrashHandler();

        // Print finish line on every exit path. Both handlers idempotent
        // via s_finishPrinted; whichever fires first wins.
        std::atexit(&PrintFinishLine);
        std::at_quick_exit(&PrintFinishLine);

        // Reset the progress baseline so the watchdog measures from startup.
        NotifyProgress();

        // Stall watchdog: forces a crash dump if no forward progress occurs for
        // the configured interval, to capture the Win32 ASAN EXR-Loader stall
        // in CI (where a hang otherwise times out as an undumpable cancel).
        // Default 300s under ASAN; override/enable elsewhere with
        // BN_WATCHDOG_SECONDS=<positive int>.
        int watchdogSeconds = 0;
#if defined(__SANITIZE_ADDRESS__)
        watchdogSeconds = 300;
#endif
        const char* envWatchdog = nullptr;
#if defined(_MSC_VER)
        char* envBuf = nullptr;
        size_t envLen = 0;
        if (::_dupenv_s(&envBuf, &envLen, "BN_WATCHDOG_SECONDS") == 0)
        {
            envWatchdog = envBuf;
        }
#else
        envWatchdog = std::getenv("BN_WATCHDOG_SECONDS");
#endif
        if (envWatchdog != nullptr)
        {
            watchdogSeconds = std::atoi(envWatchdog);
        }
#if defined(_MSC_VER)
        if (envBuf != nullptr)
        {
            std::free(envBuf);
        }
#endif
        if (watchdogSeconds > 0)
        {
            s_watchdogStop.store(false, std::memory_order_relaxed);
            s_watchdogThread = std::thread(&WatchdogLoop, watchdogSeconds);
        }
    }

    void SetExitCode(int code)
    {
        s_exitCode.store(code, std::memory_order_relaxed);
    }

    void NotifyProgress()
    {
        s_lastProgressMs.store(ElapsedMs(), std::memory_order_relaxed);
    }

    void PrintFinishLine()
    {
        bool expected = false;
        if (!s_finishPrinted.compare_exchange_strong(expected, true))
        {
            return;
        }

        StopWatchdog();

        const int code = s_exitCode.load(std::memory_order_relaxed);
        const bool success = (code == 0);

        const auto elapsed = std::chrono::steady_clock::now() - s_startTime;
        const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();

        // Format elapsed as "1m 23.456s" / "12.345s" / "1234 ms".
        char timeBuf[64];
        if (ms >= 60000)
        {
            const long long totalSec = ms / 1000;
            const long long minutes = totalSec / 60;
            const long long seconds = totalSec % 60;
            const long long millis = ms % 1000;
            std::snprintf(timeBuf, sizeof(timeBuf), "%lldm %lld.%03llds", minutes, seconds, millis);
        }
        else if (ms >= 1000)
        {
            const long long seconds = ms / 1000;
            const long long millis = ms % 1000;
            std::snprintf(timeBuf, sizeof(timeBuf), "%lld.%03llds", seconds, millis);
        }
        else
        {
            std::snprintf(timeBuf, sizeof(timeBuf), "%lld ms", static_cast<long long>(ms));
        }

        const char* colorOn  = "";
        const char* colorOff = "";
        if (s_ansiEnabled)
        {
            colorOn  = success ? "\x1b[1;32m" : "\x1b[1;31m";  // bold green / bold red
            colorOff = "\x1b[0m";
        }

        // fputs via stdio (unbuffered, set in wWinMain) so the line reaches
        // the pipe before any subsequent _Exit / quick_exit.
        std::fprintf(stdout,
                     "%sPlayground: Finished in %s. (exit %d)%s\n",
                     colorOn, timeBuf, code, colorOff);
        std::fflush(stdout);
    }

    void DumpFailureV(const char* category, const char* file, int line, unsigned int skipFrames, const char* fmt, va_list args)
    {
        // Build the dump in a static buffer with bx::write helpers, then
        // mirror to stderr (console / CI logs) and bx::getDebugOut()
        // (OutputDebugString for attached debuggers).
        char temp[16 * 1024];
        bx::StaticMemoryBlockWriter smb(temp, BX_COUNTOF(temp));
        bx::ErrorIgnore err;
        int32_t total = 0;

        total += bx::write(&smb, &err, "\n--- BN: %s ---\n\n", category != nullptr ? category : "FAILURE");

        if (file != nullptr)
        {
            total += bx::write(&smb, &err, "%s(%d): ", file, line);
        }

        if (fmt != nullptr)
        {
            // bx::write(WriterI*, StringView format, va_list, Error*) passes
            // through to vsnprintf.
            total += bx::write(&smb, fmt, args, &err);
        }

        total += bx::write(&smb, &err, "\n\n");

        // +2 to skip this function and the public DumpFailure trampoline.
        uintptr_t stack[64];
        const uint32_t numFrames = bx::getCallStackExact(2 + skipFrames, BX_COUNTOF(stack), stack);
        total += bx::writeCallstack(&smb, stack, numFrames, &err);

        total += bx::write(&smb, &err,
            "\nBuild info:\n"
            "\tCompiler: " BX_COMPILER_NAME
            ", CPU: " BX_CPU_NAME
            ", Arch: " BX_ARCH_NAME
            ", OS: " BX_PLATFORM_NAME
            ", CRT: " BX_CRT_NAME
            ", C++: " BX_CPP_NAME
            ", Date: " __DATE__
            ", Time: " __TIME__
            "\n");

        total += bx::write(&smb, &err, "\n--- END ---\n\n");

        if (total > 0)
        {
            const size_t bytes = static_cast<size_t>(total);
            std::fwrite(temp, 1, bytes, stderr);
            std::fflush(stderr);

            // Mirror to OutputDebugString / logcat / syslog for attached debuggers.
            bx::write(bx::getDebugOut(), temp, total, bx::ErrorIgnore{});
        }
    }

    void DumpFailure(const char* category, const char* file, int line, unsigned int skipFrames, const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        DumpFailureV(category, file, line, skipFrames, fmt, args);
        va_end(args);
    }
}
