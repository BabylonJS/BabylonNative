// App.cpp : Defines the entry point for the application.
//
// Migrated to Babylon::Integrations: this host no longer constructs
// Babylon Native components directly. The cross-platform `Runtime` +
// `View` API handles plugin/polyfill setup, GPU device construction,
// frame rendering, and input forwarding.

#include "App.h"

#include <Babylon/Integrations/Runtime.h>
#include <Babylon/Integrations/View.h>

#include <Shared/PlaygroundScripts.h>

#include <Windows.h>
#include <Windowsx.h>
#include <Shlwapi.h>

#include <filesystem>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                     // current instance
WCHAR szTitle[MAX_LOADSTRING];       // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING]; // the main window class name

// Process-scoped: created on app start, recreated on 'R' refresh,
// destroyed on app exit.
std::unique_ptr<Babylon::Integrations::Runtime> g_runtime;

// Window-scoped: created on InitInstance after CreateWindowW returns,
// destroyed on WM_DESTROY (or torn down + recreated by RefreshBabylon).
std::unique_ptr<Babylon::Integrations::View> g_view;

bool minimized{false};

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

        for (int idx = 1; idx < argc; idx++)
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
        Babylon::Integrations::RuntimeOptions options{};
        options.enableDebugger = true;     // matches AppContext default
        options.log = [](Babylon::Integrations::LogLevel level, std::string_view message) {
            std::ostringstream ss{};
            ss << message << std::endl;
            OutputDebugStringA(ss.str().data());
            std::cout << ss.str();

            // Match AppContext's historical behavior: terminate on
            // uncaught JS exceptions (Fatal). Routine console.error
            // calls (Error) just print and continue.
            if (level == Babylon::Integrations::LogLevel::Fatal)
            {
                std::quick_exit(1);
            }
        };
        return options;
    }

    void QueueScripts()
    {
        // Babylon.js bootstrap (core + loaders/materials/gui/serializers).
        // Shared with the other Playground hosts via Shared/PlaygroundScripts.
        Playground::LoadBootstrapScripts(*g_runtime);

        const auto args = GetCommandLineArguments();
        if (args.empty())
        {
            g_runtime->LoadScript("app:///Scripts/experience.js");
        }
        else
        {
            for (const auto& arg : args)
            {
                g_runtime->LoadScript(GetUrlFromPath(arg));
            }
            g_runtime->LoadScript("app:///Scripts/playground_runner.js");
        }
    }

    Babylon::Integrations::ViewDescriptor DescribeWindow(HWND hWnd)
    {
        RECT rect;
        if (!GetClientRect(hWnd, &rect))
        {
            throw std::exception{"Unable to get client rect"};
        }
        Babylon::Integrations::ViewDescriptor descriptor{};
        descriptor.nativeWindow = hWnd;
        descriptor.width = static_cast<uint32_t>(rect.right - rect.left);
        descriptor.height = static_cast<uint32_t>(rect.bottom - rect.top);
        return descriptor;
    }

    void Uninitialize()
    {
        // Destroy in reverse-construction order: View first (so the
        // surface is unbound and the in-flight frame is closed), then
        // Runtime (which joins the JS thread).
        g_view.reset();
        g_runtime.reset();
    }

    void RefreshBabylon(HWND hWnd)
    {
        Uninitialize();

        g_runtime = Babylon::Integrations::Runtime::Create(MakeRuntimeOptions());
        QueueScripts();

        // First View::Attach triggers GPU device construction, plugin
        // initialization on the JS thread, and flushes the queued
        // scripts.
        g_view = Babylon::Integrations::View::Attach(*g_runtime, DescribeWindow(hWnd));
    }
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Process-wide Playground setup (PerfTrace level, etc.). Shared
    // with the other Playground hosts via Shared/PlaygroundScripts.
    Playground::Initialize();

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_PLAYGROUNDWIN32, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow))
    {
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

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    EnableMouseInPointer(true);

    RefreshBabylon(hWnd);

    return TRUE;
}

void ProcessMouseButtons(tagPOINTER_BUTTON_CHANGE_TYPE changeType, int x, int y)
{
    using View = Babylon::Integrations::View;
    if (!g_view) return;

    switch (changeType)
    {
        case POINTER_CHANGE_FIRSTBUTTON_DOWN:
            g_view->OnMouseDown(View::LeftMouseButton(), static_cast<float>(x), static_cast<float>(y));
            break;
        case POINTER_CHANGE_FIRSTBUTTON_UP:
            g_view->OnMouseUp(View::LeftMouseButton(), static_cast<float>(x), static_cast<float>(y));
            break;
        case POINTER_CHANGE_SECONDBUTTON_DOWN:
            g_view->OnMouseDown(View::RightMouseButton(), static_cast<float>(x), static_cast<float>(y));
            break;
        case POINTER_CHANGE_SECONDBUTTON_UP:
            g_view->OnMouseUp(View::RightMouseButton(), static_cast<float>(x), static_cast<float>(y));
            break;
        case POINTER_CHANGE_THIRDBUTTON_DOWN:
            g_view->OnMouseDown(View::MiddleMouseButton(), static_cast<float>(x), static_cast<float>(y));
            break;
        case POINTER_CHANGE_THIRDBUTTON_UP:
            g_view->OnMouseUp(View::MiddleMouseButton(), static_cast<float>(x), static_cast<float>(y));
            break;
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    using View = Babylon::Integrations::View;

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
                                static_cast<uint32_t>(HIWORD(lParam)));
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
                                                static_cast<float>(y));
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
                        g_view->OnMouseMove(static_cast<float>(x), static_cast<float>(y));
                    }
                    else
                    {
                        g_view->OnPointerMove(static_cast<int32_t>(pointerId),
                                                static_cast<float>(x),
                                                static_cast<float>(y));
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
                                              static_cast<float>(y));
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
