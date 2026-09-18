#include <gtest/gtest.h>

#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Graphics/DeviceContext.h>
#include <Babylon/Graphics/FrameBuffer.h>
#include <Babylon/Graphics/Texture.h>

#include "Helpers.h"

#include <winrt/base.h>

#include <array>
#include <cstring>
#include <future>
#include <memory>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

extern Babylon::Graphics::Configuration g_deviceConfig;

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

    struct DepthTexture
    {
        winrt::com_ptr<ID3D11Texture2D> Texture;
        winrt::com_ptr<ID3D11DepthStencilView> View;
    };

    RenderTargetTexture CreateTestRenderTargetTexture(
        ID3D11Device* device,
        uint32_t width,
        uint32_t height,
        uint32_t samples = 1,
        DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM)
    {
        D3D11_TEXTURE2D_DESC desc{};
        desc.Width = width;
        desc.Height = height;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = format;
        desc.SampleDesc.Count = samples;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_RENDER_TARGET;

        winrt::com_ptr<ID3D11Texture2D> texture;
        EXPECT_HRESULT_SUCCEEDED(device->CreateTexture2D(&desc, nullptr, texture.put()));

        D3D11_RENDER_TARGET_VIEW_DESC rtvDesc{};
        rtvDesc.Format = desc.Format;
        rtvDesc.ViewDimension = samples > 1 ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;

        winrt::com_ptr<ID3D11RenderTargetView> view;
        EXPECT_HRESULT_SUCCEEDED(device->CreateRenderTargetView(texture.get(), &rtvDesc, view.put()));

        return {texture, view};
    }

    RenderTargetTexture CreateArrayMipRenderTarget(ID3D11Device* device)
    {
        D3D11_TEXTURE2D_DESC desc{};
        desc.Width = 64;
        desc.Height = 32;
        desc.MipLevels = 3;
        desc.ArraySize = 3;
        desc.Format = DXGI_FORMAT_R8G8B8A8_TYPELESS;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_RENDER_TARGET;

        winrt::com_ptr<ID3D11Texture2D> texture;
        EXPECT_HRESULT_SUCCEEDED(device->CreateTexture2D(&desc, nullptr, texture.put()));

        D3D11_RENDER_TARGET_VIEW_DESC viewDesc{};
        viewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        viewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
        viewDesc.Texture2DArray.MipSlice = 2;
        viewDesc.Texture2DArray.FirstArraySlice = 1;
        viewDesc.Texture2DArray.ArraySize = 1;

        winrt::com_ptr<ID3D11RenderTargetView> view;
        EXPECT_HRESULT_SUCCEEDED(device->CreateRenderTargetView(texture.get(), &viewDesc, view.put()));
        return {texture, view};
    }

    DepthTexture CreateArrayMipDepthTexture(ID3D11Device* device)
    {
        D3D11_TEXTURE2D_DESC desc{};
        desc.Width = 64;
        desc.Height = 32;
        desc.MipLevels = 3;
        desc.ArraySize = 3;
        desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

        winrt::com_ptr<ID3D11Texture2D> texture;
        EXPECT_HRESULT_SUCCEEDED(device->CreateTexture2D(&desc, nullptr, texture.put()));

        D3D11_DEPTH_STENCIL_VIEW_DESC viewDesc{};
        viewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        viewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
        viewDesc.Flags = D3D11_DSV_READ_ONLY_DEPTH | D3D11_DSV_READ_ONLY_STENCIL;
        viewDesc.Texture2DArray.MipSlice = 2;
        viewDesc.Texture2DArray.FirstArraySlice = 1;
        viewDesc.Texture2DArray.ArraySize = 1;

        winrt::com_ptr<ID3D11DepthStencilView> view;
        EXPECT_HRESULT_SUCCEEDED(device->CreateDepthStencilView(texture.get(), &viewDesc, view.put()));
        return {texture, view};
    }

    DepthTexture CreateWindowDepthTexture(ID3D11Device* device, const D3D11_DEPTH_STENCIL_VIEW_DESC& viewDesc, uint32_t arraySize = 1)
    {
        D3D11_TEXTURE2D_DESC desc{};
        desc.Width = 32;
        desc.Height = 24;
        desc.MipLevels = 2;
        desc.ArraySize = arraySize;
        desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

        winrt::com_ptr<ID3D11Texture2D> texture;
        EXPECT_HRESULT_SUCCEEDED(device->CreateTexture2D(&desc, nullptr, texture.put()));
        winrt::com_ptr<ID3D11DepthStencilView> view;
        EXPECT_HRESULT_SUCCEEDED(device->CreateDepthStencilView(texture.get(), &viewDesc, view.put()));
        return {texture, view};
    }

    Babylon::Graphics::DeviceContext& GetContext(Babylon::Graphics::Device& device, Babylon::AppRuntime& runtime)
    {
        std::promise<Babylon::Graphics::DeviceContext*> result;
        auto future = result.get_future();
        runtime.Dispatch([&](Napi::Env env) {
            device.AddToJavaScript(env);
            result.set_value(&Babylon::Graphics::DeviceContext::GetFromJavaScript(env));
        });
        return *future.get();
    }

    std::vector<uint8_t> ClearAndCapture(
        Babylon::Graphics::Device& device,
        Babylon::Graphics::DeviceContext& context,
        uint32_t color)
    {
        auto captured = std::make_shared<std::optional<std::vector<uint8_t>>>();
        context.RequestScreenShot([captured](auto pixels) { captured->emplace(std::move(pixels)); });
        for (size_t frame = 0; frame < 3 && !captured->has_value(); ++frame)
        {
            device.StartRenderingCurrentFrame();
            Babylon::Graphics::FrameBuffer backBuffer{context, BGFX_INVALID_HANDLE, 0, 0, true, true, true};
            backBuffer.Clear(*context.GetActiveEncoder(), BGFX_CLEAR_COLOR, color, 1.0f, 0);
            device.FinishRenderingCurrentFrame();
        }
        if (!captured->has_value())
        {
            throw std::runtime_error{"External back buffer screenshot did not complete within three frames."};
        }
        return std::move(captured->value());
    }

    void ApplyPendingBackBufferUpdate(Babylon::Graphics::Device& device)
    {
        device.StartRenderingCurrentFrame();
        device.FinishRenderingCurrentFrame();
    }

    void ExpectSolidColor(
        const std::vector<uint8_t>& pixels,
        uint32_t width,
        uint32_t height,
        uint32_t color)
    {
        ASSERT_EQ(pixels.size(), static_cast<size_t>(width) * height * 4);
        const std::array<uint8_t, 4> expected{
            static_cast<uint8_t>(color >> 24),
            static_cast<uint8_t>(color >> 16),
            static_cast<uint8_t>(color >> 8),
            static_cast<uint8_t>(color)};
        for (size_t index = 0; index < pixels.size(); ++index)
        {
            if (pixels[index] != expected[index % expected.size()])
            {
                ADD_FAILURE() << "Unexpected channel at byte " << index << ": "
                              << unsigned(pixels[index]) << ", expected "
                              << unsigned(expected[index % expected.size()]);
                break;
            }
        }
    }

    void ClearArrayMip(
        ID3D11Device* device,
        ID3D11Texture2D* texture,
        uint32_t arraySlice,
        const std::array<float, 4>& color)
    {
        D3D11_RENDER_TARGET_VIEW_DESC viewDesc{};
        viewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        viewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
        viewDesc.Texture2DArray.MipSlice = 2;
        viewDesc.Texture2DArray.FirstArraySlice = arraySlice;
        viewDesc.Texture2DArray.ArraySize = 1;

        winrt::com_ptr<ID3D11RenderTargetView> view;
        ASSERT_HRESULT_SUCCEEDED(device->CreateRenderTargetView(texture, &viewDesc, view.put()));
        winrt::com_ptr<ID3D11DeviceContext> context;
        device->GetImmediateContext(context.put());
        context->ClearRenderTargetView(view.get(), color.data());
    }

    std::vector<uint8_t> ReadArrayMip(ID3D11Device* device, ID3D11Texture2D* texture, uint32_t arraySlice)
    {
        D3D11_TEXTURE2D_DESC stagingDesc{};
        stagingDesc.Width = 16;
        stagingDesc.Height = 8;
        stagingDesc.MipLevels = 1;
        stagingDesc.ArraySize = 1;
        stagingDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        stagingDesc.SampleDesc.Count = 1;
        stagingDesc.Usage = D3D11_USAGE_STAGING;
        stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

        winrt::com_ptr<ID3D11Texture2D> staging;
        EXPECT_HRESULT_SUCCEEDED(device->CreateTexture2D(&stagingDesc, nullptr, staging.put()));
        winrt::com_ptr<ID3D11DeviceContext> context;
        device->GetImmediateContext(context.put());
        context->CopySubresourceRegion(
            staging.get(),
            0,
            0,
            0,
            0,
            texture,
            D3D11CalcSubresource(2, arraySlice, 3),
            nullptr);

        D3D11_MAPPED_SUBRESOURCE mapped{};
        EXPECT_HRESULT_SUCCEEDED(context->Map(staging.get(), 0, D3D11_MAP_READ, 0, &mapped));
        std::vector<uint8_t> pixels(stagingDesc.Width * stagingDesc.Height * 4);
        for (uint32_t row = 0; row < stagingDesc.Height; ++row)
        {
            memcpy(
                pixels.data() + row * stagingDesc.Width * 4,
                static_cast<const uint8_t*>(mapped.pData) + row * mapped.RowPitch,
                stagingDesc.Width * 4);
        }
        context->Unmap(staging.get(), 0);
        return pixels;
    }
}

