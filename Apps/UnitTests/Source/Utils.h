#pragma once

#include <Babylon/Graphics/Device.h>
#include <Babylon/AppRuntime.h>

#include <cstdint>
#include <vector>

struct Color
{
    uint8_t R, G, B, A;
};

Babylon::Graphics::TextureT CreateTestTexture(Babylon::Graphics::DeviceT device, uint32_t width, uint32_t height, uint32_t arraySize = 1);
void DestroyTestTexture(Babylon::Graphics::TextureT texture);

Babylon::Graphics::TextureT CreateTestTextureArrayWithData(Babylon::Graphics::DeviceT device, uint32_t width, uint32_t height, const Color* sliceColors, uint32_t sliceCount);
Babylon::Graphics::TextureT CreateRenderTargetTexture(Babylon::Graphics::DeviceT device, uint32_t width, uint32_t height);
std::vector<uint8_t> ReadBackRenderTarget(Babylon::Graphics::DeviceT device, Babylon::Graphics::TextureT texture, uint32_t width, uint32_t height);
void DestroyRenderTargetTexture(Babylon::Graphics::TextureT texture);

void DispatchSync(Babylon::AppRuntime& runtime, std::function<void(Napi::Env)> func);