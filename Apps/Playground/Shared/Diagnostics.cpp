#include "Diagnostics.h"

#include <bx/bx.h>
#include <bx/debug.h>
#include <bx/error.h>
#include <bx/platform.h>
#include <bx/readerwriter.h>
#include <bx/string.h>

#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <string>

#if defined(_MSC_VER)
#   ifndef WIN32_LEAN_AND_MEAN
#       define WIN32_LEAN_AND_MEAN
#   endif // WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <crtdbg.h>
#include <dbghelp.h>
#include <stdlib.h>
#include <io.h>
#include <wchar.h>
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

    // Recover the message of the exception that is currently propagating, if
    // any. Returns an empty string when no exception is in flight. Valid inside
    // a terminate handler, and inside the MSVC SIGABRT handler that terminate()
    // ends up calling, because the exception stays current until the handler
    // returns. Allocates, so it must not be called from a signal handler that
    // can fire asynchronously (see OnSignalAbort on non-MSVC).
    std::string DescribeCurrentException()
    {
        if (std::current_exception() == nullptr)
        {
            return {};
        }

        try
        {
            std::rethrow_exception(std::current_exception());
        }
        catch (const std::exception& e)
        {
            return std::string{"uncaught std::exception: "} + e.what();
        }
        catch (...)
        {
            return "uncaught non-std exception.";
        }
    }

#if defined(_MSC_VER)
    // bx::writeCallstack() prints "<Unknown?>" for every frame in a stock
    // Release build, which makes crash reports impossible to triage. Resolve
    // the frames with dbghelp (shipped with Windows) and always append
    // "module+RVA" so the trace stays actionable even when no PDB is found.
    int32_t WriteSymbolizedCallstack(bx::WriterI* writer, const uintptr_t* stack, uint32_t numFrames, bx::Error* err)
    {
        const HANDLE process = ::GetCurrentProcess();

        static bool s_symInitialized = false;
        static bool s_symTried = false;
        if (!s_symTried)
        {
            s_symTried = true;
            ::SymSetOptions(SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES | SYMOPT_UNDNAME);
            s_symInitialized = ::SymInitialize(process, nullptr, TRUE) != FALSE;
        }

        int32_t total = bx::write(writer, err, "Callstack (%u):\n", numFrames);

        for (uint32_t ii = 0; ii < numFrames; ++ii)
        {
            const uintptr_t address = stack[ii];

            char moduleName[MAX_PATH];
            bx::snprintf(moduleName, sizeof(moduleName), "%s", "<unknown-module>");
            uintptr_t rva = address;

            HMODULE module = nullptr;
            if (::GetModuleHandleExA(
                    GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                    reinterpret_cast<LPCSTR>(address),
                    &module)
                && module != nullptr)
            {
                char fullPath[MAX_PATH];
                if (::GetModuleFileNameA(module, fullPath, MAX_PATH) != 0)
                {
                    const char* leaf = std::strrchr(fullPath, '\\');
                    bx::snprintf(moduleName, sizeof(moduleName), "%s", leaf != nullptr ? leaf + 1 : fullPath);
                }
                rva = address - reinterpret_cast<uintptr_t>(module);
            }

            total += bx::write(writer, err, "\t%2u: 0x%016llx  %s+0x%llx",
                ii,
                static_cast<unsigned long long>(address),
                moduleName,
                static_cast<unsigned long long>(rva));

            if (s_symInitialized)
            {
                alignas(SYMBOL_INFO) char symbolStorage[sizeof(SYMBOL_INFO) + MAX_SYM_NAME]{};
                auto* symbol = reinterpret_cast<SYMBOL_INFO*>(symbolStorage);
                symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
                symbol->MaxNameLen = MAX_SYM_NAME;

                DWORD64 symbolOffset = 0;
                if (::SymFromAddr(process, address, &symbolOffset, symbol) != FALSE)
                {
                    total += bx::write(writer, err, "  %s+0x%llx",
                        symbol->Name, static_cast<unsigned long long>(symbolOffset));
                }

                IMAGEHLP_LINE64 lineInfo{};
                lineInfo.SizeOfStruct = sizeof(lineInfo);
                DWORD lineOffset = 0;
                if (::SymGetLineFromAddr64(process, address, &lineOffset, &lineInfo) != FALSE)
                {
                    total += bx::write(writer, err, "  (%s:%u)",
                        lineInfo.FileName, static_cast<unsigned>(lineInfo.LineNumber));
                }
            }

            total += bx::write(writer, err, "\n");
        }

        return total;
    }

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
        // abort() is where std::terminate() ends up, among other paths. On MSVC
        // std::set_terminate() is per-thread, so a terminate on a worker thread
        // never reaches OnTerminate below and lands here instead; recover the
        // exception message either way.
        const std::string detail = DescribeCurrentException();
        Diagnostics::DumpFailure("ABORT", nullptr, 0, 1, "SIGABRT raised.%s%s",
            detail.empty() ? "" : "\n", detail.c_str());
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
        // Deliberately does not call DescribeCurrentException(). Outside the
        // Microsoft CRT std::set_terminate() is global, and OnTerminate() below
        // ends in std::_Exit(), so an uncaught exception is fully reported there
        // and never reaches abort(). Everything that does land here -- a direct
        // abort(), a libc assertion, raise(SIGABRT), kill -ABRT -- has no C++
        // exception in flight, so recovering one would return an empty string
        // anyway.
        //
        // That matters because this is a real signal handler: std::current_exception()
        // and std::string allocate, and abort() is frequently raised from inside
        // the allocator (heap corruption, a glibc malloc assertion). Allocating
        // here would deadlock against the allocator's own lock in exactly the
        // cases where the diagnostic is most needed.
        Diagnostics::DumpFailure("ABORT", nullptr, 0, 1, "SIGABRT raised.");
        Diagnostics::SetExitCode(3);
        Diagnostics::PrintFinishLine();
        std::_Exit(3);
    }
