#include <gtest/gtest.h>
#include "Helpers.h"

#include <winrt/base.h>

#include <cstring>

namespace Helpers
{
    Babylon::Graphics::TextureT CreateTexture(Babylon::Graphics::DeviceT device, uint32_t width, uint32_t height, uint32_t arraySize, bool renderTarget, uint32_t samples)
    {
        D3D11_TEXTURE2D_DESC desc{};
        desc.Width = width;
        desc.Height = height;
        desc.MipLevels = 1;
        desc.ArraySize = arraySize;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = samples;
        desc.SampleDesc.Quality = 0;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | (renderTarget ? D3D11_BIND_RENDER_TARGET : 0);
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;

        ID3D11Texture2D* texture = nullptr;
        EXPECT_HRESULT_SUCCEEDED(device->CreateTexture2D(&desc, nullptr, &texture));

        return texture;
    }

    void DestroyTexture(Babylon::Graphics::TextureT texture)
    {
        texture->Release();
    }

    Babylon::Graphics::TextureT CreateTextureArrayWithData(Babylon::Graphics::DeviceT device, uint32_t width, uint32_t height, const Color* sliceColors, uint32_t sliceCount)
    {
        const uint32_t rowPitch = width * 4;
        const uint32_t sliceSize = rowPitch * height;

        std::vector<uint8_t> pixels(sliceSize * sliceCount);
        for (uint32_t s = 0; s < sliceCount; ++s)
        {
            for (uint32_t i = 0; i < width * height; ++i)
            {
                uint8_t* p = pixels.data() + s * sliceSize + i * 4;
                p[0] = sliceColors[s].R;
                p[1] = sliceColors[s].G;
                p[2] = sliceColors[s].B;
                p[3] = sliceColors[s].A;
            }
        }

        std::vector<D3D11_SUBRESOURCE_DATA> initData(sliceCount);
        for (uint32_t s = 0; s < sliceCount; ++s)
        {
            initData[s].pSysMem = pixels.data() + s * sliceSize;
            initData[s].SysMemPitch = rowPitch;
            initData[s].SysMemSlicePitch = 0;
        }

        D3D11_TEXTURE2D_DESC desc{};
        desc.Width = width;
        desc.Height = height;
        desc.MipLevels = 1;
        desc.ArraySize = sliceCount;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

        ID3D11Texture2D* texture = nullptr;
        EXPECT_HRESULT_SUCCEEDED(device->CreateTexture2D(&desc, initData.data(), &texture));

        return texture;
    }

    std::vector<uint8_t> ReadPixels(const Babylon::Graphics::PlatformInfo& platformInfo, Babylon::Graphics::TextureT texture, uint32_t width, uint32_t height)
    {
        winrt::com_ptr<ID3D11DeviceContext> context;
        platformInfo.Device->GetImmediateContext(context.put());

        auto* srcTexture = static_cast<ID3D11Texture2D*>(texture);

        D3D11_TEXTURE2D_DESC srcDesc{};
        srcTexture->GetDesc(&srcDesc);

        // If the source is MSAA, resolve into a non-MSAA intermediate before copying to staging.
        // ResolveSubresource performs the standard box-filter resolve from N samples down to 1.
        winrt::com_ptr<ID3D11Texture2D> resolveTarget;
        if (srcDesc.SampleDesc.Count > 1)
        {
            D3D11_TEXTURE2D_DESC resolveDesc = srcDesc;
            resolveDesc.SampleDesc.Count = 1;
            resolveDesc.SampleDesc.Quality = 0;
            resolveDesc.Usage = D3D11_USAGE_DEFAULT;
            resolveDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            resolveDesc.CPUAccessFlags = 0;
            resolveDesc.MiscFlags = 0;

            EXPECT_HRESULT_SUCCEEDED(platformInfo.Device->CreateTexture2D(&resolveDesc, nullptr, resolveTarget.put()));
            context->ResolveSubresource(resolveTarget.get(), 0, srcTexture, 0, srcDesc.Format);
        }

        D3D11_TEXTURE2D_DESC stagingDesc{};
        stagingDesc.Width = width;
        stagingDesc.Height = height;
        stagingDesc.MipLevels = 1;
        stagingDesc.ArraySize = 1;
        stagingDesc.Format = srcDesc.Format;
        stagingDesc.SampleDesc.Count = 1;
        stagingDesc.SampleDesc.Quality = 0;
        stagingDesc.Usage = D3D11_USAGE_STAGING;
        stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

        winrt::com_ptr<ID3D11Texture2D> staging;
        EXPECT_HRESULT_SUCCEEDED(platformInfo.Device->CreateTexture2D(&stagingDesc, nullptr, staging.put()));

        context->CopyResource(staging.get(), resolveTarget ? static_cast<ID3D11Resource*>(resolveTarget.get()) : srcTexture);

        D3D11_MAPPED_SUBRESOURCE mapped{};
        EXPECT_HRESULT_SUCCEEDED(context->Map(staging.get(), 0, D3D11_MAP_READ, 0, &mapped));

        const uint32_t rowPitch = width * 4;
        std::vector<uint8_t> pixels(rowPitch * height);
        const auto* src = static_cast<const uint8_t*>(mapped.pData);
        for (uint32_t row = 0; row < height; ++row)
        {
            memcpy(pixels.data() + row * rowPitch, src + row * mapped.RowPitch, rowPitch);
        }

        context->Unmap(staging.get(), 0);

        return pixels;
    }

    Babylon::Graphics::DeviceT CreateDevice()
    {
        ID3D11Device* device = nullptr;
        EXPECT_HRESULT_SUCCEEDED(D3D11CreateDevice(
            nullptr,
            D3D_DRIVER_TYPE_WARP,
            nullptr,
            0,
            nullptr,
            0,
            D3D11_SDK_VERSION,
            &device,
            nullptr,
            nullptr));
        return device;
    }

    void DestroyDevice(Babylon::Graphics::DeviceT device)
    {
        if (device != nullptr)
        {
            device->Release();
        }
    }
}
