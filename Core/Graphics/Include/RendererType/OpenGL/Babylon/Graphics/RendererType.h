#pragma once

#include <Babylon/Graphics/GL/Texture.h>

namespace Babylon::Graphics
{
    using DeviceT = void*;
    using TextureFormatT = unsigned int;
    using TextureT = GL::Texture*;

    struct PlatformInfo
    {
        DeviceT Device;
    };
}
