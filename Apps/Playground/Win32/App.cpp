// App.cpp : Defines the entry point for the application.
//
// Two mutually-exclusive build configurations, selected by the CMake option
// BABYLON_NATIVE_PLUGIN_NATIVEDAWN:
//
//   * Default (bgfx / NativeEngine): built on Babylon::Embedding — the
//     cross-platform Runtime + View API handles plugin/polyfill setup, GPU
//     device construction, frame rendering, and input forwarding.
//
//   * NativeDawn (WebGPU via Dawn): the Embedding View always constructs the
//     bgfx Graphics device, which this backend replaces, so the Dawn host
//     below bypasses Embedding and drives a direct AppRuntime + ScriptLoader +
//     NativeDawn pipeline. It installs navigator.gpu, loads the same Babylon
//     bootstrap scripts, then runs the default scene (experience.js) on
//     WebGPU via dawn_bootstrap.js.

#if BABYLON_NATIVE_PLUGIN_NATIVEDAWN

#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Shell32.lib")

#include <Babylon/AppRuntime.h>
#include <Babylon/ScriptLoader.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/Window.h>
#include <Babylon/Polyfills/Performance.h>
#include <Babylon/Polyfills/Scheduling.h>
#include <Babylon/Polyfills/XMLHttpRequest.h>
#include <Babylon/Polyfills/Fetch.h>
#include <Babylon/Polyfills/Blob.h>
#include <Babylon/Polyfills/File.h>
#include <Babylon/Polyfills/TextDecoder.h>
#include <Babylon/Polyfills/TextEncoder.h>
#include <Babylon/Polyfills/AbortController.h>
#include <Babylon/Polyfills/URL.h>
#include <Babylon/Plugins/NativeDawn.h>

#include <Shared/Diagnostics.h>

#include <napi/napi.h>

#include <winrt/base.h>

#include <atomic>
#include <cstdio>
#include <filesystem>
#include <mutex>
#include <optional>
#include <string>
#include <vector>

namespace
{
    std::optional<Babylon::AppRuntime> g_runtime;
    std::optional<Babylon::ScriptLoader> g_loader;
    std::atomic<bool> g_frameInFlight{false};
    HWND g_hwnd{};

    // Win32 input + resize are produced on the host (message) thread and consumed
    // on the JS thread once per frame. A small mutex-protected queue marshals
    // pointer events; resize is coalesced to the latest pending size.
    struct PointerEvent
    {
        std::string type; // "pointerdown" | "pointermove" | "pointerup" | "wheel"
        int x = 0;
        int y = 0;
        int button = -1; // DOM button: 0 left, 1 middle, 2 right, -1 none
        int buttons = 0; // DOM buttons bitmask: 1 left, 2 right, 4 middle
        double deltaY = 0.0;
    };
    std::mutex g_inputMutex;
    std::vector<PointerEvent> g_pointerEvents;
    bool g_resizePending = false;
    uint32_t g_pendingWidth = 0;
    uint32_t g_pendingHeight = 0;

    int DomButtonsFromWParam(WPARAM wParam)
    {
        int buttons = 0;
        if (wParam & MK_LBUTTON) buttons |= 1;
        if (wParam & MK_RBUTTON) buttons |= 2;
        if (wParam & MK_MBUTTON) buttons |= 4;
        return buttons;
    }

    void PushPointerEvent(const char* type, int x, int y, int button, int buttons, double deltaY)
    {
        std::lock_guard<std::mutex> lock{g_inputMutex};
        g_pointerEvents.push_back(PointerEvent{type, x, y, button, buttons, deltaY});
    }

    std::filesystem::path ExeDir()
    {
        wchar_t buf[MAX_PATH]{};
        ::GetModuleFileNameW(nullptr, buf, MAX_PATH);
        return std::filesystem::path(buf).parent_path();
    }

