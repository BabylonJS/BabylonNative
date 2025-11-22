#pragma once

#include <Babylon/Graphics/Device.h>

Babylon::Graphics::TextureT CreateTestTexture(Babylon::Graphics::DeviceT device, uint32_t width, uint32_t height);
void DestroyTestTexture(Babylon::Graphics::TextureT texture);
