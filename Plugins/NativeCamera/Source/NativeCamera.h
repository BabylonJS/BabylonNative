#pragma once
#include <bgfx/bgfx.h>

namespace Babylon::Plugins::Internal
{
    // Move this struct to Graphics
    struct TextureData final
    {
        ~TextureData()
        {
            if (bgfx::isValid(Handle))
            {
                bgfx::destroy(Handle);
            }
        }

        bgfx::TextureHandle Handle{ bgfx::kInvalidHandle };
        uint32_t Width{ 0 };
        uint32_t Height{ 0 };
        uint32_t Flags{ 0 };
        uint8_t AnisotropicLevel{ 0 };
    };

    struct CameraInterface
    {
        virtual ~CameraInterface() = default;
        static CameraInterface* CreateInterface();
        virtual void UpdateCameraTexture(bgfx::TextureHandle textureHandle) = 0;
    };
}
