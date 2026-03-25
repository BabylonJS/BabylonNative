#include <gtest/gtest.h>
#include "Utils.h"

Babylon::Graphics::TextureT CreateTestTexture(Babylon::Graphics::DeviceT device, uint32_t width, uint32_t height, uint32_t arraySize)
{
    D3D12_RESOURCE_DESC desc{};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    desc.Alignment = 0;
    desc.Width = width;
    desc.Height = height;
    desc.DepthOrArraySize = arraySize;
    desc.MipLevels = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    desc.Flags = D3D12_RESOURCE_FLAG_NONE;

    D3D12_HEAP_PROPERTIES heapProps{};
    heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
    heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    heapProps.CreationNodeMask = 1;
    heapProps.VisibleNodeMask = 1;

    ID3D12Resource* texture = nullptr;
    EXPECT_HRESULT_SUCCEEDED(device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        IID_PPV_ARGS(&texture)));

    return texture;
}

void DestroyTestTexture(Babylon::Graphics::TextureT texture)
{
    texture->Release();
}

Babylon::Graphics::TextureT CreateTestTextureArrayWithData(Babylon::Graphics::DeviceT device, uint32_t width, uint32_t height, const Color* sliceColors, uint32_t sliceCount)
{
    throw std::runtime_error{"not implemented for D3D12"};
}

Babylon::Graphics::TextureT CreateRenderTargetTexture(Babylon::Graphics::DeviceT device, uint32_t width, uint32_t height)
{
    throw std::runtime_error{"not implemented for D3D12"};
}

std::vector<uint8_t> ReadBackRenderTarget(Babylon::Graphics::DeviceT device, Babylon::Graphics::TextureT texture, uint32_t width, uint32_t height)
{
    throw std::runtime_error{"not implemented for D3D12"};
}

void DestroyRenderTargetTexture(Babylon::Graphics::TextureT texture)
{
    texture->Release();
}
