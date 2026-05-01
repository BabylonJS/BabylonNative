#pragma once

#include <optional>
#include <string>
#include <vector>

struct PlaygroundOptions
{
    bool ParseError = false;
    std::string ErrorMessage;

    bool ShowHelp = false;
    bool ListTests = false;
    bool Headless = false;
    bool BreakOnFail = false;
    bool GenerateReferences = false;
    bool RunOnce = false;
    bool IncludeExcluded = false;

    std::optional<bool> SaveResults;
    std::optional<bool> DebugTrace;
    std::optional<std::string> PerfTrace;

    // 1-based frame index at which to call TestUtils.captureNextFrame()
    // (RenderDoc capture trigger). When set, the runner extends each test's
    // render budget so the .rdc finalizes.
    std::optional<int> CaptureFrame;

    // Absolute path to a renderdoc.dll to preload before bgfx::init.
    // Validated at parse time (must point to an existing file, or to a
    // directory containing renderdoc.dll which is then resolved to that file).
    std::optional<std::string> RenderDocDll;

    std::vector<std::string> TestFilters;
    std::vector<int> TestIndices;

    // Positional script arguments (URLs / paths).
    std::vector<std::string> Scripts;
};

namespace CommandLine
{
    // Parses argv (argv[0] is treated as the program name and ignored).
    // On a parse error, returns an options struct with ParseError == true
    // and ErrorMessage populated.
    PlaygroundOptions Parse(int argc, const char* const* argv);

    // Prints usage / help text to stdout. argv0 may be nullptr.
    void PrintUsage(const char* argv0);
}