    std::string FileUrl(const std::filesystem::path& path)
    {
        char url[2048];
        DWORD length = ARRAYSIZE(url);
        if (FAILED(::UrlCreateFromPathA(
                reinterpret_cast<const char*>(path.u8string().c_str()), url, &length, 0)))
        {
            return {};
        }
        return std::string(url, length);
    }

    // JSON/JS-escape a Windows filesystem path (backslashes) so it can be
    // embedded in a double-quoted JS string literal.
    std::string JsEscapePath(const std::filesystem::path& path)
    {
        std::string s = path.string();
        std::string out;
        for (char c : s)
        {
            if (c == '\\' || c == '"') out += '\\';
            out += c;
        }
        return out;
    }

    LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
            case WM_DESTROY:
                ::PostQuitMessage(0);
                return 0;

            case WM_SIZE:
            {
                if (wParam != SIZE_MINIMIZED)
                {
                    std::lock_guard<std::mutex> lock{g_inputMutex};
                    g_pendingWidth = static_cast<uint32_t>(LOWORD(lParam));
                    g_pendingHeight = static_cast<uint32_t>(HIWORD(lParam));
                    g_resizePending = true;
                }
                return 0;
            }

            case WM_MOUSEMOVE:
                PushPointerEvent("pointermove", GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam),
                    -1, DomButtonsFromWParam(wParam), 0.0);
                return 0;

            case WM_LBUTTONDOWN:
                ::SetCapture(hWnd);
                PushPointerEvent("pointerdown", GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam),
                    0, DomButtonsFromWParam(wParam), 0.0);
                return 0;
            case WM_LBUTTONUP:
                ::ReleaseCapture();
                PushPointerEvent("pointerup", GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam),
                    0, DomButtonsFromWParam(wParam), 0.0);
                return 0;

            case WM_RBUTTONDOWN:
                ::SetCapture(hWnd);
                PushPointerEvent("pointerdown", GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam),
                    2, DomButtonsFromWParam(wParam), 0.0);
                return 0;
            case WM_RBUTTONUP:
                ::ReleaseCapture();
                PushPointerEvent("pointerup", GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam),
                    2, DomButtonsFromWParam(wParam), 0.0);
                return 0;

            case WM_MBUTTONDOWN:
                ::SetCapture(hWnd);
                PushPointerEvent("pointerdown", GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam),
                    1, DomButtonsFromWParam(wParam), 0.0);
                return 0;
            case WM_MBUTTONUP:
                ::ReleaseCapture();
                PushPointerEvent("pointerup", GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam),
                    1, DomButtonsFromWParam(wParam), 0.0);
                return 0;

            case WM_MOUSEWHEEL:
            {
                // Wheel coords are in screen space; convert to client space.
                POINT pt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                ::ScreenToClient(hWnd, &pt);
                const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
                // DOM wheel deltaY is positive when scrolling down (away from user),
                // opposite sign to Win32's WHEEL_DELTA.
                PushPointerEvent("wheel", pt.x, pt.y, -1, 0, -static_cast<double>(delta));
                return 0;
            }

            default:
                return ::DefWindowProcW(hWnd, message, wParam, lParam);
        }
    }
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE,
    _In_ LPWSTR,
    _In_ int nCmdShow)
{
    ::SetConsoleOutputCP(CP_UTF8);
    std::setvbuf(stdout, nullptr, _IONBF, 0);
    std::setvbuf(stderr, nullptr, _IONBF, 0);

    Diagnostics::Initialize();
    std::fprintf(stderr, "[Playground] starting (NativeDawn / WebGPU, no bgfx)\n");

    // Window.
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = L"PlaygroundDawnWindow";
    ::RegisterClassExW(&wc);

    const uint32_t width = 1280;
    const uint32_t height = 720;
    g_hwnd = ::CreateWindowW(wc.lpszClassName, L"BabylonNative Playground (NativeDawn / WebGPU)",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        static_cast<int>(width), static_cast<int>(height),
        nullptr, nullptr, hInstance, nullptr);
    if (!g_hwnd)
    {
        std::fprintf(stderr, "[Playground] CreateWindow failed\n");
        return 1;
    }
    ::ShowWindow(g_hwnd, nCmdShow);
    ::UpdateWindow(g_hwnd);

    RECT rc{};
    ::GetClientRect(g_hwnd, &rc);
    const uint32_t clientW = static_cast<uint32_t>(rc.right - rc.left);
    const uint32_t clientH = static_cast<uint32_t>(rc.bottom - rc.top);

    // Runtime (V8). Uncaught JS exceptions go to the diagnostics banner.
    Babylon::AppRuntime::Options runtimeOptions{};
    runtimeOptions.UnhandledExceptionHandler = [](const Napi::Error& error) {
        const std::string message = Napi::GetErrorString(error);
        Diagnostics::DumpFailure("UNCAUGHT JS ERROR", nullptr, 0, 0, "%s", message.c_str());
        Diagnostics::SetExitCode(1);
        Diagnostics::PrintFinishLine();
        std::quick_exit(1);
    };
    g_runtime.emplace(std::move(runtimeOptions));
    g_loader.emplace(*g_runtime);

    // Install polyfills + NativeDawn on the JS thread (the Dawn device is
    // created here, on the JS thread, so all later Dawn calls stay on it).
    void* hwnd = g_hwnd;
    g_loader->Dispatch([hwnd, clientW, clientH](Napi::Env env) {
        // The JS thread needs a properly winrt-initialized apartment for
        // UrlLib's XMLHttpRequest backend (Windows.Foundation.Uri / HTTP);
        // the bare CoInitializeEx done by AppRuntime is not sufficient.
        try
        {
            winrt::init_apartment(winrt::apartment_type::single_threaded);
        }
        catch (const winrt::hresult_error&)
        {
        }

        Babylon::Polyfills::Console::Initialize(env,
            [](const char* message, Babylon::Polyfills::Console::LogLevel level) {
                std::FILE* out = level == Babylon::Polyfills::Console::LogLevel::Error ? stderr : stdout;
                std::fprintf(out, "%s", message ? message : "");
            });
        // Pre-define devicePixelRatio so the Window polyfill's Graphics(bgfx)-
        // backed accessor isn't the only source (it throws without a device).
        env.Global().Set("devicePixelRatio", Napi::Number::New(env, 1));
        Babylon::Polyfills::Window::Initialize(env);
        Babylon::Polyfills::Scheduling::Initialize(env);
        Babylon::Polyfills::Performance::Initialize(env);
        Babylon::Polyfills::TextDecoder::Initialize(env);
        Babylon::Polyfills::TextEncoder::Initialize(env);
        Babylon::Polyfills::Blob::Initialize(env);
        Babylon::Polyfills::File::Initialize(env);
        Babylon::Polyfills::URL::Initialize(env);
        Babylon::Polyfills::AbortController::Initialize(env);
        Babylon::Polyfills::XMLHttpRequest::Initialize(env);
        Babylon::Polyfills::Fetch::Initialize(env);
        Babylon::Plugins::NativeDawn::Initialize(env, hwnd, clientW, clientH);
    });

    // Resolve the scene script: default experience.js, or the first command-line
    // argument (a filesystem path) if provided.
    const std::filesystem::path scripts = ExeDir() / "Scripts";
    std::filesystem::path scenePath = scripts / "experience.js";
    {
        int argc = 0;
        LPWSTR* argv = ::CommandLineToArgvW(::GetCommandLineW(), &argc);
        if (argv != nullptr)
        {
            if (argc > 1)
            {
                std::filesystem::path arg(argv[1]);
                scenePath = arg.is_relative() ? (scripts / arg.filename()) : arg;
            }
            ::LocalFree(argv);
        }
    }

    // Load the Babylon bootstrap scripts (same set as the Embedding Playground),
    // skipping any that aren't staged. recast.js is intentionally omitted (asm.js
    // incompatible with v8jsi).
    const char* bootstrap[] = {
        "ammo.js",
        "babylon.max.js",
        "babylonjs.addons.js",
        "babylonjs.loaders.js",
        "babylonjs.materials.js",
        "babylon.gui.js",
        "meshwriter.min.js",
        "babylonjs.serializers.js",
    };
    for (const char* name : bootstrap)
    {
        const std::filesystem::path p = scripts / name;
        if (std::filesystem::exists(p))
        {
            g_loader->LoadScript(FileUrl(p));
        }
    }

    // Expose roots + the scene path (read natively by dawn_bootstrap.js so it can
    // eval the scene once the WebGPU engine is ready).
    {
        std::string scriptsRoot = FileUrl(scripts);
        if (!scriptsRoot.empty() && scriptsRoot.back() != '/') scriptsRoot += '/';
        std::string js = "globalThis.__scriptsRoot = \"" + scriptsRoot + "\";\n"
                         "globalThis.__sceneFsPath = \"" + JsEscapePath(scenePath) + "\";\n"
                         "globalThis.__sceneName = \"" + scenePath.filename().string() + "\";\n";
        g_loader->Eval(js, "Playground-dawn-roots.js");
    }

    // dawn_bootstrap.js pre-inits WebGPUEngine, aliases BABYLON.NativeEngine, and
    // evaluates the scene script once ready.
    g_loader->LoadScript(FileUrl(scripts / "dawn_bootstrap.js"));

    // Main loop: pump Win32 messages and dispatch one JS frame at a time
    // (throttled so the JS queue doesn't flood). The frame callback runs on the
    // JS thread, calls the JS frame() (which flushes requestAnimationFrame
    // callbacks and presents) and ticks Dawn.
    MSG msg{};
    while (msg.message != WM_QUIT)
    {
        if (::PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessageW(&msg);
            continue;
        }

        if (!g_frameInFlight.exchange(true))
        {
            g_loader->Dispatch([](Napi::Env env) {
                // Apply a coalesced resize (from WM_SIZE) before rendering so the
                // Dawn swapchain and the engine's back buffer match this frame.
                bool resized = false;
                uint32_t newW = 0;
                uint32_t newH = 0;
                std::vector<PointerEvent> events;
                {
                    std::lock_guard<std::mutex> lock{g_inputMutex};
                    if (g_resizePending)
                    {
                        g_resizePending = false;
                        resized = true;
                        newW = g_pendingWidth;
                        newH = g_pendingHeight;
                    }
                    events.swap(g_pointerEvents);
                }

                if (resized)
                {
                    Babylon::Plugins::NativeDawn::ResizeSurface(newW, newH);
                    Napi::Value resizeFn = env.Global().Get("__dawnResize");
                    if (resizeFn.IsFunction())
                    {
                        resizeFn.As<Napi::Function>().Call({
                            Napi::Number::New(env, newW),
                            Napi::Number::New(env, newH)});
                    }
                }

                if (!events.empty())
                {
                    Napi::Value inputFn = env.Global().Get("__dawnInput");
                    if (inputFn.IsFunction())
                    {
                        Napi::Function fn = inputFn.As<Napi::Function>();
                        for (const PointerEvent& e : events)
                        {
                            fn.Call({
                                Napi::String::New(env, e.type),
                                Napi::Number::New(env, e.x),
                                Napi::Number::New(env, e.y),
                                Napi::Number::New(env, e.button),
                                Napi::Number::New(env, e.buttons),
                                Napi::Number::New(env, e.deltaY)});
                        }
                    }
                }

                Napi::Value frame = env.Global().Get("frame");
                if (frame.IsFunction())
                {
                    frame.As<Napi::Function>().Call({});
                }
                Babylon::Plugins::NativeDawn::Tick(env);
                g_frameInFlight.store(false);
            });
        }
        else
        {
            ::Sleep(1);
        }
    }

    g_loader.reset();
    g_runtime.reset();
    Diagnostics::SetExitCode(static_cast<int>(msg.wParam));
    Diagnostics::PrintFinishLine();
    return static_cast<int>(msg.wParam);
}

