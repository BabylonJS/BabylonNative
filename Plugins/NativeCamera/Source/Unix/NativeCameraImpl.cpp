#include "NativeCameraImpl.h"
#include <napi/napi.h>

namespace Babylon::Plugins
{
    Camera::Impl::Impl(Napi::Env /*env*/, bool /*overrideCameraTexture*/)
    {
    }

    Camera::Impl::~Impl()
    {
    }

    void Camera::Impl::Open(uint32_t /*width*/, uint32_t /*height*/, bool /*frontCamera*/)
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
 