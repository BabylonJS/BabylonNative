// App.cpp : Defines the entry point for the application.
//

#include "App.h"
#include "Utility.h"

#include <Windows.h>
#include <Windowsx.h>
#include <Shlwapi.h>
#include <filesystem>
#include <stdio.h>

#include <d3d11_1.h>
#include <directxcolors.h>

#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/GraphicsInterops/ExternalTexture.h>
#include <Babylon/ScriptLoader.h>
#include <Babylon/Plugins/NativeCapture.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <Babylon/Plugins/NativeOptimizations.h>
#include <Babylon/Plugins/ChromeDevTools.h>
#include <Babylon/Plugins/NativeXr.h>
#include <Babylon/Plugins/NativeCamera.h>
#include <Babylon/Plugins/NativeInput.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/Window.h>
#include <Babylon/Polyfills/XMLHttpRequest.h>
#include <Babylon/Polyfills/Canvas.h>

#include <iostream>

#include <stb_image.h>

ID3D11Device* g_d3dDevice;
IDXGISwapChain* g_SwapChain;
ID3D11Texture2D* g_DepthStencilBuffer;
ID3D11RenderTargetView* g_RenderTargetView;
ID3D11DepthStencilView* g_DepthStencilView;
ID3D11Texture2D* g_Texture2D;

std::unique_ptr<Babylon::AppRuntime> runtime{};
std::unique_ptr<Babylon::Graphics::Device> device{};
Napi::Value deviceContext;
std::unique_ptr<Babylon::Graphics::DeviceUpdate> update{};
std::unique_ptr<Babylon::Graphics::ExternalTexture> externalTexture{};
Babylon::Plugins::NativeInput* nativeInput{};
std::unique_ptr<Babylon::Plugins::ChromeDevTools> chromeDevTools{};
std::unique_ptr<Babylon::Polyfills::Canvas> nativeCanvas{};

uint32_t m4xMsaaQuality;
bool mEnable4xMsaa;

HWND mhMainWnd;

#define MAX_LOADSTRING 100

#define ReleaseCOM(x)     \
    {                     \
        if (x)            \
        {                 \
            x->Release(); \
            x = 0;        \
        }                 \
    } 

// Global Variables:
HINSTANCE hInst;                     // current instance
WCHAR szTitle[MAX_LOADSTRING];       // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING]; // the main window class name

// Forward declarations of functions included in this code module:
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

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


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

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

    // Main message loop:
    while (msg.message != WM_QUIT)
    {
        BOOL result;

        if (device)
        {
            update->Finish();
            device->FinishRenderingCurrentFrame();
            
            ASSERT_SUCCEEDED(g_SwapChain->Present(0, 0), "Fail to present");

            device->StartRenderingCurrentFrame();
            update->Start();
        }

        result = PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) && msg.message != WM_QUIT;

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

namespace
{
    void LoadD3D11ExternalTexture() 
    {
        int width, height, nrChannels;
        unsigned char* data = stbi_load("Resources/external.png", &width, &height, &nrChannels, 0);

        ASSERT(data != nullptr, "Fail to load external.png");

        D3D11_TEXTURE2D_DESC desc = {};
        desc.Width = static_cast<uint32_t>(width);
        desc.Height = static_cast<uint32_t>(height);
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_IMMUTABLE;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

        uint32_t pitch = nrChannels * width;

        D3D11_SUBRESOURCE_DATA initData = {data, pitch, 0};

        ASSERT_SUCCEEDED(g_d3dDevice->CreateTexture2D(&desc, &initData, &g_Texture2D));

        Babylon::Graphics::ExternalTextureCreateInfo textureInfo;
        textureInfo.Texture = g_Texture2D;
        textureInfo.Device = device.get();

        externalTexture = std::make_unique<Babylon::Graphics::ExternalTexture>(textureInfo);
    }

