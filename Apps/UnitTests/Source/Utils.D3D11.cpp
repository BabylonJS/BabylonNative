#include <gtest/gtest.h>
#include "Utils.h"

Babylon::Graphics::TextureT CreateTestTexture(Babylon::Graphics::DeviceT device, uint32_t width, uint32_t height, uint32_t arraySize)
{
    D3D11_TEXTURE2D_DESC desc{};
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = arraySize;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    ID3D11Texture2D* texture = nullptr;
    EXPECT_HRESULT_SUCCEEDED(device->CreateTexture2D(&desc, nullptr, &texture));

    return texture;
}

void DestroyTestTexture(Babylon::Graphics::TextureT texture)
{
    texture->Release();
}

Babylon::Graphics::TextureT CreateTestTextureArrayWithData(Babylon::Graphics::DeviceT device, uint32_t width, uint32_t height, const Color* sliceColors, uint32_t sliceCount)
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

Babylon::Graphics::TextureT CreateRenderTargetTexture(Babylon::Graphics::DeviceT device, uint32_t width, uint32_t height)
{
    D3D11_TEXTURE2D_DESC desc{};
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

    ID3D11Texture2D* texture = nullptr;
    EXPECT_HRESULT_SUCCEEDED(device->CreateTexture2D(&desc, nullptr, &texture));

    return texture;
}

std::vector<uint8_t> ReadBackRenderTarget(Babylon::Graphics::DeviceT device, Babylon::Graphics::TextureT texture, uint32_t width, uint32_t height)
{
    ID3D11DeviceContext* context = nullptr;
    device->GetImmediateContext(&context);

    D3D11_TEXTURE2D_DESC stagingDesc{};
    stagingDesc.Width = width;
    stagingDesc.Height = height;
    stagingDesc.MipLevels = 1;
    stagingDesc.ArraySize = 1;
    stagingDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    stagingDesc.SampleDesc.Count = 1;
    stagingDesc.Usage = D3D11_USAGE_STAGING;
    stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

    ID3D11Texture2D* staging = nullptr;
    EXPECT_HRESULT_SUCCEEDED(device->CreateTexture2D(&stagingDesc, nullptr, &staging));

    context->CopyResource(staging, static_cast<ID3D11Texture2D*>(texture));

    D3D11_MAPPED_SUBRESOURCE mapped{};
    EXPECT_HRESULT_SUCCEEDED(context->Map(staging, 0, D3D11_MAP_READ, 0, &mapped));

    const uint32_t rowPitch = width * 4;
    std::vector<uint8_t> pixels(rowPitch * height);
    const auto* src = static_cast<const uint8_t*>(mapped.pData);
    for (uint32_t row = 0; row < height; ++row)
    {
        memcpy(pixels.data() + row * rowPitch, src + row * mapped.RowPitch, rowPitch);
    }

    context->Unmap(staging, 0);
    staging->Release();
    context->Release();

    return pixels;
}

void DestroyRenderTargetTexture(Babylon::Graphics::TextureT texture)
{
    texture->Release();
}
