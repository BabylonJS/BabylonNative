// ShaderCacheGenerator
//
// Headless Win32 + OpenGL tool that builds a GPU shader cache for a Babylon.js
// scene. It hosts the scene through the Babylon::Embedding Runtime/View API (an
// offscreen window driving the ANGLE OpenGL backend), renders frames until the
// scene has finished compiling its shaders ("scene is ready"), then writes the
// ShaderCache to a file and exits.
//
// Usage:
//     ShaderCacheGenerator <scene.js> <output.bin> [shared options]
//
// Command-line parsing, process diagnostics (console/crash/exit handling) and
// the log callback are reused from Apps/Playground/Shared. The two positional
// arguments are the scene script and the output cache file.
//
// "Scene ready" is defined as: the shader cache entry count has stopped growing
// for `kStableChecks` consecutive checks. For a shader-cache generator this is
// exactly the meaningful signal -- every effect the scene uses has been
// compiled (and therefore cached).

#include <Babylon/Embedding/Runtime.h>
#include <Babylon/Embedding/View.h>
#include <Babylon/Plugins/ShaderCache.h>

#include <Shared/CommandLine.h>
#include <Shared/Diagnostics.h>
#include <Shared/PlaygroundScripts.h>

#include <Windows.h>
#include <Shlwapi.h>

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace
{
    // Offscreen render target size.
    constexpr uint32_t kWidth = 1024;
    constexpr uint32_t kHeight = 1024;

    // Readiness heuristic: render until the cached-shader count is unchanged for
    // kStableChecks consecutive checks (sampled every kCheckInterval frames),
    // capped at kMaxFrames.
    constexpr uint32_t kCheckInterval = 10;
    constexpr uint32_t kStableChecks = 15;
    constexpr uint32_t kMaxFrames = 3000;

    void PrintUsage(const char* exe)
    {
        std::cerr << "Usage: " << (exe != nullptr ? exe : "ShaderCacheGenerator")
                  << " <scene.js> <output.bin> [shared options]\n"
                  << "  <scene.js>    Babylon.js scene script to load and render.\n"
                  << "  <output.bin>  Shader cache file to write.\n"
                  << "Shared options (see below) are parsed by the Playground command line,\n"
                  << "e.g. --debug-trace=true.\n\n";
        CommandLine::PrintUsage(exe);
    }

    // Build a file:// URL for an absolute path so ScriptLoader/File can fetch it.
    std::string GetUrlFromPath(const std::string& path)
    {
        char url[2048];
        DWORD length = ARRAYSIZE(url);
        if (FAILED(UrlCreateFromPathA(path.c_str(), url, &length, 0)))
        {
            throw std::runtime_error("Failed to create URL from path: " + path);
        }
        return std::string{url, length};
    }

    LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    // Create a hidden top-level window with a real client area for the GL
    // swapchain. Not shown -- the tool is headless. (No Playground/Shared helper
    // exists for this; the Embedding View requires a platform window handle.)
    HWND CreateOffscreenWindow(HINSTANCE instance, uint32_t width, uint32_t height)
    {
        static const wchar_t* className = L"ShaderCacheGeneratorWindow";

        WNDCLASSEXW wcex{};
        wcex.cbSize = sizeof(WNDCLASSEXW);
        wcex.lpfnWndProc = WndProc;
        wcex.hInstance = instance;
        wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wcex.lpszClassName = className;
        RegisterClassExW(&wcex);

        return CreateWindowW(
            className, L"ShaderCacheGenerator", WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, 0, static_cast<int>(width), static_cast<int>(height),
            nullptr, nullptr, instance, nullptr);
    }

    void PumpMessages()
    {
        MSG msg{};
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    // Read the current shader-cache entry count. The runtime is suspended first
    // so the JS thread's in-flight frame is closed and no shader compilation is
    // in progress while ShaderCache::Save walks the cache.
    uint32_t CountShaderCacheEntries(Babylon::Embedding::Runtime& runtime)
    {
        runtime.Suspend();
        std::ostringstream sink;
        const uint32_t count = Babylon::Plugins::ShaderCache::Save(sink);
        runtime.Resume();
        return count;
    }
}

int main(int argc, char** argv)
{
    // Console setup, crash handler, ANSI colors and the atexit finish-line hook.
    Diagnostics::Initialize();

    const char* argv0 = argc > 0 ? argv[0] : nullptr;

    PlaygroundOptions options = CommandLine::Parse(argc, argv);
    if (options.ShowHelp)
    {
        PrintUsage(argv0);
        Diagnostics::SetExitCode(0);
        return 0;
    }
    if (options.ParseError)
    {
        std::cerr << "Error: " << options.ErrorMessage << "\n\n";
        PrintUsage(argv0);
        Diagnostics::SetExitCode(2);
        return 2;
    }
    if (options.Scripts.size() != 2)
    {
        std::cerr << "Error: expected exactly two positional arguments "
                     "(<scene.js> <output.bin>).\n\n";
        PrintUsage(argv0);
        Diagnostics::SetExitCode(2);
        return 2;
    }

    const std::string scriptArg = options.Scripts[0];
    const std::filesystem::path outputPath = std::filesystem::path(options.Scripts[1]);

    std::error_code ec;
    const std::filesystem::path scriptAbsolute =
        std::filesystem::absolute(std::filesystem::path(scriptArg), ec);
    if (ec || !std::filesystem::exists(scriptAbsolute))
    {
        std::cerr << "Scene script not found: " << scriptArg << "\n";
        Diagnostics::SetExitCode(2);
        return 2;
    }

    std::string scriptUrl;
    try
    {
        scriptUrl = GetUrlFromPath(scriptAbsolute.string());
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << "\n";
        Diagnostics::SetExitCode(2);
        return 2;
    }

    const HINSTANCE instance = GetModuleHandleW(nullptr);
    const HWND window = CreateOffscreenWindow(instance, kWidth, kHeight);
    if (window == nullptr)
    {
        std::cerr << "Failed to create offscreen window.\n";
        Diagnostics::SetExitCode(1);
        return 1;
    }

    std::cout << "ShaderCacheGenerator\n"
              << "  script : " << scriptAbsolute.string() << "\n"
              << "  output : " << outputPath.string() << "\n"
              << "  size   : " << kWidth << "x" << kHeight << "\n";

    // PerfTrace and other process-wide Playground settings.
    Playground::Initialize(options);

    uint32_t finalCount = 0;
    {
        Babylon::Embedding::RuntimeOptions runtimeOptions{};
        runtimeOptions.enableDebugTrace = options.DebugTrace.value_or(false);
        // Do NOT set shaderCachePath: we manage Save() ourselves so the cache is
        // written exactly once, after the scene is ready. The Embedding layer
        // still enables the ShaderCache on the first View attach, so every
        // compiled shader is captured.
        runtimeOptions.log = Playground::MakeLogCallback([](std::string_view text) {
            std::string line{text};
            line.push_back('\n');
            OutputDebugStringA(line.c_str());
            std::fputs(line.c_str(), stdout);
        });

        Babylon::Embedding::Runtime runtime{std::move(runtimeOptions)};
        runtime.LoadScript(scriptUrl);

        Babylon::Embedding::View view{runtime, window};
        view.Resize(kWidth, kHeight, Babylon::Embedding::CoordinateUnits::Physical);

        uint32_t lastCount = 0;
        uint32_t stable = 0;
        bool ready = false;

        uint32_t frame = 0;
        for (; frame < kMaxFrames && !ready; ++frame)
        {
            PumpMessages();
            view.RenderFrame();
            ::Sleep(5); // give the JS thread time to run the render loop / compile shaders

            const bool isCheckFrame =
                frame >= kCheckInterval &&
                (frame % kCheckInterval) == 0 &&
                Babylon::Plugins::ShaderCache::IsEnabled();

            if (isCheckFrame)
            {
                const uint32_t count = CountShaderCacheEntries(runtime);
                const bool grew = (count != lastCount);
                if (count > 0 && !grew)
                {
                    ++stable;
                    if (stable >= kStableChecks)
                    {
                        ready = true;
                    }
                }
                else
                {
                    stable = 0;
                    lastCount = count;
                }
                std::cout << "  frame " << frame << ": " << count
                          << " shader(s) cached "
                          << (grew ? std::string{"(growing)"}
                                   : "(stable " + std::to_string(stable) + "/" +
                                         std::to_string(kStableChecks) + ")")
                          << "\n";
            }
        }

        if (ready)
        {
            std::cout << "Scene ready after " << frame << " frames.\n";
        }
        else
        {
            std::cout << "Reached frame cap (" << kMaxFrames
                      << ") before the shader cache stabilized; saving anyway.\n";
        }

        // Suspend once more so the write happens on a quiescent engine, then
        // persist the cache while it is still enabled (before ~Runtime).
        runtime.Suspend();
        std::ofstream outputFile{outputPath, std::ios::binary | std::ios::trunc};
        if (!outputFile.is_open())
        {
            std::cerr << "Failed to open output file for writing: "
                      << outputPath.string() << "\n";
            Diagnostics::SetExitCode(1);
            return 1;
        }
        finalCount = Babylon::Plugins::ShaderCache::Save(outputFile);
        outputFile.close();
    }

    if (finalCount == 0)
    {
        std::cerr << "No shaders were cached. The scene may not have rendered "
                     "anything, or shader compilation is disabled in this build.\n";
        Diagnostics::SetExitCode(1);
        return 1;
    }

    std::cout << "Wrote " << finalCount << " shader(s) to " << outputPath.string() << "\n";
    Diagnostics::SetExitCode(0);
    return 0;
}
