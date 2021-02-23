#include "NativeCamera.h"

namespace Babylon::Plugins::Internal
{
    CameraData* InitializeCameraTexture()
    {
        return nullptr;
    }

    void UpdateCameraTexture(bgfx::TextureHandle /*textureHandle*/, const CameraData* /*cameraData*/)
    {
    }

    void DisposeCameraTexture(const CameraData* /*cameraData*/)
    {
    }
}