#include <gtest/gtest.h>
#include "Helpers.h"

#include <stdexcept>

namespace Helpers
{
    Babylon::Graphics::TextureT CreateTexture(Babylon::Graphics::DeviceT, uint32_t, uint32_t, uint32_t, bool, uint32_t)
    {
        throw std::runtime_error{"Helpers::CreateTexture is not implemented for Vulkan"};
    }

    void DestroyTexture(Babylon::Graphics::TextureT)
    {
    }

    Babylon::Graphics::TextureT CreateTextureArrayWithData(Babylon::Graphics::DeviceT, uint32_t, uint32_t, const Color*, uint32_t)
    {
        throw std::runtime_error{"Helpers::CreateTextureArrayWithData is not implemented for Vulkan"};
    }

    std::vector<uint8_t> ReadPixels(const Babylon::Graphics::PlatformInfo&, Babylon::Graphics::TextureT, uint32_t, uint32_t)
    {
        throw std::runtime_error{"Helpers::ReadPixels is not implemented for Vulkan"};
    }
}
