#pragma once

#include <bgfx/bgfx.h>
#include <napi/napi.h>
#include <arcana/threading/task.h>
#include <NativeCamera.h>
#include "CameraCapability.h"

namespace Babylon::Plugins
{
    struct CameraTrack final
    {
        ~CameraTrack();
        
        int32_t width;
        int32_t height;
        
        struct ImplData;
        std::unique_ptr<ImplData> implData;
    };

    struct CameraDevice final
    {
        ~CameraDevice();

        std::vector<std::shared_ptr<CameraTrack>> supportedResolutions;
        std::vector<std::shared_ptr<CameraCapability>> capabilities{};
        
        struct ImplData;
        std::unique_ptr<ImplData> implData;
    };

    class Camera::Impl final : public std::enable_shared_from_this<Camera::Impl>
    {
    public:
        struct CameraDimensions {
            uint32_t width;
            uint32_t height;
        };

        struct ImplData;

        Impl(Napi::Env env, bool overrideCameraTexture);
        ~Impl();
        arcana::task<CameraDimensions, std::exception_ptr> Open(std::shared_ptr<CameraDevice> cameraDevice, std::shared_ptr<CameraTrack> track);

        std::vector<std::shared_ptr<CameraDevice>> GetCameraDevices();

        void SetTextureOverride(void* texturePtr);
        void UpdateCameraTexture(bgfx::TextureHandle textureHandle);
        void Close();

    private:
        std::unique_ptr<ImplData> m_implData;
    };
}
