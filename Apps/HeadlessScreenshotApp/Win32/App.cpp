#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/ScriptLoader.h>
#include <Babylon/Plugins/ExternalTexture.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/Window.h>
#include <Babylon/Polyfills/XMLHttpRequest.h>

#include <winrt/base.h>

#include <WICTextureLoader.h>
#include <ScreenGrab.h>
#include <wincodec.h>

#include <filesystem>
#include <iostream>

#include "RenderDoc.h"

namespace
{
    constexpr const uint32_t WIDTH = 1024;
    constexpr const uint32_t HEIGHT = 1024;

    std::filesystem::path GetModulePath()
    {
        WCHAR modulePath[4096];
        DWORD result = GetModuleFileNameW(nullptr, modulePath, ARRAYSIZE(modulePath));
        winrt::check_bool(result != 0 && result != std::size(modulePath));
        return std::filesystem::path{modulePath}.parent_path();
    }

    winrt::com_ptr<ID3D11Device> CreateD3DDevice()
    {
        winrt::com_ptr<ID3D11Device> d3dDevice{};
        uint32_t flags = D3D11_CREATE_DEVICE_SINGLETHREADED | D3D11_CREATE_DEVICE_BGRA_SUPPORT;
        winrt::check_hresult(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags, nullptr, 0, D3D11_SDK_VERSION, d3dDevice.put(), nullptr, nullptr));
        return d3dDevice;
    }

    winrt::com_ptr<ID3D11Texture2D> CreateD3DRenderTargetTexture(ID3D11Device* d3dDevice)
    {
        D3D11_TEXTURE2D_DESC desc{};
        desc.Width = WIDTH;
        desc.Height = HEIGHT;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc = {4, 0};
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;

        winrt::com_ptr<ID3D11Texture2D> texture;
        winrt::check_hresult(d3dDevice->CreateTexture2D(&desc, nullptr, texture.put()));
        return texture;
    }

    Babylon::Graphics::Device CreateGraphicsDevice(ID3D11Device* d3dDevice)
    {
        Babylon::Graphics::Configuration config{};
        config.Device = d3dDevice;
        config.Width = WIDTH;
        config.Height = HEIGHT;
        return Babylon::Graphics::Device(config);
    }

    void CatchAndLogError(Napi::Promise jsPromise)
    {
        auto jsOnRejected = Napi::Function::New(jsPromise.Env(), [](const Napi::CallbackInfo& info) {
            auto console = info.Env().Global().Get("console").As<Napi::Object>();
            console.Get("error").As<Napi::Function>().Call(console, {info[0]});
            std::exit(1);
        });

        jsPromise.Get("catch").As<Napi::Function>().Call(jsPromise, {jsOnRejected});
    }
}

