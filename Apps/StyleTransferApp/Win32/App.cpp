#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/ScriptLoader.h>
#include <Babylon/Plugins/ExternalTexture.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <Babylon/Plugins/NativeInput.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/Window.h>
#include <Babylon/Polyfills/XMLHttpRequest.h>

#include <winrt/base.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.AI.MachineLearning.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Graphics.Imaging.h>
#include <winrt/Windows.Media.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Graphics.DirectX.Direct3D11.h>
#include <Windows.graphics.directx.direct3d11.interop.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Web.Http.h>
#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.Foundation.Collections.h>

#include <PathCch.h>
#include <Windows.h>
#include <Windowsx.h>
#include <Shlwapi.h>
#include <filesystem>
#include <stdio.h>
#include <wrl.h>
#include <dxgi1_2.h>
#include <filesystem>

#include "resource.h"

using namespace winrt::Windows::AI::MachineLearning;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Graphics::Imaging;
using namespace winrt::Windows::Media;
using namespace winrt::Windows::Storage;
using namespace winrt::Windows::Graphics::DirectX::Direct3D11;
using namespace winrt::Windows::Graphics::DirectX;

using namespace Microsoft::WRL;
using namespace Windows::Graphics::DirectX::Direct3D11;

#define MAX_LOADSTRING 100

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

namespace
{
    // Global variables
    constexpr const uint32_t WIDTH = 720;
    constexpr const uint32_t HEIGHT = 720;

    const std::vector<winrt::hstring> g_models = {
        L"\\Models\\candy.onnx",
        L"\\Models\\la_muse.onnx",
        L"\\Models\\mosaic.onnx",
        L"\\Models\\udnie.onnx"};

    int g_selectedModel = 0;

    // Global Variables:
    HINSTANCE hInst;                     // current instance
    WCHAR szTitle[MAX_LOADSTRING];       // The title bar text
    WCHAR szWindowClass[MAX_LOADSTRING]; // the main window class name

    std::optional<Babylon::Graphics::Device> g_device{};
    std::optional<Babylon::Graphics::DeviceUpdate> g_update{};
    Babylon::Plugins::NativeInput* g_nativeInput{};
    std::optional<Babylon::AppRuntime> g_runtime{};
    bool g_minimized{false};
    winrt::com_ptr<ID3D11Texture2D> g_BabylonRenderTexture{};

    std::filesystem::path GetModulePath()
    {
        WCHAR modulePath[4096];
        DWORD result = GetModuleFileNameW(nullptr, modulePath, ARRAYSIZE(modulePath));
        winrt::check_bool(result != 0 && result != std::size(modulePath));
        return std::filesystem::path{modulePath}.parent_path();
    }

    HWND CreateAndShowWindow(HINSTANCE hInstance, int nCmdShow)
    {
        // Initialize global strings
        LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
        LoadStringW(hInstance, IDC_PLAYGROUNDWIN32, szWindowClass, MAX_LOADSTRING);

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

        RegisterClassExW(&wcex);

        HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, WIDTH, HEIGHT, nullptr, nullptr, hInstance, nullptr);

        if (!hWnd)
        {
            return FALSE;
        }

        ShowWindow(hWnd, nCmdShow);
        UpdateWindow(hWnd);
        EnableMouseInPointer(true);

