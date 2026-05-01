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
#include <vector>

#if defined(_MSC_VER)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <crtdbg.h>
#include <stdlib.h>
#include <io.h>
#include <wchar.h>
#include <wctype.h>
// VS_FIXEDFILEINFO + VerQueryValueW
#pragma comment(lib, "version.lib")
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

    void DumpCallstack()
    {
        // Skip 1 frame (this helper).
        bx::debugOutputCallstack(1);
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

        bx::installExceptionHandler();

#if defined(_MSC_VER)
        // Route assert() to stderr instead of UCRT's modal dialog. Covers the
        // direct assert() codepath; _CrtSetReportMode below covers _CRT_*.
        _set_error_mode(_OUT_TO_STDERR);

        // Disable abort()'s retry/ignore message box.
        _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);

        _set_invalid_parameter_handler(&OnInvalidParameter);
        std::signal(SIGABRT, &OnSignalAbort);

        // Force CRT report output to stderr and through our hook.
        const int kReportTypes[] = {_CRT_WARN, _CRT_ERROR, _CRT_ASSERT};
        for (int reportType : kReportTypes)
        {
            _CrtSetReportMode(reportType, _CRTDBG_MODE_FILE);
            _CrtSetReportFile(reportType, _CRTDBG_FILE_STDERR);
        }
        _CrtSetReportHook(&OnCrtReport);
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

#if defined(_MSC_VER)
namespace
{
    // Minimal RenderDoc API surface for diagnostic queries. GetAPIVersion is
    // the first entry point in every published renderdoc_app.h version, so
    // probing it works regardless of which API revision the DLL implements.
    struct RenderDocApiQuery
    {
        void(__cdecl* GetAPIVersion)(int* major, int* minor, int* patch);
    };
    using pRENDERDOC_GetAPI_t = int(__cdecl*)(int version, void** outAPIPointers);

    // Newest first; we probe in order until one is accepted.
    constexpr int kRenderDocApiVersions[] = { 10600, 10500, 10400, 10300, 10200, 10100 };

    std::wstring Utf8ToWide(const char* s)
    {
        if (s == nullptr || *s == '\0')
        {
            return {};
        }
        const int needed = ::MultiByteToWideChar(CP_UTF8, 0, s, -1, nullptr, 0);
        if (needed <= 1)
        {
            return {};
        }
        std::wstring w(static_cast<size_t>(needed - 1), L'\0');
        ::MultiByteToWideChar(CP_UTF8, 0, s, -1, w.data(), needed);
        return w;
    }

    std::string WideToUtf8(const wchar_t* w)
    {
        if (w == nullptr || *w == L'\0')
        {
            return {};
        }
        const int needed = ::WideCharToMultiByte(CP_UTF8, 0, w, -1, nullptr, 0, nullptr, nullptr);
        if (needed <= 1)
        {
            return {};
        }
        std::string s(static_cast<size_t>(needed - 1), '\0');
        ::WideCharToMultiByte(CP_UTF8, 0, w, -1, s.data(), needed, nullptr, nullptr);
        return s;
    }

    std::wstring GetEnvW(const wchar_t* name)
    {
        const DWORD needed = ::GetEnvironmentVariableW(name, nullptr, 0);
        if (needed == 0)
        {
            return {};
        }
        std::wstring v(static_cast<size_t>(needed), L'\0');
        const DWORD got = ::GetEnvironmentVariableW(name, v.data(), needed);
        if (got == 0 || got >= needed)
        {
            return {};
        }
        v.resize(got);
        return v;
    }

