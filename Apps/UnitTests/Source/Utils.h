#pragma once

#include <Babylon/Graphics/Device.h>
#include <Babylon/Polyfills/Console.h>

Babylon::Graphics::TextureT CreateTestTexture(Babylon::Graphics::DeviceT device, uint32_t width, uint32_t height, uint32_t arraySize = 1);
void DestroyTestTexture(Babylon::Graphics::TextureT texture);

namespace Babylon::Polyfills::Console
{
    const char* EnumToString(Babylon::Polyfills::Console::LogLevel logLevel);
}
