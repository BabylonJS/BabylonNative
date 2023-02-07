#pragma once

#include <bgfx/bgfx.h>
#include <napi/napi.h>
#include <arcana/threading/task.h>
#include "Capability.h"

namespace Babylon::Plugins
{
    // The CameraTrack class is a platform agnostic representation of a specific stream
    // available on the CameraDevice. Typically a CameraDevice provides a different CameraTrack
    // for each different resolution it supports.
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
        explicit CameraTrack(std::unique_ptr<Impl> impl);
        std::unique_ptr<Impl> m_impl;

        friend class CameraDevice;
    };

    // The CameraDevice class is a platform agnostic representation of a camera available on the current
    // device and exposes what capabilities the specific camera supports. It encapsulates the logic to
    // render frames off of the camera stream and onto a texture that can be rendered by Babylon.
    class CameraDevice final : public std::enable_shared_from_this<CameraDevice>
    {
    public:
        struct CameraDimensions {
            uint32_t width{};
            uint32_t height{};
        };

        ~CameraDevice();
        
        // Move semantics
        CameraDevice(CameraDevice&&) noexcept;
        CameraDevice& operator=(CameraDevice&&) noexcept;

        static std::vector<CameraDevice> GetCameraDevices(Napi::Env env);

        arcana::task<CameraDimensions, std::exception_ptr> OpenAsync(const CameraTrack& track);
        void Close();
        CameraDimensions UpdateCameraTexture(bgfx::TextureHandle textureHandle);

        const std::vector<CameraTrack>& SupportedResolutions() const;
        const std::vector<std::unique_ptr<Capability>>& Capabilities() const;

    private:
        struct Impl;
        explicit CameraDevice(std::unique_ptr<Impl> impl);
        std::unique_ptr<Impl> m_impl;
    };
}