#else // BABYLON_NATIVE_PLUGIN_NATIVEDAWN

#include "App.h"

#include <Babylon/Embedding/Runtime.h>
#include <Babylon/Embedding/View.h>
#include <Babylon/Plugins/TestUtils.h>

#include <Shared/CommandLine.h>
#include <Shared/Diagnostics.h>
#include <Shared/PlaygroundScripts.h>

#include <Windows.h>
#include <Windowsx.h>
#include <Shlwapi.h>

#include <napi/napi.h>

#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                     // current instance
WCHAR szTitle[MAX_LOADSTRING];       // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING]; // the main window class name

// Process-scoped: created on app start, recreated on 'R' refresh,
// destroyed on app exit.
std::optional<Babylon::Embedding::Runtime> g_runtime;

// Window-scoped: created on InitInstance after CreateWindowW returns,
// destroyed on WM_DESTROY (or torn down + recreated by RefreshBabylon).
std::optional<Babylon::Embedding::View> g_view;

bool minimized{false};
PlaygroundOptions options{};

// Forward declarations of functions included in this code module:
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

namespace
{
    std::string GetUrlFromPath(const std::filesystem::path& path)
    {
        char url[1024];
        DWORD length = ARRAYSIZE(url);
        HRESULT hr = UrlCreateFromPathA(reinterpret_cast<const char*>(path.u8string().data()), url, &length, 0);
        if (FAILED(hr))
        {
            throw std::exception("Failed to create url from path", hr);
        }

        return {url};
    }