    void InitD3D11()
    {
        UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
        createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        D3D_FEATURE_LEVEL featureLevel;

        HRESULT hr = D3D11CreateDevice(
            0,
            D3D_DRIVER_TYPE_HARDWARE,
            0,
            createDeviceFlags,
            0, 0,
            D3D11_SDK_VERSION,
            &g_d3dDevice,
            &featureLevel,
            NULL);

        ASSERT_SUCCEEDED(hr, "Fail to create D3DDevice");

        ASSERT(featureLevel == D3D_FEATURE_LEVEL_11_0, "Fail DirectX 11 not supported.");

        // Check 4X MSAA quality support for our back buffer format.
        // All Direct3D 11 capable devices support 4X MSAA for all render
        // target formats, so we only need to check quality support.

        ASSERT_SUCCEEDED(g_d3dDevice->CheckMultisampleQualityLevels(
                             DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m4xMsaaQuality),
            "Unable to get Msaa quality level.")

        assert(m4xMsaaQuality > 0);

        // Fill out a DXGI_SWAP_CHAIN_DESC to describe our swap chain.
        RECT rect;
        if (!GetClientRect(mhMainWnd, &rect))
        {
            return;
        }

        auto width = static_cast<size_t>(rect.right - rect.left);
        auto height = static_cast<size_t>(rect.bottom - rect.top);

        DXGI_SWAP_CHAIN_DESC sd;
        sd.BufferDesc.Width = static_cast<uint32_t>(width);
        sd.BufferDesc.Height = static_cast<uint32_t>(height);
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

        // Use 4X MSAA?
        if (mEnable4xMsaa)
        {
            sd.SampleDesc.Count = 4;
            sd.SampleDesc.Quality = m4xMsaaQuality - 1;
        }
        // No MSAA
        else
        {
            sd.SampleDesc.Count = 1;
            sd.SampleDesc.Quality = 0;
        }

        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.BufferCount = 1;
        sd.OutputWindow = mhMainWnd;
        sd.Windowed = true;
        sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        sd.Flags = 0;

        // To correctly create the swap chain, we must use the IDXGIFactory that was
        // used to create the device.  If we tried to use a different IDXGIFactory instance
        // (by calling CreateDXGIFactory), we get an error: "IDXGIFactory::CreateSwapChain:
        // This function is being called with a device from a different IDXGIFactory."

        IDXGIDevice* dxgiDevice = 0;
        ASSERT_SUCCEEDED(g_d3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice));

