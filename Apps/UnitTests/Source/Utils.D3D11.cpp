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

Babylon::Graphics::DeviceT CreateGraphicsDeviceForTest()
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

void DestroyGraphicsDeviceForTest(Babylon::Graphics::DeviceT device)
{
    if (device != nullptr)
    {
        device->Release();
    }
}
