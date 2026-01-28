#include <gtest/gtest.h>

#include <Babylon/Graphics/Device.h>

#include "Utils.h"

#include <winrt/base.h>

namespace
{
    winrt::com_ptr<ID3D11Device> CreateDevice()
    {
        winrt::com_ptr<ID3D11Device> device{};
        EXPECT_HRESULT_SUCCEEDED(D3D11CreateDevice(
            nullptr,
            D3D_DRIVER_TYPE_WARP,
            nullptr,
            0,
            nullptr,
            0,
            D3D11_SDK_VERSION,
            device.put(),
            nullptr,
            nullptr));
        return device;
    }

    struct RenderTargetTexture
    {
        winrt::com_ptr<ID3D11Texture2D> Texture;
        winrt::com_ptr<ID3D11RenderTargetView> View;
    };

    RenderTargetTexture CreateTestRenderTargetTexture(ID3D11Device* device, uint32_t width, uint32_t height)
    {
        D3D11_TEXTURE2D_DESC desc{};
        desc.Width = width;
        desc.Height = height;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_RENDER_TARGET;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;

        winrt::com_ptr<ID3D11Texture2D> texture;
        EXPECT_HRESULT_SUCCEEDED(device->CreateTexture2D(&desc, nullptr, texture.put()));

        D3D11_RENDER_TARGET_VIEW_DESC rtvDesc{};
        rtvDesc.Format = desc.Format;
        rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        rtvDesc.Texture2D.MipSlice = 0;

        winrt::com_ptr<ID3D11RenderTargetView> view;
        EXPECT_HRESULT_SUCCEEDED(device->CreateRenderTargetView(texture.get(), &rtvDesc, view.put()));

        return {texture, view};
    }
}

TEST(Device, BackBuffer)
{
    winrt::com_ptr<ID3D11Device> d3dDevice = CreateDevice();

    constexpr SIZE dimensions[] = {
        {1600, 900},
        {1280, 720},
        {1600, 900},
        {640, 360},
        {640, 360},
        {1280, 720},
    };

    std::vector<RenderTargetTexture> renderTargetTextures;
    for (size_t i = 0; i < std::size(dimensions); ++i)
    {
        renderTargetTextures.push_back(CreateTestRenderTargetTexture(d3dDevice.get(), dimensions[i].cx, dimensions[i].cy));
    }

    Babylon::Graphics::Configuration config{};
    config.Device = d3dDevice.get();
    config.BackBufferColor = renderTargetTextures[0].View.get();
    config.Width = dimensions[0].cx;
    config.Height = dimensions[0].cy;

    Babylon::Graphics::Device device{config};
    Babylon::Graphics::DeviceUpdate update{device.GetUpdate("update")};

    for (size_t i = 1; i < std::size(dimensions); ++i)
    {
        device.StartRenderingCurrentFrame();
        update.Start();

        device.UpdateBackBuffer(renderTargetTextures[i].View.get());
        device.UpdateSize(dimensions[i].cx, dimensions[i].cy);

        update.Finish();
        device.FinishRenderingCurrentFrame();
    }
}
