#include <gtest/gtest.h>
#include "Utils.h"

Babylon::Graphics::TextureT CreateTestTexture(Babylon::Graphics::DeviceT device, uint32_t width, uint32_t height, uint32_t arraySize)
{
    throw std::runtime_error{"not implemented"};
}

void DestroyTestTexture(Babylon::Graphics::TextureT texture)
{
    throw std::runtime_error{"not implemented"};
}

Babylon::Graphics::TextureT CreateTestTextureArrayWithData(Babylon::Graphics::DeviceT device, uint32_t width, uint32_t height, const Color* sliceColors, uint32_t sliceCount)
{
    throw std::runtime_error{"not implemented"};
}

Babylon::Graphics::TextureT CreateRenderTargetTexture(Babylon::Graphics::DeviceT device, uint32_t width, uint32_t height)
{
    throw std::runtime_error{"not implemented"};
}

std::vector<uint8_t> ReadBackRenderTarget(Babylon::Graphics::DeviceT device, Babylon::Graphics::TextureT texture, uint32_t width, uint32_t height)
{
    throw std::runtime_error{"not implemented"};
}

void DestroyRenderTargetTexture(Babylon::Graphics::TextureT texture)
{
    throw std::runtime_error{"not implemented"};
}