    bool IsDirectoryW(const std::wstring& p)
    {
        const DWORD attrs = ::GetFileAttributesW(p.c_str());
        return attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_DIRECTORY) != 0;
    }

    bool IsFileW(const std::wstring& p)
    {
        const DWORD attrs = ::GetFileAttributesW(p.c_str());
        return attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_DIRECTORY) == 0;
    }

    // Append "\renderdoc.dll" if path is a directory; otherwise leave alone.
    std::wstring NormalizeRenderDocCandidate(std::wstring p)
    {
        if (IsDirectoryW(p))
        {
            if (!p.empty() && p.back() != L'\\' && p.back() != L'/')
            {
                p.push_back(L'\\');
            }
            p += L"renderdoc.dll";
        }
        return p;
    }

    // Lowercase + slash-normalize for case/separator-insensitive path
    // comparison. Doesn't resolve symlinks or "..", but our comparands all
    // come from GetModuleFileNameW or std::filesystem::absolute.
    std::wstring CanonicalizeForCompare(std::wstring s)
    {
        for (auto& c : s)
        {
            if (c == L'/')
            {
                c = L'\\';
            }
            else
            {
                c = static_cast<wchar_t>(::towlower(c));
            }
        }
        // Strip "\\?\" extended-length prefix if present.
        if (s.size() >= 4 && s.compare(0, 4, L"\\\\?\\") == 0)
        {
            s.erase(0, 4);
        }
        return s;
    }

    bool SamePath(const std::wstring& a, const std::wstring& b)
    {
        return CanonicalizeForCompare(a) == CanonicalizeForCompare(b);
    }

    std::wstring GetModulePathW(HMODULE mod)
    {
        wchar_t buf[MAX_PATH];
        DWORD got = ::GetModuleFileNameW(mod, buf, static_cast<DWORD>(std::size(buf)));
        if (got == 0)
        {
            return {};
        }
        if (got < std::size(buf))
        {
            return std::wstring(buf, got);
        }
        // ERROR_INSUFFICIENT_BUFFER: retry with extended-length buffer.
        std::vector<wchar_t> big(32768);
        got = ::GetModuleFileNameW(mod, big.data(), static_cast<DWORD>(big.size()));
        if (got == 0 || got >= big.size())
        {
            return {};
        }
        return std::wstring(big.data(), got);
    }

    // Returns "1.41.0.0"-style FileVersion from the DLL's VS_FIXEDFILEINFO,
    // or empty on any failure.
    std::string GetFileVersionStringW(const wchar_t* path)
    {
        DWORD ignored = 0;
        const DWORD size = ::GetFileVersionInfoSizeW(path, &ignored);
        if (size == 0)
        {
            return {};
        }
        std::vector<BYTE> buf(size);
        if (!::GetFileVersionInfoW(path, 0, size, buf.data()))
        {
            return {};
        }
        VS_FIXEDFILEINFO* info = nullptr;
        UINT len = 0;
        if (!::VerQueryValueW(buf.data(), L"\\", reinterpret_cast<void**>(&info), &len) ||
            info == nullptr || len < sizeof(VS_FIXEDFILEINFO))
        {
            return {};
        }
        char out[64];
        std::snprintf(out, sizeof out, "%u.%u.%u.%u",
                      HIWORD(info->dwFileVersionMS),
                      LOWORD(info->dwFileVersionMS),
                      HIWORD(info->dwFileVersionLS),
                      LOWORD(info->dwFileVersionLS));
        return std::string{out};
    }

    // Returns the DLL's "X.Y.Z" API version via RENDERDOC_GetAPI, or empty
    // on failure. Probes newest-first so older DLLs still report something.
    std::string QueryRenderDocApiVersion(HMODULE mod)
    {
        auto getApi = reinterpret_cast<pRENDERDOC_GetAPI_t>(
            ::GetProcAddress(mod, "RENDERDOC_GetAPI"));
        if (getApi == nullptr)
        {
            return {};
        }
        for (int v : kRenderDocApiVersions)
        {
            void* api = nullptr;
            if (getApi(v, &api) == 1 && api != nullptr)
            {
                int maj = 0, min = 0, pat = 0;
                reinterpret_cast<RenderDocApiQuery*>(api)->GetAPIVersion(&maj, &min, &pat);
                char out[32];
                std::snprintf(out, sizeof out, "%d.%d.%d", maj, min, pat);
                return std::string{out};
            }
        }
        return {};
    }

    void PrintRenderDocInfo(HMODULE mod)
    {
        const std::wstring wpath = GetModulePathW(mod);
        const std::string  path  = WideToUtf8(wpath.c_str());
        const std::string  apiVer  = QueryRenderDocApiVersion(mod);
        const std::string  fileVer = wpath.empty() ? std::string{} : GetFileVersionStringW(wpath.c_str());

        std::printf("RenderDoc: %s", path.empty() ? "(unknown path)" : path.c_str());
        const bool haveAny = !apiVer.empty() || !fileVer.empty();
        if (haveAny)
        {
            std::printf(" (");
            const char* sep = "";
            if (!apiVer.empty())
            {
                std::printf("API %s", apiVer.c_str());
                sep = ", ";
            }
            if (!fileVer.empty())
            {
                std::printf("%sFileVersion %s", sep, fileVer.c_str());
            }
            std::printf(")");
        }
        std::printf("\n");
        std::fflush(stdout);
    }

    void PrintNoDllWarning()
    {
        std::fprintf(stderr,
            "RenderDoc: WARNING: --capture requested but renderdoc.dll could not be loaded.\n"
            "RenderDoc: Set --renderdoc-dll=PATH, BN_RENDERDOC_DLL, or RENDERDOC_PYTHON_PATH env var\n"
            "RenderDoc: (rdc-cli's renderdoc-py directory), or place renderdoc.dll on PATH.\n");
        std::fflush(stderr);
    }

    // Warn if the loaded DLL doesn't match RENDERDOC_PYTHON_PATH/renderdoc.dll
    // (rdc-cli's pair). A mismatch means rdc-cli may reject the capture due
    // to format-version skew.
    void WarnOnRdcCliMismatch(HMODULE mod)
    {
        std::wstring renderdocPy = GetEnvW(L"RENDERDOC_PYTHON_PATH");
        if (renderdocPy.empty())
        {
            return;
        }
        if (renderdocPy.back() != L'\\' && renderdocPy.back() != L'/')
        {
            renderdocPy.push_back(L'\\');
        }
        renderdocPy += L"renderdoc.dll";
        if (!IsFileW(renderdocPy))
        {
            return;
        }
        const std::wstring loaded = GetModulePathW(mod);
        if (loaded.empty() || SamePath(loaded, renderdocPy))
        {
            return;
        }
        const std::string py = WideToUtf8(renderdocPy.c_str());
        std::fprintf(stderr,
            "RenderDoc: WARNING: loaded DLL differs from RENDERDOC_PYTHON_PATH pair.\n"
            "RenderDoc:          rdc-cli pair: %s\n"
            "RenderDoc:          Captures may not open in rdc-cli due to version mismatch.\n"
            "RenderDoc:          Pin with --renderdoc-dll=\"%s\".\n",
            py.c_str(), py.c_str());
        std::fflush(stderr);
    }

    std::atomic<bool> s_renderDocSetupDone{false};
} // anonymous namespace
#endif // _MSC_VER