#endif

    // bx routes BOTH BX_ASSERT failures and the Windows SEH top-level
    // exception filter (bx::installExceptionHandler) through its assert
    // handler. bx's built-in default writes the banner only to
    // bx::getDebugOut() (OutputDebugString), which is invisible in a console /
    // CI run, and then tears the process down via TerminateProcess -- so the
    // native callstack never reaches the log and atexit (the finish line)
    // never runs. Route it through DumpFailure instead, which writes to stderr
    // (captured by CI), then exit deterministically with code 3 like the other
    // hard-failure handlers above.
    bool OnBxAssert(const bx::Location& location, uint32_t skip, const char* format, va_list args)
    {
        // Guard against re-entry if formatting/stack-walking itself faults.
        // Returning false here would tell bx to *continue* past the failed
        // assert / propagate the exception, i.e. keep running in an
        // already-failing state. Instead fail fast deterministically: skip the
        // (faulting) formatting path, emit a minimal marker, and exit.
        static std::atomic<bool> s_inHandler{false};
        bool expected = false;
        if (!s_inHandler.compare_exchange_strong(expected, true))
        {
            std::fputs("\n--- BN: CRASH (re-entrant) ---\n", stderr);
            std::fflush(stderr);
            Diagnostics::SetExitCode(3);
            Diagnostics::PrintFinishLine();
            std::_Exit(3);
        }

        // bx's exception-handler call sites (SEH filter, pure-call) pass
        // UINT32_MAX as the line and a synthetic file ("Exception Handler");
        // real BX_ASSERT sites pass an actual file/line.
        const bool isException = (location.line == UINT32_MAX);
        Diagnostics::DumpFailureV(
            isException ? "CRASH" : "ASSERT",
            isException ? nullptr : location.filePath,
            isException ? 0 : static_cast<int>(location.line),
            skip,
            format,
            args);

#if defined(_MSC_VER)
        if (::IsDebuggerPresent())
        {
            bx::debugBreak();
        }
#endif

        Diagnostics::SetExitCode(3);
        Diagnostics::PrintFinishLine();
        std::_Exit(3);
    }

    void OnTerminate()
    {
        // An uncaught C++ exception otherwise reaches abort() with nothing but
        // "SIGABRT raised.", which says nothing about what actually went wrong.
        //
        // This only covers the thread that installed it on Windows: the standard
        // says the terminate handler is global, but the Microsoft CRT keeps it
        // per-thread. OnSignalAbort() above repeats the same reporting so
        // worker-thread terminations stay diagnosable there.
        std::string detail = DescribeCurrentException();
        if (detail.empty())
        {
            detail = "terminate called without an active exception.";
        }

        Diagnostics::DumpFailure("TERMINATE", nullptr, 0, 1, "%s", detail.c_str());
        Diagnostics::SetExitCode(3);
        Diagnostics::PrintFinishLine();
        std::_Exit(3);
    }
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

        bx::installExceptionHandler();

        // Route bx asserts + the SEH top-level exception filter to DumpFailure
        // (stderr-visible) instead of bx's OutputDebugString-only default.
        bx::setAssertHandler(&OnBxAssert);

        // Report the message of an uncaught exception before abort() swallows it.
        std::set_terminate(&OnTerminate);

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
    }

    void SetExitCode(int code)
    {
        s_exitCode.store(code, std::memory_order_relaxed);
    }

    void PrintFinishLine()
    {
        bool expected = false;
        if (!s_finishPrinted.compare_exchange_strong(expected, true))
        {
            return;
        }

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
#if defined(_MSC_VER)
        // bx::writeCallstack() only resolves symbols when a debugger-quality
        // symbol handler is available, so in a plain Release run every frame
        // comes back as "<Unknown?>" with a raw address -- useless for triage.
        // Resolve here with dbghelp (always present on Windows) and always
        // fall back to "module+RVA", which stays meaningful even with no PDB
        // because it can be symbolized after the fact.
        total += WriteSymbolizedCallstack(&smb, stack, numFrames, &err);
#else
        total += bx::writeCallstack(&smb, stack, numFrames, &err);
#endif

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