TEST(Device, ExternalBackBufferCaptureAndUpdate)
{
    winrt::com_ptr<ID3D11Device> d3dDevice = CreateDevice();
    auto first = CreateTestRenderTargetTexture(
        d3dDevice.get(),
        64,
        48,
        1,
        DXGI_FORMAT_B8G8R8A8_UNORM);

    Babylon::Graphics::Configuration config{};
    config.Device = d3dDevice.get();
    config.BackBufferColor = first.View.get();
    config.Width = 64;
    config.Height = 48;

    Babylon::Graphics::Device device{config};
    Babylon::AppRuntime runtime{};
    auto& context = GetContext(device, runtime);

    first = {};
    ExpectSolidColor(ClearAndCapture(device, context, 0x2050a0ff), 64, 48, 0x2050a0ff);

    auto second = CreateTestRenderTargetTexture(d3dDevice.get(), 32, 24);
    device.UpdateBackBuffer(second.View.get());
    device.UpdateSize(32, 24);
    second = {};
    ApplyPendingBackBufferUpdate(device);

    ExpectSolidColor(ClearAndCapture(device, context, 0x804020ff), 32, 24, 0x804020ff);
}

TEST(Device, BackBufferPreservesViewSubresourceFormatAndDepthFlags)
{
    winrt::com_ptr<ID3D11Device> d3dDevice = CreateDevice();
    auto color = CreateArrayMipRenderTarget(d3dDevice.get());
    auto depth = CreateArrayMipDepthTexture(d3dDevice.get());

    ClearArrayMip(d3dDevice.get(), color.Texture.get(), 0, {0.0f, 1.0f, 0.0f, 1.0f});
    ClearArrayMip(d3dDevice.get(), color.Texture.get(), 1, {0.0f, 0.0f, 1.0f, 1.0f});
    ClearArrayMip(d3dDevice.get(), color.Texture.get(), 2, {0.0f, 1.0f, 0.0f, 1.0f});

    Babylon::Graphics::Configuration config{};
    config.Device = d3dDevice.get();
    config.BackBufferColor = color.View.get();
    config.BackBufferDepthStencil = depth.View.get();
    config.Width = 16;
    config.Height = 8;

    Babylon::Graphics::Device device{config};
    Babylon::AppRuntime runtime{};
    auto& context = GetContext(device, runtime);
    ExpectSolidColor(ClearAndCapture(device, context, 0xff0000ff), 16, 8, 0xff0000ff);

    ExpectSolidColor(ReadArrayMip(d3dDevice.get(), color.Texture.get(), 0), 16, 8, 0x00ff00ff);
    ExpectSolidColor(ReadArrayMip(d3dDevice.get(), color.Texture.get(), 1), 16, 8, 0xff0000ff);
    ExpectSolidColor(ReadArrayMip(d3dDevice.get(), color.Texture.get(), 2), 16, 8, 0x00ff00ff);
}