    std::vector<std::string> GetCommandLineArguments()
    {
        int argc;
        auto argv = CommandLineToArgvW(GetCommandLineW(), &argc);

        std::vector<std::string> arguments{};
        arguments.reserve(argc);

        // Include argv[0]; CommandLine::Parse() skips it itself.
        for (int idx = 0; idx < argc; idx++)
        {
            std::wstring hstr{argv[idx]};
            int bytesRequired = ::WideCharToMultiByte(CP_UTF8, 0, &hstr[0], static_cast<int>(hstr.size()), nullptr, 0, nullptr, nullptr);
            arguments.push_back(std::string(bytesRequired, 0));
            ::WideCharToMultiByte(CP_UTF8, 0, hstr.data(), static_cast<int>(hstr.size()), arguments.back().data(), bytesRequired, nullptr, nullptr);
        }

        LocalFree(argv);

        return arguments;
    }

    Babylon::Embedding::RuntimeOptions MakeRuntimeOptions()
    {
        Babylon::Embedding::RuntimeOptions runtimeOptions{};
        runtimeOptions.enableDebugger = true;
        runtimeOptions.enableDebugTrace = options.DebugTrace.value_or(true);
        runtimeOptions.log = Playground::MakeLogCallback([](std::string_view text) {
            std::string line{text};
            line.push_back('\n');
            OutputDebugStringA(line.c_str());
            std::fputs(line.c_str(), stdout);
        });
        return runtimeOptions;
    }

