// App.cpp : Defines the entry point for the application.
//
// Built on Babylon::Integrations: the cross-platform Runtime + View API
// handles plugin/polyfill setup, GPU device construction, frame rendering,
// and input forwarding.

#include "App.h"

#include <Babylon/Integrations/Runtime.h>
#include <Babylon/Integrations/View.h>
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
std::optional<Babylon::Integrations::Runtime> g_runtime;

// Window-scoped: created on InitInstance after CreateWindowW returns,
// destroyed on WM_DESTROY (or torn down + recreated by RefreshBabylon).
std::optional<Babylon::Integrations::View> g_view;

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

    Babylon::Integrations::RuntimeOptions MakeRuntimeOptions()
    {
        Babylon::Integrations::RuntimeOptions runtimeOptions{};
        runtimeOptions.enableDebugger = true;
        runtimeOptions.enableDebugTrace = options.DebugTrace.value_or(true);
        runtimeOptions.log = [](Babylon::Integrations::LogLevel level, std::string_view message) {
            std::string text{message};
            while (!text.empty() && (text.back() == '\n' || text.back() == '\r'))
            {
                text.pop_back();
            }

            std::string line{text};
            line.push_back('\n');
            OutputDebugStringA(line.c_str());
            std::fputs(line.c_str(), stdout);

            if (level == Babylon::Integrations::LogLevel::Error)
            {
                Diagnostics::DumpFailure(
                    "JS CONSOLE ERROR",
                    nullptr,
                    0,
                    0,
                    "%s",
                    text.c_str());
            }

            if (level == Babylon::Integrations::LogLevel::Fatal)
            {
                Diagnostics::DumpFailure(
                    "UNCAUGHT JS ERROR",
                    nullptr,
                    0,
                    0,
                    "%s",
                    text.c_str());
                Diagnostics::SetExitCode(1);
                Diagnostics::PrintFinishLine();
                std::quick_exit(1);
            }
        };
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
                           Babylon::Integrations::CoordinateUnits::Physical);
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
    using View = Babylon::Integrations::View;
    using CoordinateUnits = Babylon::Integrations::CoordinateUnits;
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
    using View = Babylon::Integrations::View;
    using CoordinateUnits = Babylon::Integrations::CoordinateUnits;

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
