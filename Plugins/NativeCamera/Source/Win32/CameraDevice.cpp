#include "NativeCamera.h"
#include "../CameraDevice.h"
#include <napi/napi.h>
#include <arcana/threading/affinity.h>

namespace Babylon::Plugins
{
    struct CameraTrack::Impl
    {
        int32_t width{};
        int32_t height{};
    };

    struct CameraDevice::Impl
    {
        arcana::affinity threadAffinity{};
        std::vector<CameraTrack> supportedResolutions{};
        std::vector<std::unique_ptr<Capability>> capabilities{};
    };

    std::vector<CameraDevice> CameraDevice::GetCameraDevices(Napi::Env /*env*/)
    {
        // When implementing this function remove the disabled warning at the top of MediaStream.cpp
        throw std::runtime_error{"HW Camera not implemented for this platform."};
    }

    arcana::task<CameraDevice::CameraDimensions, std::exception_ptr> CameraDevice::OpenAsync(const CameraTrack& /*track*/)
    {
        // When implementing this function remove the disabled warning at the top of MediaStream.cpp
        throw std::runtime_error{"HW Camera not implemented for this platform."};
    }

    CameraDevice::CameraDimensions CameraDevice::UpdateCameraTexture(bgfx::TextureHandle /*textureHandle*/)
    {
        throw std::runtime_error{"HW Camera not implemented for this platform."};
    }

    void CameraDevice::Close()
    {
        throw std::runtime_error{"HW Camera not implemented for this platform."};
    }
}

#include "../CameraDeviceSharedPImpl.h"
