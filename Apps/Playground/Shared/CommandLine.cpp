#include "CommandLine.h"

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <string>
#include <string_view>

namespace
{
    bool StartsWith(std::string_view s, std::string_view prefix)
    {
        return s.size() >= prefix.size() && s.compare(0, prefix.size(), prefix) == 0;
    }

    std::string ToLower(std::string s)
    {
        std::transform(s.begin(), s.end(), s.begin(),
                       [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        return s;
    }

    bool ParseBool(std::string_view value, bool& out)
    {
        std::string v = ToLower(std::string{value});
        if (v == "1" || v == "true" || v == "yes" || v == "on")
        {
            out = true;
            return true;
        }
        if (v == "0" || v == "false" || v == "no" || v == "off")
        {
            out = false;
            return true;
        }
        return false;
    }

    bool ParseIntStrict(std::string_view s, int& out)
    {
        if (s.empty())
        {
            return false;
        }
        std::string str{s};
        char* end = nullptr;
        long v = std::strtol(str.c_str(), &end, 10);
        if (end == nullptr || *end != '\0')
        {
            return false;
        }
        out = static_cast<int>(v);
        return true;
    }

    // Accepts: "3", "3,5,7", "3-6", "3,5-7,9".
    bool ParseIndexList(std::string_view spec, std::vector<int>& out, std::string& err)
    {
        size_t pos = 0;
        const std::string s{spec};
        while (pos < s.size())
        {
            size_t comma = s.find(',', pos);
            std::string_view token = std::string_view{s}.substr(pos, comma == std::string::npos ? std::string::npos : comma - pos);
            if (token.empty())
            {
                err = "empty index in range list";
                return false;
            }
            size_t dash = token.find('-');
            if (dash == std::string_view::npos)
            {
                int v;
                if (!ParseIntStrict(token, v))
                {
                    err = "not an integer: '" + std::string{token} + "'";
                    return false;
                }
                out.push_back(v);
            }
            else
            {
                int lo, hi;
                if (!ParseIntStrict(token.substr(0, dash), lo) ||
                    !ParseIntStrict(token.substr(dash + 1), hi))
                {
                    err = "not an integer range: '" + std::string{token} + "'";
                    return false;
                }
                if (hi < lo)
                {
                    std::swap(lo, hi);
                }
                for (int v = lo; v <= hi; ++v)
                {
                    out.push_back(v);
                }
            }
            if (comma == std::string::npos)
            {
                break;
            }
            pos = comma + 1;
        }
        return true;
    }

    // Returns true if arg matches "--longName" / "--longName=value" or
    // shortName. For ValueRequired flags missing an "=", consumes the next
    // arg as the value.
    enum class FlagKind
    {
        Boolean,        // --flag (no value)
        ValueRequired,  // --flag=value or --flag value
    };

    struct FlagMatch
    {
        bool matched = false;
        std::string value;     // for ValueRequired (or "" for Boolean)
        bool consumedNextArg = false;
    };

    FlagMatch MatchFlag(
        std::string_view arg,
        std::string_view longName,    // e.g. "--test"
        std::string_view shortName,   // e.g. "-t" or "" if none
        FlagKind kind,
        const char* nextArg /* may be null */,
        std::string& err)
    {
        FlagMatch m;

        // --flag=value form
        if (StartsWith(arg, longName))
        {
            std::string_view rest = arg.substr(longName.size());
            if (rest.empty())
            {
                m.matched = true;
                if (kind == FlagKind::ValueRequired)
                {
                    if (nextArg == nullptr)
                    {
                        err = "flag '" + std::string{longName} + "' requires a value";
                        m.matched = false;
                        return m;
                    }
                    m.value = nextArg;
                    m.consumedNextArg = true;
                }
                return m;
            }
            if (rest[0] == '=')
            {
                if (kind == FlagKind::Boolean)
                {
                    err = "flag '" + std::string{longName} + "' does not take a value";
                    return m;
                }
                m.matched = true;
                m.value = std::string{rest.substr(1)};
                return m;
            }
            // not a real match (e.g. --testindex prefix-matches --test)
        }

        if (!shortName.empty() && arg == shortName)
        {
            m.matched = true;
            if (kind == FlagKind::ValueRequired)
            {
                if (nextArg == nullptr)
                {
                    err = "flag '" + std::string{shortName} + "' requires a value";
                    m.matched = false;
                    return m;
                }
                m.value = nextArg;
                m.consumedNextArg = true;
            }
        }
        return m;
    }
}

namespace CommandLine
{
    PlaygroundOptions Parse(int argc, const char* const* argv)
    {
        PlaygroundOptions opt;
        if (argc <= 0 || argv == nullptr)
        {
            return opt;
        }

        // argv[0] is the program name; skip it.
        for (int i = 1; i < argc; ++i)
        {
            std::string_view arg = argv[i] != nullptr ? argv[i] : "";
            if (arg.empty())
            {
                continue;
            }

            // "--" terminates options; everything after is positional.
            if (arg == "--")
            {
                for (++i; i < argc; ++i)
                {
                    if (argv[i] != nullptr)
                    {
                        opt.Scripts.emplace_back(argv[i]);
                    }
                }
                break;
            }

            const char* nextArg = (i + 1 < argc) ? argv[i + 1] : nullptr;
            std::string err;

            auto match = [&](std::string_view longN, std::string_view shortN, FlagKind kind) {
                FlagMatch m = MatchFlag(arg, longN, shortN, kind, nextArg, err);
                if (m.matched && m.consumedNextArg)
                {
                    ++i;
                }
                return m;
            };

            if (auto m = match("--help", "-h", FlagKind::Boolean); m.matched)
            {
                opt.ShowHelp = true;
                continue;
            }
            if (!err.empty()) { opt.ParseError = true; opt.ErrorMessage = err; return opt; }

            if (auto m = match("--list", "-l", FlagKind::Boolean); m.matched)
            {
                opt.ListTests = true;
                continue;
            }
            if (!err.empty()) { opt.ParseError = true; opt.ErrorMessage = err; return opt; }

            if (auto m = match("--headless", "", FlagKind::Boolean); m.matched)
            {
                opt.Headless = true;
                continue;
            }
            if (!err.empty()) { opt.ParseError = true; opt.ErrorMessage = err; return opt; }

            if (auto m = match("--break-on-fail", "", FlagKind::Boolean); m.matched)
            {
                opt.BreakOnFail = true;
                continue;
            }
            if (!err.empty()) { opt.ParseError = true; opt.ErrorMessage = err; return opt; }

            if (auto m = match("--generate-references", "", FlagKind::Boolean); m.matched)
            {
                opt.GenerateReferences = true;
                continue;
            }
            if (!err.empty()) { opt.ParseError = true; opt.ErrorMessage = err; return opt; }

            if (auto m = match("--once", "", FlagKind::Boolean); m.matched)
            {
                opt.RunOnce = true;
                continue;
            }
            if (!err.empty()) { opt.ParseError = true; opt.ErrorMessage = err; return opt; }

            if (auto m = match("--include-excluded", "", FlagKind::Boolean); m.matched)
            {
                opt.IncludeExcluded = true;
                continue;
            }
            if (!err.empty()) { opt.ParseError = true; opt.ErrorMessage = err; return opt; }

            if (auto m = match("--save-results", "", FlagKind::ValueRequired); m.matched)
            {
                bool b = false;
                if (!ParseBool(m.value, b))
                {
                    opt.ParseError = true;
                    opt.ErrorMessage = "invalid bool for --save-results: '" + m.value + "'";
                    return opt;
                }
                opt.SaveResults = b;
                continue;
            }
            if (!err.empty()) { opt.ParseError = true; opt.ErrorMessage = err; return opt; }

            if (auto m = match("--debug-trace", "", FlagKind::ValueRequired); m.matched)
            {
                bool b = false;
                if (!ParseBool(m.value, b))
                {
                    opt.ParseError = true;
                    opt.ErrorMessage = "invalid bool for --debug-trace: '" + m.value + "'";
                    return opt;
                }
                opt.DebugTrace = b;
                continue;
            }
            if (!err.empty()) { opt.ParseError = true; opt.ErrorMessage = err; return opt; }

            if (auto m = match("--perf-trace", "", FlagKind::ValueRequired); m.matched)
            {
                opt.PerfTrace = m.value;
                continue;
            }
            if (!err.empty()) { opt.ParseError = true; opt.ErrorMessage = err; return opt; }

            if (auto m = match("--capture", "", FlagKind::ValueRequired); m.matched)
            {
                int n = 0;
                if (!ParseIntStrict(m.value, n) || n < 1 || n > 100000)
                {
                    opt.ParseError = true;
                    opt.ErrorMessage = "invalid --capture value (expected positive integer): '" + m.value + "'";
                    return opt;
                }
                opt.CaptureFrame = n;
                continue;
            }
            if (!err.empty()) { opt.ParseError = true; opt.ErrorMessage = err; return opt; }

            if (auto m = match("--test", "-t", FlagKind::ValueRequired); m.matched)
            {
                opt.TestFilters.push_back(m.value);
                continue;
            }
            if (!err.empty()) { opt.ParseError = true; opt.ErrorMessage = err; return opt; }

            if (auto m = match("--test-index", "", FlagKind::ValueRequired); m.matched)
            {
                std::string ierr;
                if (!ParseIndexList(m.value, opt.TestIndices, ierr))
                {
                    opt.ParseError = true;
                    opt.ErrorMessage = "invalid --test-index value: " + ierr;
                    return opt;
                }
                continue;
            }
            if (!err.empty()) { opt.ParseError = true; opt.ErrorMessage = err; return opt; }

            // Unknown flag?
            if (StartsWith(arg, "-"))
            {
                opt.ParseError = true;
                opt.ErrorMessage = "unknown flag: '" + std::string{arg} + "'";
                return opt;
            }

            // Positional argument (script path / URL).
            opt.Scripts.emplace_back(arg);
        }

        return opt;
    }

    void PrintUsage(const char* argv0)
    {
        const char* prog = (argv0 != nullptr && *argv0 != '\0') ? argv0 : "Playground";
        std::printf(
            "Usage: %s [options] [script...]\n"
            "\n"
            "Run Babylon Native Playground / visual tests.\n"
            "\n"
            "Options:\n"
            "  -h, --help                  Show this help and exit (0).\n"
            "  -l, --list                  List configured tests as TSV and exit (0).\n"
            "      --headless              Don't show a window (still creates HWND).\n"
            "      --break-on-fail         Trigger debugger break on a failing test.\n"
            "      --generate-references   Save rendered images as new reference PNGs.\n"
            "      --once                  Run only the first matching test, then exit.\n"
            "      --include-excluded      Force-run tests that have\n"
            "                              excludeFromAutomaticTesting / onlyVisual /\n"
            "                              excludedGraphicsApis set in config.json.\n"
            "                              Useful for debugging quarantined tests\n"
            "                              without editing the config.\n"
            "      --save-results=BOOL     Override saving of result PNGs (default true).\n"
            "      --debug-trace=BOOL      Enable/disable Babylon::DebugTrace.\n"
            "      --perf-trace=LEVEL      Set Babylon::PerfTrace level (None/Log).\n"
            "      --capture=N             Trigger RenderDoc capture on the Nth\n"
            "                              rendered frame (1-based) of each executed\n"
            "                              test. The test's renderCount is auto-bumped\n"
            "                              so the .rdc finalizes even if the test\n"
            "                              normally renders only 1 frame. Pixel\n"
            "                              comparison still happens at the test's\n"
            "                              original renderCount, so pass/fail behavior\n"
            "                              is unaffected. The .rdc lives at\n"
            "                              <cwd>/temp/bgfx_frame<N>.rdc.\n"
            "                              Combine with --once / --test / --test-index\n"
            "                              to limit to a single capture.\n"
            "                              Requires renderdoc.dll to be loaded into\n"
            "                              the process. Easiest: launch via\n"
            "                              `renderdoccmd capture -w Playground.exe ...`\n"
            "                              or `rdc capture --trigger -w -- Playground.exe ...`\n"
            "                              (those inject the paired DLL before main).\n"
            "  -t, --test=PATTERN          Run tests whose title contains PATTERN.\n"
            "                              May be specified multiple times (OR).\n"
            "      --test-index=LIST       Run only the listed indices. LIST may be a\n"
            "                              comma list of numbers or N-M ranges,\n"
            "                              e.g. '3,5,7' or '3-6' or '3,5-7,9'.\n"
            "      --                      End of options; everything after is a script.\n"
            "\n"
            "Exit codes:\n"
            "   0  success / help / list\n"
            "   1  uncaught JS exception\n"
            "   2  command-line parse error\n"
            "   3  hard crash (assert / SIGABRT / unhandled exception)\n"
            "  -1  pixel-diff comparison failure\n",
            prog);
    }
}
