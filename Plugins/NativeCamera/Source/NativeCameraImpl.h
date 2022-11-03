#pragma once

#include <bgfx/bgfx.h>
#include <napi/napi.h>
#include <arcana/threading/task.h>
#include <NativeCamera.h>
#include "CameraCapability.h"

namespace Babylon::Plugins
{
    class CameraTrack final
    {
    public:
        ~CameraTrack();

        // Move semantics
        CameraTrack(CameraTrack&&) noexcept;
        CameraTrack& operator=(CameraTrack&&) noexcept;
        
        int32_t Width() const;
        int32_t Height() const;
        
    private:
        struct Impl;
        CameraTrack(std::unique_ptr<Impl> impl);
        std::unique_ptr<Impl> m_impl;
        
        friend class Camera::Impl;
    };

    class CameraDevice final
    {
    public:
        ~CameraDevice();
        
        // Move semantics
        CameraDevice(CameraDevice&&) noexcept;
        CameraDevice& operator=(CameraDevice&&) noexcept;
                
        const std::vector<CameraTrack>& SupportedResolutions() const;
        const std::vector<std::unique_ptr<CameraCapability>>& Capabilities() const;

    private:
        struct Impl;
        CameraDevice(std::unique_ptr<Impl> impl);
        std::unique_ptr<Impl> m_impl;
        
        friend class Camera::Impl;
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
        arcana::task<CameraDimensions, std::exception_ptr> Open(CameraDevice cameraDevice, const CameraTrack* track);
        const CameraDevice* GetOpenedCamera();
        std::vector<CameraDevice> GetCameraDevices();

        void SetTextureOverride(void* texturePtr);
        void UpdateCameraTexture(bgfx::TextureHandle textureHandle);
        void Close();

    private:
        std::unique_ptr<ImplData> m_implData;
    };
}
