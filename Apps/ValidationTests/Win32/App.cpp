// App.cpp : Defines the entry point for the application.
//

#include "App.h"
// NOMINMAX to prevent compilation errors with bgfx
#define NOMINMAX
#include <Windows.h>
#undef NOMINMAX
#include <Windowsx.h>
#include <Shlwapi.h>
#include <filesystem>

#include <Shared/TestUtils.h>

#include <Babylon/AppRuntime.h>
#include <Babylon/ScriptLoader.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <Babylon/Plugins/NativeWindow.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/Window.h>
#include <Babylon/Polyfills/XMLHttpRequest.h>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
std::unique_ptr<Babylon::AppRuntime> runtime{};

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

namespace
{
    std::filesystem::path GetModulePath()
    {
        char buffer[1024];
        ::GetModuleFileNameA(nullptr, buffer, ARRAYSIZE(buffer));
        return std::filesystem::path{ buffer };
    }

    std::string GetUrlFromPath(const std::filesystem::path& path)
    {
        char url[1024];
        DWORD length = ARRAYSIZE(url);
        HRESULT hr = UrlCreateFromPathA(path.u8string().data(), url, &length, 0);
        if (FAILED(hr))
        {
            throw std::exception("Failed to create url from path", hr);
        }

        return { url };
    }

    void Uninitialize()
    {
        if (runtime)
        {
            runtime.reset();
            Babylon::Plugins::NativeEngine::DeinitializeGraphics();
        }
    }

    void RefreshBabylon(HWND hWnd)
    {
        Uninitialize();

        runtime = std::make_unique<Babylon::AppRuntime>();

        RECT rect;
        if (!GetWindowRect(hWnd, &rect))
        {
            return;
        }

        // Initialize console plugin.
        runtime->Dispatch([rect, hWnd](Napi::Env env)
        {
            Babylon::Polyfills::Console::Initialize(env, [](const char* message, auto)
            {
                OutputDebugStringA(message);
            });

            Babylon::Polyfills::Window::Initialize(env);
            Babylon::Polyfills::XMLHttpRequest::Initialize(env);

            // Initialize NativeWindow plugin.
            auto width = static_cast<float>(rect.right - rect.left);
            auto height = static_cast<float>(rect.bottom - rect.top);
            Babylon::Plugins::NativeWindow::Initialize(env, hWnd, width, height);

            // Initialize NativeEngine plugin.
            Babylon::Plugins::NativeEngine::InitializeGraphics(hWnd, width, height);
            Babylon::Plugins::NativeEngine::Initialize(env);

            Babylon::TestUtils::CreateInstance(env, hWnd);
        });

        // Scripts are copied to the parent of the executable due to CMake issues.
        // See the CMakeLists.txt comments for more details.
        std::string scriptsRootUrl = GetUrlFromPath(GetModulePath().parent_path().parent_path() / "Scripts");

        Babylon::ScriptLoader loader{*runtime};
        loader.LoadScript(scriptsRootUrl + "/babylon.max.js");
        loader.LoadScript(scriptsRootUrl + "/babylon.glTF2FileLoader.js");
        loader.LoadScript(scriptsRootUrl + "/babylonjs.materials.js");
        loader.LoadScript(scriptsRootUrl + "/validation_native.js");
    }
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_VALIDATIONTESTSWIN32, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_VALIDATIONTESTSWIN32));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_VALIDATIONTESTSWIN32));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = 0;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   RefreshBabylon(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_SYSCOMMAND:
        {
            if ((wParam & 0xFFF0) == SC_MINIMIZE) 
            {
                runtime->Suspend();
            }
            else if ((wParam & 0xFFF0) == SC_RESTORE)
            {
                runtime->Resume();
            }
            DefWindowProc(hWnd, message, wParam, lParam);
            break;
        }
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
            break;
        }
        case WM_SIZE:
        {
            if (runtime != nullptr) {
                size_t width = static_cast<size_t>(LOWORD(lParam));
                size_t height = static_cast<size_t>(HIWORD(lParam));
                runtime->Dispatch([width, height](Napi::Env env)
                {
                    Babylon::Plugins::NativeWindow::UpdateSize(env, width, height);
                });
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
        default:
        {
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    return 0;
}
