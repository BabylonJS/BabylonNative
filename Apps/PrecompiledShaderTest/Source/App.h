#pragma once

#include <Babylon/Graphics/Device.h>
#include <Babylon/Plugins/ExternalTexture.h>

#include <cstdint>
#include <filesystem>
#include <functional>
#include <vector>

constexpr uint32_t WIDTH = 1024;
constexpr uint32_t HEIGHT = 1024;

struct ImageData
{
    std::vector<uint8_t> Pixels; // RGBA8, row-major, top-to-bottom
    uint32_t Width{};
    uint32_t Height{};
};

int RunApp(
    const std::filesystem::path& executablePath,
    Babylon::Graphics::Configuration config,
    Babylon::Plugins::ExternalTexture externalTexture,
    std::function<void(const std::filesystem::path&)> saveTexture,
    std::function<ImageData(const std::filesystem::path&)> loadImage);