        IDXGIAdapter* dxgiAdapter = 0;
        ASSERT_SUCCEEDED(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));

        IDXGIFactory* dxgiFactory = 0;
        ASSERT_SUCCEEDED(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory));

        ASSERT_SUCCEEDED(dxgiFactory->CreateSwapChain(g_d3dDevice, &sd, &g_SwapChain));

        ReleaseCOM(dxgiDevice);
        ReleaseCOM(dxgiAdapter);
        ReleaseCOM(dxgiFactory);

        ID3D11Texture2D* backBuffer;

        ASSERT_SUCCEEDED(g_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)), "Fail to get swapchain buffer.");

        ASSERT_SUCCEEDED(g_d3dDevice->CreateRenderTargetView(backBuffer, 0, &g_RenderTargetView), "Fail to create render target view.");

        ReleaseCOM(backBuffer);

        // Create the depth/stencil buffer and view.

        D3D11_TEXTURE2D_DESC depthStencilDesc;

        depthStencilDesc.Width = static_cast<uint32_t>(width);
        depthStencilDesc.Height = static_cast<uint32_t>(height);
        depthStencilDesc.MipLevels = 1;
        depthStencilDesc.ArraySize = 1;
        depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

        // Use 4X MSAA? --must match swap chain MSAA values.
        if (mEnable4xMsaa)
        {
            depthStencilDesc.SampleDesc.Count = 4;
            depthStencilDesc.SampleDesc.Quality = m4xMsaaQuality - 1;
        }
        // No MSAA
        else
        {
            depthStencilDesc.SampleDesc.Count = 1;
            depthStencilDesc.SampleDesc.Quality = 0;
        }

        depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
        depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        depthStencilDesc.CPUAccessFlags = 0;
        depthStencilDesc.MiscFlags = 0;

        ASSERT_SUCCEEDED(g_d3dDevice->CreateTexture2D(&depthStencilDesc, 0, &g_DepthStencilBuffer));

        ASSERT_SUCCEEDED(g_d3dDevice->CreateDepthStencilView(g_DepthStencilBuffer, 0, &g_DepthStencilView));
    }

    void UpdateWindowSize(size_t width, size_t height)
    {
        update->Finish();
        device->FinishRenderingCurrentFrame();

        assert(g_d3dDevice);
        assert(g_SwapChain);

        // Release the old views, as they hold references to the buffers we
        // will be destroying.  Also release the old depth/stencil buffer.

        ReleaseCOM(g_RenderTargetView);
        ReleaseCOM(g_DepthStencilView);
        ReleaseCOM(g_DepthStencilBuffer);

        // Resize the swap chain and recreate the render target view.

        ASSERT_SUCCEEDED(g_SwapChain->ResizeBuffers(1, static_cast<uint32_t>(width), static_cast<uint32_t>(height), DXGI_FORMAT_R8G8B8A8_UNORM, 0), "Fail to resize swapchain.");

        ID3D11Texture2D* backBuffer;
        
        ASSERT_SUCCEEDED(g_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)), "Fail to get swapchain buffer.");
        
        ASSERT_SUCCEEDED(g_d3dDevice->CreateRenderTargetView(backBuffer, 0, &g_RenderTargetView), "Fail to create render target view.");

        ReleaseCOM(backBuffer);

        // Create the depth/stencil buffer and view.

        D3D11_TEXTURE2D_DESC depthStencilDesc;

        depthStencilDesc.Width = static_cast<uint32_t>(width);
        depthStencilDesc.Height = static_cast<uint32_t>(height);
        depthStencilDesc.MipLevels = 1;
        depthStencilDesc.ArraySize = 1;
        depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

        // Use 4X MSAA? --must match swap chain MSAA values.
        if (mEnable4xMsaa)
        {
            depthStencilDesc.SampleDesc.Count = 4;
            depthStencilDesc.SampleDesc.Quality = m4xMsaaQuality - 1;
        }
        // No MSAA
        else
        {
            depthStencilDesc.SampleDesc.Count = 1;
            depthStencilDesc.SampleDesc.Quality = 0;
        }

        depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
        depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        depthStencilDesc.CPUAccessFlags = 0;
        depthStencilDesc.MiscFlags = 0;

        ASSERT_SUCCEEDED(g_d3dDevice->CreateTexture2D(&depthStencilDesc, 0, &g_DepthStencilBuffer));

        ASSERT_SUCCEEDED(g_d3dDevice->CreateDepthStencilView(g_DepthStencilBuffer, 0, &g_DepthStencilView));

        Babylon::Graphics::BackBufferUpdateInfo backBufferUpdate{};
        backBufferUpdate.Backbuffer = g_RenderTargetView;
        backBufferUpdate.DepthStencil = g_DepthStencilView;

        device->UpdateBackbuffer(backBufferUpdate);

        // Bind the render target view and depth/stencil view to the pipeline.
        device->UpdateSize(width, height);

        device->StartRenderingCurrentFrame();
        update->Start();
    }

    void Uninitialize()
    {
        if (device)
        {
            update->Finish();
            device->FinishRenderingCurrentFrame();
        }

        chromeDevTools.reset();
        nativeInput = {};
        runtime.reset();
        nativeCanvas.reset();
        update.reset();
        device.reset();
    }

    float InitializeDPIScale(HWND hwnd)
    {
        HDC hdc = GetDC(hwnd);
        auto g_DPIScaleX = GetDeviceCaps(hdc, LOGPIXELSX) / 96.0f;
        //auto g_DPIScaleY = GetDeviceCaps(hdc, LOGPIXELSY) / 96.0f;
        ReleaseDC(hwnd, hdc);
        return g_DPIScaleX;
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

    void BabylonJs_OnReady() 
    {
        runtime->Dispatch([](Napi::Env env)
            {
                auto jsObject = externalTexture->AddToContext(deviceContext);

                auto width = externalTexture->GetWight();
                auto height = externalTexture->GetHeight();

                // Tell the JS side about the texture.
                env.Global().Get("loadNativeTexture").As<Napi::Function>().Call({
                    // Wrap the texture handle into a JavaScript type that can be consumed by the native engine.
                    // This transfers ownership of the texture to the Babylon ThinTexture on the js side.
                    // Important: We cannot do this twice with the same native texture handle.
                    jsObject,
                    Napi::Value::From(env, width),
                    Napi::Value::From(env, height),
                });

                env.Global().Get("startRender").As<Napi::Function>().Call({});
            });
    }

    void RefreshBabylon(HWND hWnd) 
    {
        Uninitialize();

        Babylon::Graphics::DeviceConfiguration config;
        config.Device = g_d3dDevice;
        config.DevicePixelRatio = InitializeDPIScale(hWnd);
        config.Backbuffer = g_RenderTargetView;
        config.DepthStencil = g_DepthStencilView;

        device = Babylon::Graphics::Device::Create(config);
        
        update = std::make_unique<Babylon::Graphics::DeviceUpdate>(device->GetUpdate("update"));

        runtime = std::make_unique<Babylon::AppRuntime>();

        runtime->Dispatch([](Napi::Env env)
            {
                device->AddToJavaScript(env);
                deviceContext = device->CreateContext(env);

                auto envGlobal = env.Global();

                envGlobal.Set("BabylonJs_OnReady", Napi::Function::New(
                                                       env, [](const Napi::CallbackInfo&)
                                                       { BabylonJs_OnReady(); },
                                                       "BabylonJs_OnReady"));

                Babylon::Polyfills::Console::Initialize(env, [](const char* message, auto)
                    { OutputDebugStringA(message); });

                Babylon::Polyfills::Window::Initialize(env);

                Babylon::Polyfills::XMLHttpRequest::Initialize(env);
                nativeCanvas = std::make_unique<Babylon::Polyfills::Canvas>(Babylon::Polyfills::Canvas::Initialize(env));

                Babylon::Plugins::NativeEngine::Initialize(env);

                Babylon::Plugins::NativeOptimizations::Initialize(env);

                Babylon::Plugins::NativeCapture::Initialize(env);

                // Initialize Camera
                Babylon::Plugins::Camera::Initialize(env);

                // Initialize NativeXr plugin.
                Babylon::Plugins::NativeXr::Initialize(env);

                nativeInput = &Babylon::Plugins::NativeInput::CreateForJavaScript(env);

                chromeDevTools = std::make_unique<Babylon::Plugins::ChromeDevTools>(Babylon::Plugins::ChromeDevTools::Initialize(env));
                if (chromeDevTools->SupportsInspector())
                {
                    chromeDevTools->StartInspector(5643, "BabylonNative Playground");
                }
            });

        Babylon::ScriptLoader loader{*runtime};
        loader.Eval("document = {}", "");
        loader.LoadScript("app:///Scripts/ammo.js");
        // Commenting out recast.js for now because v8jsi is incompatible with asm.js.
        // loader.LoadScript("app:///Scripts/recast.js");
        loader.LoadScript("app:///Scripts/babylon.max.js");
        loader.LoadScript("app:///Scripts/babylonjs.loaders.js");
        loader.LoadScript("app:///Scripts/babylonjs.materials.js");
        loader.LoadScript("app:///Scripts/babylon.gui.js");
        loader.LoadScript("app:///Scripts/meshwriter.min.js");

        std::vector<std::string> scripts = GetCommandLineArguments();
        
        if (scripts.empty())
        {
            loader.LoadScript("app:///Scripts/slideShow_experience.js");
        }
        else
        {
            for (const auto& script : scripts)
            {
                loader.LoadScript(GetUrlFromPath(script));
            }

            loader.LoadScript("app:///Scripts/playground_runner.js");
        }
    }
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Store instance handle in our global variable

    mhMainWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!mhMainWnd)
    {
        return FALSE;
    }

    ShowWindow(mhMainWnd, nCmdShow);
    UpdateWindow(mhMainWnd);
    InitD3D11();
    RefreshBabylon(mhMainWnd);

    RECT rect;
    if (!GetClientRect(mhMainWnd, &rect))
    {
        return false;
    }

    auto width = static_cast<size_t>(rect.right - rect.left);
    auto height = static_cast<size_t>(rect.bottom - rect.top);
    device->UpdateSize(width, height);
    device->EnableRendering();

    LoadD3D11ExternalTexture();

    device->StartRenderingCurrentFrame();
    update->Start();


    // The remaining steps that need to be carried out for d3d creation
    // also need to be executed every time the window is resized.  So
    // just call the OnResize method here to avoid code duplication.
    //UpdateWindowSize(width, height);

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_SYSCOMMAND:
        {
            if ((wParam & 0xFFF0) == SC_MINIMIZE)
            {
                if (device)
                {
                    update->Finish();
                    device->FinishRenderingCurrentFrame();
                }

                runtime->Suspend();
            }
            else if ((wParam & 0xFFF0) == SC_RESTORE)
            {
            }
            DefWindowProc(hWnd, message, wParam, lParam);
            break;
        }
        case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
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
            if (device)
            {
                auto width = static_cast<size_t>(LOWORD(lParam));
                auto height = static_cast<size_t>(HIWORD(lParam));
                UpdateWindowSize(width, height);
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
        case WM_MOUSEMOVE:
        {
            if (nativeInput != nullptr)
            {
                nativeInput->MouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            }
            break;
        }
        case WM_LBUTTONDOWN:
        {
            SetCapture(hWnd);
            if (nativeInput != nullptr)
            {
                nativeInput->MouseDown(0, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            }
            break;
        }
        case WM_LBUTTONUP:
        {
            if (nativeInput != nullptr)
            {
                nativeInput->MouseUp(0, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            }
            ReleaseCapture();
            break;
        }
        default:
        {
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    return 0;
}

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