int main()
{
    // Initialize RenderDoc.
    RenderDoc::Init();

    // Create a DirectX device.
    auto d3dDevice = CreateD3DDevice();

    // Get the immediate context for DirectXTK when saving the texture.
    winrt::com_ptr<ID3D11DeviceContext> d3dDeviceContext;
    d3dDevice->GetImmediateContext(d3dDeviceContext.put());

    // Create the Babylon Native graphics device and update.
    auto device = CreateGraphicsDevice(d3dDevice.get());
    auto deviceUpdate = device.GetUpdate("update");

    // Start rendering a frame to unblock the JavaScript from queuing graphics
    // commands.
    device.StartRenderingCurrentFrame();
    deviceUpdate.Start();

    // Create a Babylon Native application runtime which hosts a JavaScript
    // engine on a new thread.
    Babylon::AppRuntime runtime{};
    runtime.Dispatch([&device](Napi::Env env) {
        // Add the Babylon Native graphics device to the JavaScript environment.
        device.AddToJavaScript(env);

        // Initialize the console polyfill.
        Babylon::Polyfills::Console::Initialize(env, [](const char* message, auto) {
            std::cout << message << std::endl;
        });

        // Initialize the window, XMLHttpRequest, and NativeEngine polyfills.
        Babylon::Polyfills::Window::Initialize(env);
        Babylon::Polyfills::XMLHttpRequest::Initialize(env);
        Babylon::Plugins::NativeEngine::Initialize(env);
    });

    // Load the scripts for Babylon.js core and loaders plus this app's index.js.
    Babylon::ScriptLoader loader{runtime};
    loader.LoadScript("app:///Scripts/babylon.max.js");
    loader.LoadScript("app:///Scripts/babylonjs.loaders.js");
    loader.LoadScript("app:///Scripts/index.js");

    // Create a render target texture for the output.
    winrt::com_ptr<ID3D11Texture2D> outputTexture = CreateD3DRenderTargetTexture(d3dDevice.get());

    std::promise<void> addToContext{};
    std::promise<void> startup{};

    // Create an external texture for the render target texture and pass it to
    // the `startup` JavaScript function.
    loader.Dispatch([externalTexture = Babylon::Plugins::ExternalTexture{outputTexture.get()}, &addToContext, &startup](Napi::Env env) {
        auto jsPromise = externalTexture.AddToContextAsync(env);
        addToContext.set_value();

        auto jsOnFulfilled = Napi::Function::New(env, [&startup](const Napi::CallbackInfo& info) {
            auto nativeTexture = info[0];
            info.Env().Global().Get("startup").As<Napi::Function>().Call(
                {
                    nativeTexture,
                    Napi::Value::From(info.Env(), WIDTH),
                    Napi::Value::From(info.Env(), HEIGHT),
                });
            startup.set_value();
        });

        jsPromise = jsPromise.Get("then").As<Napi::Function>().Call(jsPromise, {jsOnFulfilled}).As<Napi::Promise>();

        CatchAndLogError(jsPromise);
    });

    // Wait for `AddToContextAsync` to be called.
    addToContext.get_future().wait();

    // Render a frame so that `AddToContextAsync` will complete.
    deviceUpdate.Finish();
    device.FinishRenderingCurrentFrame();

    // Wait for `startup` to finish.
    startup.get_future().wait();

    struct Asset
    {
        const char* Name;
        const char* Url;
    };

    std::array<Asset, 3> assets = {
        Asset{"BoomBox", "https://raw.githubusercontent.com/KhronosGroup/glTF-Sample-Models/master/2.0/BoomBox/glTF/BoomBox.gltf"},
        Asset{"GlamVelvetSofa", "https://raw.githubusercontent.com/KhronosGroup/glTF-Sample-Models/master/2.0/GlamVelvetSofa/glTF/GlamVelvetSofa.gltf"},
        Asset{"MaterialsVariantsShoe", "https://raw.githubusercontent.com/KhronosGroup/glTF-Sample-Models/master/2.0/MaterialsVariantsShoe/glTF/MaterialsVariantsShoe.gltf"},
    };

    for (const auto& asset : assets)
    {
        // Tell RenderDoc to start capturing.
        RenderDoc::StartFrameCapture(d3dDevice.get());

        // Start rendering a frame to unblock the JavaScript again.
        device.StartRenderingCurrentFrame();
        deviceUpdate.Start();

        std::promise<void> loadAndRenderAsset{};

        // Call `loadAndRenderAssetAsync` with the asset URL.
        loader.Dispatch([&loadAndRenderAsset, &asset](Napi::Env env) {
            std::cout << "Loading " << asset.Name << std::endl;

            auto jsPromise = env.Global().Get("loadAndRenderAssetAsync").As<Napi::Function>().Call({Napi::String::From(env, asset.Url)}).As<Napi::Promise>();

            auto jsOnFulfilled = Napi::Function::New(env, [&loadAndRenderAsset](const Napi::CallbackInfo&) {
                loadAndRenderAsset.set_value();
            });

            jsPromise = jsPromise.Get("then").As<Napi::Function>().Call(jsPromise, {jsOnFulfilled}).As<Napi::Promise>();

            CatchAndLogError(jsPromise);
        });

        // Wait for the function to complete.
        loadAndRenderAsset.get_future().wait();

        // Finish rendering the frame.
        deviceUpdate.Finish();
        device.FinishRenderingCurrentFrame();

        // Tell RenderDoc to stop capturing.
        RenderDoc::StopFrameCapture(d3dDevice.get());

        // Save the texture into an PNG next to the executable.
        auto filePath = GetModulePath() / asset.Name;
        filePath.concat(".png");
        std::cout << "Writing " << filePath.string() << std::endl;

        // See https://github.com/Microsoft/DirectXTK/wiki/ScreenGrab#srgb-vs-linear-color-space
        winrt::check_hresult(DirectX::SaveWICTextureToFile(d3dDeviceContext.get(), outputTexture.get(), GUID_ContainerFormatPng, filePath.c_str(), nullptr, nullptr, true));
    }

    return 0;
}
