#include "NativeCamera.h"
#include "../NativeCameraImpl.h"
#include <napi/napi.h>

namespace Babylon::Plugins
{
    struct Camera::Impl::ImplData{};

    Camera::Impl::Impl(Napi::Env /*env*/, bool /*overrideCameraTexture*/)
    {
    }

    Camera::Impl::~Impl()
    {
    }

    std::vector<std::shared_ptr<CameraDevice>> Camera::Impl::GetCameraDevices()
    {
        throw std::runtime_error{ "HW Camera not implemented for this platform." };
    }

    arcana::task<Camera::Impl::CameraDimensions, std::exception_ptr> Camera::Impl::Open(
            std::shared_ptr<CameraDevice> /*cameraDevice*/,
            std::shared_ptr<CameraTrack> /*track*/)
    {
        throw std::runtime_error{ "HW Camera not implemented for this platform." };
    }

    void Camera::Impl::SetTextureOverride(void* /*texturePtr*/)
    {
        throw std::runtime_error{ "HW Camera not implemented for this platform." };
    }

    void Camera::Impl::UpdateCameraTexture(bgfx::TextureHandle /*textureHandle*/)
    {
        throw std::runtime_error{ "HW Camera not implemented for this platform." };
    }

    void Camera::Impl::Close()
    {
        throw std::runtime_error{ "HW Camera not implemented for this platform." };
    }
}