    void QueuePlaygroundOptions()
    {
        g_runtime->RunOnJsThread([playgroundOptions = options](Napi::Env env) {
            auto js = Napi::Object::New(env);
            js.Set("listTests",          Napi::Boolean::New(env, playgroundOptions.ListTests));
            js.Set("headless",           Napi::Boolean::New(env, playgroundOptions.Headless));
            js.Set("breakOnFail",        Napi::Boolean::New(env, playgroundOptions.BreakOnFail));
            js.Set("generateReferences", Napi::Boolean::New(env, playgroundOptions.GenerateReferences));
            js.Set("runOnce",            Napi::Boolean::New(env, playgroundOptions.RunOnce));
            js.Set("includeExcluded",    Napi::Boolean::New(env, playgroundOptions.IncludeExcluded));
            if (playgroundOptions.SaveResults.has_value())
            {
                js.Set("saveResults", Napi::Boolean::New(env, *playgroundOptions.SaveResults));
            }
            if (playgroundOptions.CaptureFrame.has_value())
            {
                js.Set("captureFrame", Napi::Number::New(env, *playgroundOptions.CaptureFrame));
            }

            auto filters = Napi::Array::New(env, playgroundOptions.TestFilters.size());
            for (uint32_t idx = 0; idx < playgroundOptions.TestFilters.size(); ++idx)
            {
                filters[idx] = Napi::String::New(env, playgroundOptions.TestFilters[idx]);
            }
            js.Set("testFilters", filters);

            auto indices = Napi::Array::New(env, playgroundOptions.TestIndices.size());
            for (uint32_t idx = 0; idx < playgroundOptions.TestIndices.size(); ++idx)
            {
                indices[idx] = Napi::Number::New(env, playgroundOptions.TestIndices[idx]);
            }
            js.Set("testIndices", indices);

            env.Global().Set("_playgroundOptions", js);
        });
    }

