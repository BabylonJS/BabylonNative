#include "CommandLine.h"

#include <algorithm>
#include <array>
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

    enum class FlagKind
    {
        Boolean,        // --flag (no value)
        ValueRequired,  // --flag=value or --flag value
    };

    // One row per flag. Add a flag = add a row here. PrintUsage walks the
    // same table so flag definitions stop being a second source of truth.
    struct FlagSpec
    {
        std::string_view longName;    // e.g. "--test"
        std::string_view shortName;   // e.g. "-t" or "" if none
        FlagKind kind;
        std::string_view valueLabel;  // shown in usage as --flag=LABEL; "" for Boolean
        // Single-line summary; long supplementary text goes in continuation.
        // Continuation lines are pre-indented so they line up under the
        // description column in PrintUsage.
        std::string_view help;
        std::string_view continuation;
        // apply: set field(s) on `opt` from `value`. For Boolean flags `value`
        // is empty. On a parse error set `err` and leave `opt` unchanged.
        void (*apply)(PlaygroundOptions& opt, std::string_view value, std::string& err);
    };

    constexpr std::array kFlags = {
        FlagSpec{"--help", "-h", FlagKind::Boolean, "",
            "Show this help and exit (0).", "",
            [](PlaygroundOptions& o, std::string_view, std::string&) { o.ShowHelp = true; }},

        FlagSpec{"--list", "-l", FlagKind::Boolean, "",
            "List configured tests as TSV and exit (0).",
            "                              Columns: index, title, referenceImage,\n"
            "                              exclusionReason. exclusionReason reflects\n"
            "                              config state (ignores --include-excluded).\n",
            [](PlaygroundOptions& o, std::string_view, std::string&) { o.ListTests = true; }},

        FlagSpec{"--headless", "", FlagKind::Boolean, "",
            "Don't show a window (still creates HWND).", "",
            [](PlaygroundOptions& o, std::string_view, std::string&) { o.Headless = true; }},

        FlagSpec{"--break-on-fail", "", FlagKind::Boolean, "",
            "Trigger debugger break on a failing test.", "",
            [](PlaygroundOptions& o, std::string_view, std::string&) { o.BreakOnFail = true; }},

        FlagSpec{"--generate-references", "", FlagKind::Boolean, "",
            "Save rendered images as new reference PNGs.", "",
            [](PlaygroundOptions& o, std::string_view, std::string&) { o.GenerateReferences = true; }},

        FlagSpec{"--once", "", FlagKind::Boolean, "",
            "Run only the first matching test, then exit.", "",
            [](PlaygroundOptions& o, std::string_view, std::string&) { o.RunOnce = true; }},

        FlagSpec{"--include-excluded", "", FlagKind::Boolean, "",
            "Force-run tests that have",
            "                              excludeFromAutomaticTesting / onlyVisual /\n"
            "                              excludedGraphicsApis set in config.json.\n"
            "                              Useful for debugging quarantined tests\n"
            "                              without editing the config.\n",
            [](PlaygroundOptions& o, std::string_view, std::string&) { o.IncludeExcluded = true; }},

        FlagSpec{"--save-results", "", FlagKind::ValueRequired, "BOOL",
            "Override saving of result PNGs (default true).", "",
            [](PlaygroundOptions& o, std::string_view value, std::string& err) {
                bool b = false;
                if (!ParseBool(value, b)) { err = "invalid bool for --save-results: '" + std::string{value} + "'"; return; }
                o.SaveResults = b;
            }},

        FlagSpec{"--debug-trace", "", FlagKind::ValueRequired, "BOOL",
            "Enable/disable Babylon::DebugTrace.", "",
            [](PlaygroundOptions& o, std::string_view value, std::string& err) {
                bool b = false;
                if (!ParseBool(value, b)) { err = "invalid bool for --debug-trace: '" + std::string{value} + "'"; return; }
                o.DebugTrace = b;
            }},

        FlagSpec{"--perf-trace", "", FlagKind::ValueRequired, "LEVEL",
            "Set Babylon::PerfTrace level (None/Log).", "",
            [](PlaygroundOptions& o, std::string_view value, std::string&) { o.PerfTrace = std::string{value}; }},

        FlagSpec{"--capture", "", FlagKind::ValueRequired, "N",
            "Trigger RenderDoc capture on the Nth",
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
            "                              (those inject the paired DLL before main).\n",
            [](PlaygroundOptions& o, std::string_view value, std::string& err) {
                int n = 0;
                if (!ParseIntStrict(value, n) || n < 1 || n > 100000)
                {
                    err = "invalid --capture value (expected positive integer): '" + std::string{value} + "'";
                    return;
                }
                o.CaptureFrame = n;
            }},

        FlagSpec{"--test", "-t", FlagKind::ValueRequired, "PATTERN",
            "Run tests whose title contains PATTERN.",
            "                              May be specified multiple times (OR).\n",
            [](PlaygroundOptions& o, std::string_view value, std::string&) { o.TestFilters.emplace_back(value); }},

        FlagSpec{"--test-index", "", FlagKind::ValueRequired, "LIST",
            "Run only the listed indices. LIST may be a",
            "                              comma list of numbers or N-M ranges,\n"
            "                              e.g. '3,5,7' or '3-6' or '3,5-7,9'.\n",
            [](PlaygroundOptions& o, std::string_view value, std::string& err) {
                std::string ierr;
                if (!ParseIndexList(value, o.TestIndices, ierr))
                {
                    err = "invalid --test-index value: " + ierr;
                }
            }},
    };

    struct FlagMatch
    {
        bool matched = false;
        std::string value;     // for ValueRequired (or "" for Boolean)
        bool consumedNextArg = false;
    };

    // Returns true if arg matches "--longName" / "--longName=value" or
    // shortName. For ValueRequired flags missing an "=", consumes the next
    // arg as the value.
    FlagMatch MatchFlag(
        std::string_view arg,
        const FlagSpec& spec,
        const char* nextArg /* may be null */,
        std::string& err)
    {
        FlagMatch m;

        // --flag / --flag=value form
        if (StartsWith(arg, spec.longName))
        {
            std::string_view rest = arg.substr(spec.longName.size());
            if (rest.empty())
            {
                m.matched = true;
                if (spec.kind == FlagKind::ValueRequired)
                {
                    if (nextArg == nullptr)
                    {
                        err = "flag '" + std::string{spec.longName} + "' requires a value";
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
                if (spec.kind == FlagKind::Boolean)
                {
                    err = "flag '" + std::string{spec.longName} + "' does not take a value";
                    return m;
                }
                m.matched = true;
                m.value = std::string{rest.substr(1)};
                return m;
            }
            // not a real match (e.g. --testindex prefix-matches --test)
        }

        if (!spec.shortName.empty() && arg == spec.shortName)
        {
            m.matched = true;
            if (spec.kind == FlagKind::ValueRequired)
            {
                if (nextArg == nullptr)
                {
                    err = "flag '" + std::string{spec.shortName} + "' requires a value";
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
            bool dispatched = false;

            for (const FlagSpec& spec : kFlags)
            {
                FlagMatch m = MatchFlag(arg, spec, nextArg, err);
                if (!err.empty())
                {
                    opt.ParseError = true;
                    opt.ErrorMessage = err;
                    return opt;
                }
                if (!m.matched)
                {
                    continue;
                }
                if (m.consumedNextArg)
                {
                    ++i;
                }
                std::string applyErr;
                spec.apply(opt, m.value, applyErr);
                if (!applyErr.empty())
                {
                    opt.ParseError = true;
                    opt.ErrorMessage = applyErr;
                    return opt;
                }
                dispatched = true;
                break;
            }

            if (dispatched)
            {
                continue;
            }

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
        std::printf("Usage: %s [options] [script...]\n\n", prog);
        std::printf("Run Babylon Native Playground / visual tests.\n\n");
        std::printf("Options:\n");

        // Render each row of kFlags. The left column is fixed-width so
        // descriptions line up. Continuation strings already include the
        // matching indent so they slot in under the description column.
        constexpr int leftWidth = 30;
        for (const FlagSpec& spec : kFlags)
        {
            std::string left;
            left.reserve(leftWidth);
            left += "  ";
            if (!spec.shortName.empty())
            {
                left += std::string{spec.shortName};
                left += ", ";
            }
            else
            {
                left += "    ";
            }
            left += std::string{spec.longName};
            if (spec.kind == FlagKind::ValueRequired)
            {
                left += '=';
                left += std::string{spec.valueLabel};
            }
            if (static_cast<int>(left.size()) < leftWidth)
            {
                left.append(leftWidth - left.size(), ' ');
            }
            else
            {
                left += ' ';
            }
            std::printf("%s%.*s\n", left.c_str(),
                static_cast<int>(spec.help.size()), spec.help.data());
            if (!spec.continuation.empty())
            {
                std::fwrite(spec.continuation.data(), 1, spec.continuation.size(), stdout);
            }
        }

        std::printf("      --                      End of options; everything after is a script.\n");
        std::printf("\n");
        std::printf("Exit codes:\n");
        std::printf("   0  success / help / list\n");
        std::printf("   1  uncaught JS exception\n");
        std::printf("   2  command-line parse error\n");
        std::printf("   3  hard crash (assert / SIGABRT / unhandled exception)\n");
        std::printf("  -1  pixel-diff comparison failure\n");
    }
}