bool Diagnostics::SetupRenderDoc(const char* explicitDllPath, bool captureRequested)
{
#if !defined(_MSC_VER)
    (void)explicitDllPath;
    if (captureRequested)
    {
        std::fprintf(stderr,
            "RenderDoc: --capture is not supported on this platform.\n");
    }
    return false;
#else
    bool expected = false;
    if (!s_renderDocSetupDone.compare_exchange_strong(expected, true))
    {
        // Already ran; report current state.
        return ::GetModuleHandleW(L"renderdoc.dll") != nullptr;
    }

    const std::wstring explicitW = Utf8ToWide(explicitDllPath);
    const bool haveExplicit = !explicitW.empty();

    // Step 1: detect any pre-existing renderdoc.dll. bgfx will adopt
    // whatever's already loaded via findModule.
    HMODULE mod = ::GetModuleHandleW(L"renderdoc.dll");
    std::wstring existingPath = mod ? GetModulePathW(mod) : std::wstring{};

    // Step 2: explicit-path conflict. If --renderdoc-dll was given but a
    // different module is already loaded, we can't honor the pin -- report
    // it loudly and continue with the existing module.
    if (mod && haveExplicit && !SamePath(existingPath, explicitW))
    {
        const std::string ePath = WideToUtf8(existingPath.c_str());
        std::fprintf(stderr,
            "RenderDoc: ERROR: --renderdoc-dll=%s could not be honored.\n"
            "RenderDoc:        renderdoc.dll is already loaded from %s.\n"
            "RenderDoc:        Refusing to silently use a different DLL than requested.\n",
            explicitDllPath, ePath.empty() ? "(unknown)" : ePath.c_str());
        std::fflush(stderr);
        PrintRenderDocInfo(mod);
        return true;
    }

    // Step 3: explicit-path preload (always; user explicitly asked).
    if (mod == nullptr && haveExplicit)
    {
        mod = ::LoadLibraryW(explicitW.c_str());
        if (mod == nullptr)
        {
            std::fprintf(stderr,
                "RenderDoc: ERROR: LoadLibrary failed for --renderdoc-dll=%s (Win32 error %lu).\n"
                "RenderDoc:        Capture will be a no-op for this run.\n",
                explicitDllPath, ::GetLastError());
            std::fflush(stderr);
        }
    }

    // Step 4: env-var preload. Capture-only so a stale env var doesn't
    // silently load RenderDoc into every Playground run.
    if (mod == nullptr && captureRequested)
    {
        std::wstring envExplicit = GetEnvW(L"BN_RENDERDOC_DLL");
        if (!envExplicit.empty())
        {
            envExplicit = NormalizeRenderDocCandidate(std::move(envExplicit));
            if (IsFileW(envExplicit))
            {
                mod = ::LoadLibraryW(envExplicit.c_str());
                if (mod == nullptr)
                {
                    const std::string p = WideToUtf8(envExplicit.c_str());
                    std::fprintf(stderr,
                        "RenderDoc: WARNING: BN_RENDERDOC_DLL=%s failed to load (Win32 error %lu).\n",
                        p.c_str(), ::GetLastError());
                    std::fflush(stderr);
                }
            }
            else
            {
                const std::string p = WideToUtf8(envExplicit.c_str());
                std::fprintf(stderr,
                    "RenderDoc: WARNING: BN_RENDERDOC_DLL points to a missing file: %s.\n",
                    p.c_str());
                std::fflush(stderr);
            }
        }
    }
    if (mod == nullptr && captureRequested)
    {
        std::wstring renderdocPy = GetEnvW(L"RENDERDOC_PYTHON_PATH");
        if (!renderdocPy.empty())
        {
            if (renderdocPy.back() != L'\\' && renderdocPy.back() != L'/')
            {
                renderdocPy.push_back(L'\\');
            }
            renderdocPy += L"renderdoc.dll";
            if (IsFileW(renderdocPy))
            {
                mod = ::LoadLibraryW(renderdocPy.c_str());
            }
        }
    }
    // Step 5: Windows DLL search (PATH + app dir). Capture-only so we don't
    // load RenderDoc as a side effect.
    if (mod == nullptr && captureRequested)
    {
        mod = ::LoadLibraryW(L"renderdoc.dll");
    }

    // Step 6: report.
    if (mod != nullptr)
    {
        PrintRenderDocInfo(mod);
        if (captureRequested)
        {
            WarnOnRdcCliMismatch(mod);
        }
        return true;
    }

    if (captureRequested)
    {
        PrintNoDllWarning();
    }
    return false;
#endif
}
}