    void LoadScripts()
    {
        Playground::LoadBootstrapScripts(*g_runtime);

        if (options.Scripts.empty())
        {
            g_runtime->LoadScript("app:///Scripts/experience.js");
        }
        else
        {
            for (const auto& arg : options.Scripts)
            {
                g_runtime->LoadScript(GetUrlFromPath(arg));
            }
            g_runtime->LoadScript("app:///Scripts/playground_runner.js");
        }
    }

    void Uninitialize()
    {
        // View first (unbinds surface, closes in-flight frame), then
        // Runtime (joins JS thread).
        g_view.reset();
        g_runtime.reset();
    }

    void RefreshBabylon(HWND hWnd)
    {
        Uninitialize();

        g_runtime.emplace(MakeRuntimeOptions());
        Playground::Initialize(options);
        QueuePlaygroundOptions();
        LoadScripts();

        // First View attach triggers Device construction, plugin init, and
        // flushes the queued scripts.
        g_view.emplace(*g_runtime, hWnd);

        // Drive the first Resize with the initial window bounds (physical pixels);
        // the View handles physical conversion via GetDevicePixelRatio.
        RECT rect{};
        if (GetClientRect(hWnd, &rect))
        {
            g_view->Resize(static_cast<uint32_t>(rect.right - rect.left),
                           static_cast<uint32_t>(rect.bottom - rect.top),
                           Babylon::Embedding::CoordinateUnits::Physical);
        }
    }
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // SUBSYSTEM:CONSOLE (see CMakeLists.txt) gives us inherited stdio.
    // UTF-8 output so callstacks / non-ASCII filenames survive.
    ::SetConsoleOutputCP(CP_UTF8);

    // Unbuffered stdout/stderr so the tail of the log reaches the pipe even
    // when we exit via std::quick_exit / _Exit / TestUtils.exit(). MSVC's
    // CRT aliases _IOLBF to _IOFBF, so _IONBF is the only correct choice.
    std::setvbuf(stdout, nullptr, _IONBF, 0);
    std::setvbuf(stderr, nullptr, _IONBF, 0);

    // Hook crash + assert handlers as early as possible.
    Diagnostics::Initialize();

    // Route TestUtils.exit(code) to the finish line. Fires on the JS thread
    // before the platform's default exit (quick_exit / PostMessage).
    Babylon::Plugins::TestUtils::SetExitCallback([](int code) {
        Diagnostics::SetExitCode(code);
        Diagnostics::PrintFinishLine();
    });

    // Parse argv before creating any window so --help / --list don't pop one.
    auto args = GetCommandLineArguments();
    std::vector<const char*> argv;
    argv.reserve(args.size());
    for (const auto& a : args)
    {
        argv.push_back(a.c_str());
    }
    options = CommandLine::Parse(static_cast<int>(argv.size()), argv.data());

