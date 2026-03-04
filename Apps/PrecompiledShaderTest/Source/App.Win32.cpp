#include "App.h"

#include <winrt/base.h>

#include <WICTextureLoader.h>
#include <ScreenGrab.h>
#include <wincodec.h>

namespace
{
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
        desc.SampleDesc = {1, 0};
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;

        winrt::com_ptr<ID3D11Texture2D> texture;
        winrt::check_hresult(d3dDevice->CreateTexture2D(&desc, nullptr, texture.put()));
        return texture;
    }

    ImageData LoadPNG(const std::filesystem::path& filePath)
    {
        // Use WIC to decode the PNG directly into RGBA8 pixel data.
        // This avoids a pointless GPU round-trip that DirectXTK's
        // CreateWICTextureFromFile would require.
        winrt::com_ptr<IWICImagingFactory> wicFactory;
        winrt::check_hresult(CoCreateInstance(
            CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(wicFactory.put())));

        winrt::com_ptr<IWICBitmapDecoder> decoder;
        winrt::check_hresult(wicFactory->CreateDecoderFromFilename(
            filePath.c_str(), nullptr, GENERIC_READ,
            WICDecodeMetadataCacheOnLoad, decoder.put()));

        winrt::com_ptr<IWICBitmapFrameDecode> frame;
        winrt::check_hresult(decoder->GetFrame(0, frame.put()));

        UINT width{}, height{};
        winrt::check_hresult(frame->GetSize(&width, &height));

        // Convert to 32bpp RGBA.
        winrt::com_ptr<IWICFormatConverter> converter;
        winrt::check_hresult(wicFactory->CreateFormatConverter(converter.put()));
        winrt::check_hresult(converter->Initialize(
            frame.get(), GUID_WICPixelFormat32bppRGBA,
            WICBitmapDitherTypeNone, nullptr, 0.0,
            WICBitmapPaletteTypeCustom));

        const UINT stride = width * 4;
        std::vector<uint8_t> pixels(static_cast<size_t>(stride) * height);
        winrt::check_hresult(converter->CopyPixels(nullptr, stride,
            static_cast<UINT>(pixels.size()), pixels.data()));

        return {std::move(pixels), width, height};
    }
}

int main()
{
    winrt::init_apartment(winrt::apartment_type::multi_threaded);

    auto modulePath = GetModulePath();

    // Create a DirectX device.
    auto d3dDevice = CreateD3DDevice();

    // Get the immediate context for DirectXTK when saving the texture.
    winrt::com_ptr<ID3D11DeviceContext> d3dDeviceContext;
    d3dDevice->GetImmediateContext(d3dDeviceContext.put());

    // Create a render target texture for the output.
    auto outputTexture = CreateD3DRenderTargetTexture(d3dDevice.get());

    Babylon::Graphics::Configuration config{};
    config.Device = d3dDevice.get();
    config.Width = WIDTH;
    config.Height = HEIGHT;

    return RunApp(
        modulePath,
        config,
        {outputTexture.get()},
        [&](const std::filesystem::path& filePath) {
            winrt::check_hresult(DirectX::SaveWICTextureToFile(
                d3dDeviceContext.get(), outputTexture.get(),
                GUID_ContainerFormatPng, filePath.c_str(), nullptr, nullptr, true));
        },
        LoadPNG);
}