TEST(Device, BackBufferMsaaCaptureUsesActualViewSampleCount)
{
    winrt::com_ptr<ID3D11Device> d3dDevice = CreateDevice();
    uint32_t qualityLevels{};
    ASSERT_HRESULT_SUCCEEDED(
        d3dDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &qualityLevels));
    if (qualityLevels == 0)
    {
        GTEST_SKIP() << "D3D11 WARP does not support 4x MSAA for RGBA8.";
    }

    auto color = CreateTestRenderTargetTexture(d3dDevice.get(), 32, 24, 4);
    Babylon::Graphics::Configuration config{};
    config.Device = d3dDevice.get();
    config.BackBufferColor = color.View.get();
    config.Width = 32;
    config.Height = 24;
    config.BackBufferDepthStencilFormat = Babylon::Graphics::DepthStencilFormat::Depth24Stencil8;

    Babylon::Graphics::Device device{config};
    Babylon::AppRuntime runtime{};
    auto& context = GetContext(device, runtime);
    ExpectSolidColor(ClearAndCapture(device, context, 0x4080c0ff), 32, 24, 0x4080c0ff);
}

TEST(Device, WindowDepthBackBufferRejectsReadOnlyViews)
{
    auto d3dDevice = CreateDevice();
    for (UINT flags : std::array<UINT, 3>{D3D11_DSV_READ_ONLY_DEPTH, D3D11_DSV_READ_ONLY_STENCIL,
             D3D11_DSV_READ_ONLY_DEPTH | D3D11_DSV_READ_ONLY_STENCIL})
    {
        SCOPED_TRACE(flags);
        D3D11_DEPTH_STENCIL_VIEW_DESC viewDesc{};
        viewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        viewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        viewDesc.Flags = flags;
        auto depth = CreateWindowDepthTexture(d3dDevice.get(), viewDesc);

        auto config = g_deviceConfig;
        config.Device = d3dDevice.get();
        config.Width = 32;
        config.Height = 24;
        config.BackBufferColor = nullptr;
        config.BackBufferDepthStencil = depth.View.get();
        Babylon::Graphics::Device device{config};
        EXPECT_THROW(device.EnableRendering(), std::runtime_error);
    }
}