        return hWnd;
    }

    // Creates D3D11 graphics objects from the IDirect3DDevice device created by WindowsML.
    void InitializeGraphicsInfra(_In_ HWND window,
        _In_ IDirect3DDevice device,
        _Out_ winrt::com_ptr<IDXGISwapChain1>& g_SwapChain,
        _Out_ winrt::com_ptr<ID3D11Device>& g_d3dDevice,
        _Out_ winrt::com_ptr<ID3D11DeviceContext>& g_d3dContext)
    {
        winrt::com_ptr<IDirect3DDxgiInterfaceAccess> dxgiInterfaceAccess{device.as<IDirect3DDxgiInterfaceAccess>()};
        winrt::com_ptr<IDXGIDevice2> dxgiDevice1;

        winrt::check_hresult(dxgiInterfaceAccess->GetInterface(IID_PPV_ARGS(&dxgiDevice1)));

        g_d3dDevice = dxgiDevice1.as<ID3D11Device>();

        g_d3dDevice->GetImmediateContext(g_d3dContext.put());

        DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
        swapChainDesc.Width = WIDTH; // use automatic sizing
        swapChainDesc.Height = HEIGHT;
        swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // this is the most common swapchain format
        swapChainDesc.Stereo = false;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT | DXGI_USAGE_UNORDERED_ACCESS;
        swapChainDesc.BufferCount = 2; // use double buffering to enable flip
        swapChainDesc.Scaling = DXGI_SCALING_NONE;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL; // all apps must use this SwapEffect
        swapChainDesc.Flags = 0;

        winrt::com_ptr<IDXGIAdapter> dxgiAdapter;
        winrt::check_hresult(dxgiDevice1->GetAdapter(dxgiAdapter.put()));

        winrt::com_ptr<IDXGIFactory2> dxgiFactory;
        winrt::check_hresult(dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory)));

        winrt::check_hresult(dxgiFactory->CreateSwapChainForHwnd(g_d3dDevice.get(), window, &swapChainDesc, nullptr, nullptr, g_SwapChain.put()));

        winrt::check_hresult(dxgiDevice1->SetMaximumFrameLatency(1));

        Microsoft::WRL::ComPtr<IDXGISurface1> dxgiBuffer;
        winrt::check_hresult(g_SwapChain->GetBuffer(0, __uuidof(IDXGISurface1), &dxgiBuffer));

        winrt::check_hresult(dxgiBuffer->QueryInterface(__uuidof(ID3D11Texture2D), g_BabylonRenderTexture.put_void()));
    }

    // Creates Babylon Native Graphics Device.
    std::optional<Babylon::Graphics::Device> CreateBabylonGraphicsDevice(ID3D11Device* d3dDevice)
    {
        Babylon::Graphics::Configuration config{};
        config.Device = d3dDevice;
        config.Width = WIDTH;
        config.Height = HEIGHT;
        return std::make_optional<Babylon::Graphics::Device>(config);
    }

    winrt::hstring GetInstalledLocation()
    {
        WCHAR modulePath[4096];
        DWORD result{::GetModuleFileNameW(nullptr, modulePath, ARRAYSIZE(modulePath))};
        winrt::check_bool(result != 0 && result != std::size(modulePath));
        winrt::check_hresult(PathCchRemoveFileSpec(modulePath, ARRAYSIZE(modulePath)));
        return modulePath;
    }

    // Creates a learning model session from a onnx file.
    LearningModelSession CreateModelSession(winrt::hstring modelLocalPath, LearningModelDevice device)
    {
        auto executablePath = GetInstalledLocation();
        auto finalPath = executablePath + modelLocalPath;
        auto model = LearningModel::LoadFromFilePath(finalPath.c_str());
        return LearningModelSession(model, device);
    }

    LearningModelBinding BindMLModel(LearningModelSession session)
    {
        auto outputFrame = VideoFrame::CreateAsDirect3D11SurfaceBacked(DirectXPixelFormat::B8G8R8A8UIntNormalized, WIDTH, HEIGHT, session.Device().Direct3D11Device());

        // now create a session and binding
        auto binding = LearningModelBinding{session};
        binding.Bind(L"outputImage", outputFrame);
        return binding;
    }

    VideoFrame RunModel(LearningModelSession session, LearningModelBinding binding, VideoFrame inputFrame)
    {
        // bind the input image
        binding.Bind(L"inputImage", inputFrame);

        // Run model
        auto results = session.Evaluate(binding, L"RunId");
        VideoFrame evalOutput = results.Outputs().Lookup(L"outputImage").try_as<VideoFrame>();
        return evalOutput;
    }

    void Uninitialize()
    {
        if (g_device)
        {
            g_update->Finish();
            g_device->FinishRenderingCurrentFrame();
        }

        g_nativeInput = {};
        g_runtime.reset();
        g_update.reset();
        g_device.reset();
    }

    void CopyTo(VideoFrame src, VideoFrame dst, winrt::com_ptr<ID3D11DeviceContext> d3d11Context)
    {
        auto srcD3DSurface = src.Direct3DSurface().as<IDirect3DDxgiInterfaceAccess>();
        auto dstD3DSurface = dst.Direct3DSurface().as<IDirect3DDxgiInterfaceAccess>();

        winrt::com_ptr<ID3D11Texture2D> srcTexture;
        winrt::check_hresult(srcD3DSurface->GetInterface(IID_PPV_ARGS(&srcTexture)));

        winrt::com_ptr<ID3D11Texture2D> dstTexture;
        winrt::check_hresult(dstD3DSurface->GetInterface(IID_PPV_ARGS(&dstTexture)));

        d3d11Context->CopyResource(dstTexture.get(), srcTexture.get());
    }

    void CopyTo(ID3D11Texture2D* src, VideoFrame dst, winrt::com_ptr<ID3D11DeviceContext> d3d11Context)
    {
        winrt::com_ptr<ID3D11Texture2D> dstTexture;
        winrt::check_hresult(dst.Direct3DSurface().as<IDirect3DDxgiInterfaceAccess>()->GetInterface(IID_PPV_ARGS(&dstTexture)));

        d3d11Context->CopyResource(dstTexture.get(), src);
    }

    void CopyTo(VideoFrame src, ID3D11Texture2D* dst, winrt::com_ptr<ID3D11DeviceContext> d3d11Context)
    {
        winrt::com_ptr<ID3D11Texture2D> srcTexture;
        winrt::check_hresult(src.Direct3DSurface().as<IDirect3DDxgiInterfaceAccess>()->GetInterface(IID_PPV_ARGS(&srcTexture)));

        d3d11Context->CopyResource(dst, srcTexture.get());
    }
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nCmdShow)
{
    winrt::init_apartment();

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    //------------- WinML intialization ------------------

    std::vector<LearningModelSession> MLSessions{};
    std::vector<LearningModelBinding> MLBindings{};
    LearningModelDevice learnDevice = LearningModelDevice(LearningModelDeviceKind::DirectXHighPerformance);
    VideoFrame inputFrame = VideoFrame::CreateAsDirect3D11SurfaceBacked(DirectXPixelFormat::B8G8R8A8UIntNormalized, WIDTH, HEIGHT, learnDevice.Direct3D11Device());

    for (size_t i = 0; i < g_models.size(); i++)
    {
        LearningModelSession session = CreateModelSession(g_models[i], learnDevice);
        auto binding = BindMLModel(session);
        MLSessions.push_back(session);
        MLBindings.push_back(binding);
    }

    //------------- D3D11 and application initialization ------------

    winrt::com_ptr<IDXGISwapChain1> swapChain{};
    winrt::com_ptr<ID3D11Device> d3d11Device{};
    winrt::com_ptr<ID3D11DeviceContext> d3d11Context{};

    // Create and show application window.
    HWND hWnd = CreateAndShowWindow(hInstance, nCmdShow);

    // Create D3D11 objects.
    InitializeGraphicsInfra(hWnd, learnDevice.Direct3D11Device(), swapChain, d3d11Device, d3d11Context);

    // --------------------- Babylon Native initialization --------------------------

    g_device = CreateBabylonGraphicsDevice(d3d11Device.get());
    g_update.emplace(g_device->GetUpdate("update"));

    // Start rendering a frame to unblock the JavaScript from queuing graphics
    // commands.
    g_device->StartRenderingCurrentFrame();
    g_update->Start();

    // Create a Babylon Native application runtime which hosts a JavaScript
    // engine on a new thread.
    g_runtime.emplace();

    g_runtime->Dispatch([](Napi::Env env) {
        // Add the Babylon Native graphics device to the JavaScript environment.
        g_device->AddToJavaScript(env);

        // Initialize the console polyfill.
        Babylon::Polyfills::Console::Initialize(env, [](const char* message, auto) {
            OutputDebugStringA(message);
        });

        // Initialize the window, XMLHttpRequest, and NativeEngine polyfills.
        Babylon::Polyfills::Window::Initialize(env);
        Babylon::Polyfills::XMLHttpRequest::Initialize(env);

        Babylon::Plugins::NativeEngine::Initialize(env);
        g_nativeInput = &Babylon::Plugins::NativeInput::CreateForJavaScript(env);
    });

    // Load the scripts for Babylon.js core and loaders plus this app's index.js.
    Babylon::ScriptLoader loader{*g_runtime};
    loader.LoadScript("app:///Scripts/babylon.max.js");
    loader.LoadScript("app:///Scripts/babylonjs.loaders.js");
    loader.LoadScript("app:///Scripts/index.js");

    std::promise<void> addToContext{};
    std::promise<void> startup{};

    // Create an external texture for the render target texture and pass it to
    // the `startup` JavaScript function.
    loader.Dispatch([externalTexture = Babylon::Plugins::ExternalTexture{g_BabylonRenderTexture.get()}, &addToContext, &startup](Napi::Env env) {
        auto jsPromise = externalTexture.AddToContextAsync(env);
        addToContext.set_value();

        jsPromise.Get("then").As<Napi::Function>().Call(jsPromise, {Napi::Function::New(env, [&startup](const Napi::CallbackInfo& info) {
            auto nativeTexture = info[0];
            info.Env().Global().Get("startup").As<Napi::Function>().Call(
                {
                    nativeTexture,
                    Napi::Value::From(info.Env(), WIDTH),
                    Napi::Value::From(info.Env(), HEIGHT),
                });
            startup.set_value();
        })});
    });

    // Wait for `AddToContextAsync` to be called.
    addToContext.get_future().wait();

    // Render a frame so that `AddToContextAsync` will complete.
    g_update->Finish();
    g_device->FinishRenderingCurrentFrame();

    // Wait for `startup` to finish.
    startup.get_future().wait();

    // --------------------------- Rendering loop -------------------------

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PLAYGROUNDWIN32));

    MSG msg{};

    g_device->StartRenderingCurrentFrame();
    g_update->Start();

    // Main message loop:
    while (msg.message != WM_QUIT)
    {
        BOOL result;

        if (g_minimized)
        {
            result = GetMessage(&msg, nullptr, 0, 0);
        }
        else
        {
            if (g_device)
            {
                // Finish Babylon Native rendering.
                g_update->Finish();
                g_device->FinishRenderingCurrentFrame();

                if (g_selectedModel >= 0)
                {
                    // Copy Babylon Native renderer to WinML input frame.
                    CopyTo(g_BabylonRenderTexture.get(), inputFrame, d3d11Context);

                    // Run Style Transfer model.
                    VideoFrame result = RunModel(MLSessions[g_selectedModel], MLBindings[g_selectedModel], inputFrame);

                    // Copy result back to Babylon Native render target.
                    CopyTo(result, g_BabylonRenderTexture.get(), d3d11Context);
                }

                // Present and start rendering next frame.
                swapChain->Present(1, 0);
                g_device->StartRenderingCurrentFrame();
                g_update->Start();
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

void ProcessMouseButtons(tagPOINTER_BUTTON_CHANGE_TYPE changeType, int x, int y)
{
    switch (changeType)
    {
        case POINTER_CHANGE_FIRSTBUTTON_DOWN:
            g_nativeInput->MouseDown(Babylon::Plugins::NativeInput::LEFT_MOUSE_BUTTON_ID, x, y);
            break;
        case POINTER_CHANGE_FIRSTBUTTON_UP:
            g_nativeInput->MouseUp(Babylon::Plugins::NativeInput::LEFT_MOUSE_BUTTON_ID, x, y);
            break;
        case POINTER_CHANGE_SECONDBUTTON_DOWN:
            g_nativeInput->MouseDown(Babylon::Plugins::NativeInput::RIGHT_MOUSE_BUTTON_ID, x, y);
            break;
        case POINTER_CHANGE_SECONDBUTTON_UP:
            g_nativeInput->MouseUp(Babylon::Plugins::NativeInput::RIGHT_MOUSE_BUTTON_ID, x, y);
            break;
        case POINTER_CHANGE_THIRDBUTTON_DOWN:
            g_nativeInput->MouseDown(Babylon::Plugins::NativeInput::MIDDLE_MOUSE_BUTTON_ID, x, y);
            break;
        case POINTER_CHANGE_THIRDBUTTON_UP:
            g_nativeInput->MouseUp(Babylon::Plugins::NativeInput::MIDDLE_MOUSE_BUTTON_ID, x, y);
            break;
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_SYSCOMMAND:
        {
            if ((wParam & 0xFFF0) == SC_MINIMIZE)
            {
                if (g_device)
                {
                    g_update->Finish();
                    g_device->FinishRenderingCurrentFrame();
                }

                g_runtime->Suspend();

                g_minimized = true;
            }
            else if ((wParam & 0xFFF0) == SC_RESTORE)
            {
                if (g_minimized)
                {
                    g_runtime->Resume();

                    g_minimized = false;

                    if (g_device)
                    {
                        g_device->StartRenderingCurrentFrame();
                        g_update->Start();
                    }
                }
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
                case IDM_EXIT:
                    DestroyWindow(hWnd);
                    break;
                default:
                    return DefWindowProc(hWnd, message, wParam, lParam);
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
                g_selectedModel = g_selectedModel < 3 ? (g_selectedModel + 1) : -1;
            }
            break;
        }
        case WM_POINTERWHEEL:
        {
            if (g_nativeInput != nullptr)
            {
                g_nativeInput->MouseWheel(Babylon::Plugins::NativeInput::MOUSEWHEEL_Y_ID, -GET_WHEEL_DELTA_WPARAM(wParam));
            }
            break;
        }
        case WM_POINTERDOWN:
        {
            if (g_nativeInput != nullptr)
            {
                POINTER_INFO info;
                auto pointerId = GET_POINTERID_WPARAM(wParam);

                if (GetPointerInfo(pointerId, &info))
                {
                    auto x = GET_X_LPARAM(lParam);
                    auto y = GET_Y_LPARAM(lParam);

                    if (info.pointerType == PT_MOUSE)
                    {
                        ProcessMouseButtons(info.ButtonChangeType, x, y);
                    }
                    else
                    {
                        g_nativeInput->TouchDown(pointerId, x, y);
                    }
                }
            }
            break;
        }
        case WM_POINTERUPDATE:
        {
            if (g_nativeInput != nullptr)
            {
                POINTER_INFO info;
                auto pointerId = GET_POINTERID_WPARAM(wParam);

                if (GetPointerInfo(pointerId, &info))
                {
                    auto x = GET_X_LPARAM(lParam);
                    auto y = GET_Y_LPARAM(lParam);

                    if (info.pointerType == PT_MOUSE)
                    {
                        ProcessMouseButtons(info.ButtonChangeType, x, y);
                        g_nativeInput->MouseMove(x, y);
                    }
                    else
                    {
                        g_nativeInput->TouchMove(pointerId, x, y);
                    }
                }
            }
            break;
        }
        case WM_POINTERUP:
        {
            if (g_nativeInput != nullptr)
            {
                POINTER_INFO info;
                auto pointerId = GET_POINTERID_WPARAM(wParam);

                if (GetPointerInfo(pointerId, &info))
                {
                    auto x = GET_X_LPARAM(lParam);
                    auto y = GET_Y_LPARAM(lParam);

                    if (info.pointerType == PT_MOUSE)
                    {
                        ProcessMouseButtons(info.ButtonChangeType, x, y);
                    }
                    else
                    {
                        g_nativeInput->TouchUp(pointerId, x, y);
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