    if (options.ParseError)
    {
        std::cerr << "Error: " << options.ErrorMessage << "\n\n";
        CommandLine::PrintUsage(argv.empty() ? nullptr : argv[0]);
        Diagnostics::SetExitCode(2);
        return 2;
    }

    if (options.ShowHelp)
    {
        CommandLine::PrintUsage(argv.empty() ? nullptr : argv[0]);
        Diagnostics::SetExitCode(0);
        return 0;
    }

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_PLAYGROUNDWIN32, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance(hInstance, options.Headless ? SW_HIDE : nCmdShow))
    {
        Diagnostics::SetExitCode(FALSE);
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PLAYGROUNDWIN32));

    MSG msg{};

    // Main message loop. When minimized, block on GetMessage to avoid
    // spinning the CPU. Otherwise, peek + render one frame per loop
    // iteration; View::RenderFrame is a no-op while suspended so we
    // don't need to special-case that here.
    while (msg.message != WM_QUIT)
    {
        BOOL result;

        if (minimized)
        {
            result = GetMessage(&msg, nullptr, 0, 0);
        }
        else
        {
            if (g_view)
            {
                g_view->RenderFrame();
            }
            result = PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) && msg.message != WM_QUIT;
        }

        if (result)
        {
            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }

    Diagnostics::SetExitCode(static_cast<int>(msg.wParam));
    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PLAYGROUNDWIN32));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_PLAYGROUNDWIN32);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    RefreshBabylon(hWnd);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    EnableMouseInPointer(true);

    return TRUE;
}