TEST(Device, WindowDepthBackBufferRejectsNonDefaultSubresources)
{
    auto d3dDevice = CreateDevice();
    for (uint32_t arraySize : {0u, 1u, 2u})
    {
        SCOPED_TRACE(arraySize);
        const bool arrayView = arraySize != 0;
        D3D11_DEPTH_STENCIL_VIEW_DESC viewDesc{};
        viewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        viewDesc.ViewDimension = arrayView ? D3D11_DSV_DIMENSION_TEXTURE2DARRAY : D3D11_DSV_DIMENSION_TEXTURE2D;
        if (arrayView)
        {
            viewDesc.Texture2DArray.FirstArraySlice = arraySize - 1;
            viewDesc.Texture2DArray.ArraySize = 1;
        }
        else
        {
            viewDesc.Texture2D.MipSlice = 1;
        }
        auto depth = CreateWindowDepthTexture(d3dDevice.get(), viewDesc, arrayView ? arraySize : 1);

        auto config = g_deviceConfig;
        config.Device = d3dDevice.get();
        config.Width = arrayView ? 32 : 16;
        config.Height = arrayView ? 24 : 12;
        config.BackBufferColor = nullptr;
        config.BackBufferDepthStencil = depth.View.get();
        Babylon::Graphics::Device device{config};
        EXPECT_THROW(device.EnableRendering(), std::runtime_error);
    }
}

TEST(Device, WindowDepthBackBufferAcceptsDefaultViewAfterRejection)
{
    auto d3dDevice = CreateDevice();
    D3D11_DEPTH_STENCIL_VIEW_DESC viewDesc{};
    viewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    viewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    viewDesc.Flags = D3D11_DSV_READ_ONLY_DEPTH;
    auto rejected = CreateWindowDepthTexture(d3dDevice.get(), viewDesc);
    viewDesc.Flags = 0;
    auto depth = CreateWindowDepthTexture(d3dDevice.get(), viewDesc);

    auto config = g_deviceConfig;
    config.Device = d3dDevice.get();
    config.Width = 32;
    config.Height = 24;
    config.BackBufferColor = nullptr;
    config.BackBufferDepthStencil = rejected.View.get();
    Babylon::Graphics::Device device{config};
    EXPECT_THROW(device.EnableRendering(), std::runtime_error);
    device.UpdateBackBuffer(nullptr, depth.View.get());
    ASSERT_NO_THROW(device.EnableRendering());

    Babylon::AppRuntime runtime{};
    auto& context = GetContext(device, runtime);
    ExpectSolidColor(ClearAndCapture(device, context, 0x4080c0ff), 32, 24, 0x4080c0ff);
}
