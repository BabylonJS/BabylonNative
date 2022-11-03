#include "NativeCamera.h"
#include "../NativeCameraImpl.h"
#include <napi/napi.h>

namespace Babylon::Plugins
{
    struct CameraTrack::Impl
    {
        int32_t width;
        int32_t height;
    };

    struct CameraDevice::Impl
    {
        std::vector<CameraTrack> supportedResolutions{};
        std::vector<std::unique_ptr<CameraCapability>> capabilities{};
    };

    struct Camera::Impl::ImplData{};

    Camera::Impl::Impl(Napi::Env /*env*/, bool /*overrideCameraTexture*/)
    {
    }

    Camera::Impl::~Impl()
    {
    }

    std::vector<CameraDevice> Camera::Impl::GetCameraDevices()
    {
        throw std::runtime_error{ "HW Camera not implemented for this platform." };
    }

    arcana::task<Camera::Impl::CameraDimensions, std::exception_ptr> Camera::Impl::Open(
            CameraDevice /*cameraDevice*/,
            const CameraTrack* /*track*/)
    {
        throw std::runtime_error{ "HW Camera not implemented for this platform." };
    }

    const CameraDevice* Camera::Impl::GetOpenedCamera()
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

#include "../NativeCameraImplShared.h"