void ProcessMouseButtons(tagPOINTER_BUTTON_CHANGE_TYPE changeType, int x, int y)
{
    using View = Babylon::Embedding::View;
    using CoordinateUnits = Babylon::Embedding::CoordinateUnits;
    if (!g_view) return;

    switch (changeType)
    {
        case POINTER_CHANGE_FIRSTBUTTON_DOWN:
            g_view->OnMouseDown(View::LeftMouseButton(), static_cast<float>(x), static_cast<float>(y), CoordinateUnits::Physical);
            break;
        case POINTER_CHANGE_FIRSTBUTTON_UP:
            g_view->OnMouseUp(View::LeftMouseButton(), static_cast<float>(x), static_cast<float>(y), CoordinateUnits::Physical);
            break;
        case POINTER_CHANGE_SECONDBUTTON_DOWN:
            g_view->OnMouseDown(View::RightMouseButton(), static_cast<float>(x), static_cast<float>(y), CoordinateUnits::Physical);
            break;
        case POINTER_CHANGE_SECONDBUTTON_UP:
            g_view->OnMouseUp(View::RightMouseButton(), static_cast<float>(x), static_cast<float>(y), CoordinateUnits::Physical);
            break;
        case POINTER_CHANGE_THIRDBUTTON_DOWN:
            g_view->OnMouseDown(View::MiddleMouseButton(), static_cast<float>(x), static_cast<float>(y), CoordinateUnits::Physical);
            break;
        case POINTER_CHANGE_THIRDBUTTON_UP:
            g_view->OnMouseUp(View::MiddleMouseButton(), static_cast<float>(x), static_cast<float>(y), CoordinateUnits::Physical);
            break;
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    using View = Babylon::Embedding::View;
    using CoordinateUnits = Babylon::Embedding::CoordinateUnits;

    switch (message)
    {
        case WM_SYSCOMMAND:
        {
            if ((wParam & 0xFFF0) == SC_MINIMIZE)
            {
                if (g_runtime)
                {
                    g_runtime->Suspend();
                }
                minimized = true;
            }
            else if ((wParam & 0xFFF0) == SC_RESTORE)
            {
                if (minimized)
                {
                    minimized = false;
                    if (g_runtime)
                    {
                        g_runtime->Resume();
                    }
                }
            }
            DefWindowProc(hWnd, message, wParam, lParam);
            break;
        }
        case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            switch (wmId)
            {
                case IDM_ABOUT:
                    DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                    break;
                case IDM_EXIT:
                    DestroyWindow(hWnd);
                    break;
                default:
                    return DefWindowProc(hWnd, message, wParam, lParam);
            }
            break;
        }
        case WM_SIZE:
        {
            if (g_view)
            {
                g_view->Resize(static_cast<uint32_t>(LOWORD(lParam)),
                                static_cast<uint32_t>(HIWORD(lParam)),
                                CoordinateUnits::Physical);
            }
            break;
        }
        case WM_DESTROY:
        {
            Uninitialize();
            PostQuitMessage(0);
            break;
        }
        case WM_KEYDOWN:
        {
            if (wParam == 'R')
            {
                RefreshBabylon(hWnd);
            }
            break;
        }
        case WM_POINTERWHEEL:
        {
            if (g_view)
            {
                g_view->OnMouseWheel(View::MouseWheelY(), -GET_WHEEL_DELTA_WPARAM(wParam));
            }
            break;
        }
        case WM_POINTERDOWN:
        {
            if (g_view)
            {
                POINTER_INFO info;
                auto pointerId = GET_POINTERID_WPARAM(wParam);
                POINT origin{0, 0};

                if (GetPointerInfo(pointerId, &info) && ClientToScreen(hWnd, &origin))
                {
                    auto x = GET_X_LPARAM(lParam) - origin.x;
                    auto y = GET_Y_LPARAM(lParam) - origin.y;

                    if (info.pointerType == PT_MOUSE)
                    {
                        ProcessMouseButtons(info.ButtonChangeType, x, y);
                    }
                    else
                    {
                        g_view->OnPointerDown(static_cast<int32_t>(pointerId),
                                                static_cast<float>(x),
                                                static_cast<float>(y),
                                                CoordinateUnits::Physical);
                    }
                }
            }
            break;
        }
        case WM_POINTERUPDATE:
        {
            if (g_view)
            {
                auto pointerId = GET_POINTERID_WPARAM(wParam);
                POINTER_INFO info;
                POINT origin{0, 0};

                if (GetPointerInfo(pointerId, &info) && ClientToScreen(hWnd, &origin))
                {
                    auto x = GET_X_LPARAM(lParam) - origin.x;
                    auto y = GET_Y_LPARAM(lParam) - origin.y;

                    if (info.pointerType == PT_MOUSE)
                    {
                        ProcessMouseButtons(info.ButtonChangeType, x, y);
                        g_view->OnMouseMove(static_cast<float>(x), static_cast<float>(y), CoordinateUnits::Physical);
                    }
                    else
                    {
                        g_view->OnPointerMove(static_cast<int32_t>(pointerId),
                                                static_cast<float>(x),
                                                static_cast<float>(y),
                                                CoordinateUnits::Physical);
                    }
                }
            }
            break;
        }
        case WM_POINTERUP:
        {
            if (g_view)
            {
                auto pointerId = GET_POINTERID_WPARAM(wParam);
                POINTER_INFO info;
                POINT origin{0, 0};

                if (GetPointerInfo(pointerId, &info) && ClientToScreen(hWnd, &origin))
                {
                    auto x = GET_X_LPARAM(lParam) - origin.x;
                    auto y = GET_Y_LPARAM(lParam) - origin.y;

                    if (info.pointerType == PT_MOUSE)
                    {
                        ProcessMouseButtons(info.ButtonChangeType, x, y);
                    }
                    else
                    {
                        g_view->OnPointerUp(static_cast<int32_t>(pointerId),
                                              static_cast<float>(x),
                                              static_cast<float>(y),
                                              CoordinateUnits::Physical);
                    }
                }
            }
            break;
        }
        default:
        {
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
        case WM_INITDIALOG:
            return (INT_PTR)TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return (INT_PTR)TRUE;
            }
            break;
    }
    return (INT_PTR)FALSE;
}

#endif // BABYLON_NATIVE_PLUGIN_NATIVEDAWN
