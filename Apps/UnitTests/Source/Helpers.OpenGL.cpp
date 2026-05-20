#include <gtest/gtest.h>
#include "Helpers.h"

namespace Helpers
{
    Babylon::Graphics::TextureT CreateTexture(Babylon::Graphics::DeviceT, uint32_t, uint32_t, uint32_t, bool, uint32_t)
    {
        throw std::runtime_error{"not implemented"};
    }

    void DestroyTexture(Babylon::Graphics::TextureT)
    {
        throw std::runtime_error{"not implemented"};
    }

    std::vector<uint8_t> ReadPixels(const Babylon::Graphics::PlatformInfo&, Babylon::Graphics::TextureT, uint32_t, uint32_t)
    {
        throw std::runtime_error{"not implemented"};
    }
}
