#pragma once

#include <set>
#include <bgfx/bgfx.h>
#include <napi/napi.h>
#include <arcana/threading/task.h>
#include <gsl/gsl>
#include "Capability.h"

#ifdef _MSC_VER
#define DISABLE_UNREACHABLE_CODE_WARNINGS \
        __pragma(warning(push)) \
        __pragma(warning(disable : 4702))
#define ENABLE_UNREACHABLE_CODE_WARNINGS __pragma(warning(pop))
#else
#define DISABLE_UNREACHABLE_CODE_WARNINGS
#define ENABLE_UNREACHABLE_CODE_WARNINGS
#endif

namespace Babylon::Plugins
{
    enum class RedEyeReduction
    {
        Never,
        Always,
        Controllable,
    };

    enum class FillLightMode
    {
        Off,
        Flash,
        Auto,
    };

    class PhotoCapabilities final
    {
    public:
        Babylon::Plugins::RedEyeReduction RedEyeReduction{};
        std::set<Babylon::Plugins::FillLightMode> FillLightModes{};

        uint32_t MinWidth{};
        uint32_t MaxWidth{};
        uint32_t StepWidth{};

        uint32_t MinHeight{};
        uint32_t MaxHeight{};
        uint32_t StepHeight{};
    };

    class PhotoSettings final
    {
    public:
        bool RedEyeReduction{};
        Babylon::Plugins::FillLightMode FillLightMode{};
        uint32_t Width{};
        uint32_t Height{};
    };

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
        using TakePhotoTask = arcana::task<gsl::span<const uint8_t>, std::exception_ptr>;

        struct CameraDimensions
        {
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
        TakePhotoTask TakePhotoAsync(PhotoSettings photoSettings);

        const std::vector<CameraTrack>& SupportedResolutions() const;
        const std::vector<std::unique_ptr<Capability>>& Capabilities() const;

        // Gets high resolution photo capture capabilities for the currently opened stream/track.
        const Plugins::PhotoCapabilities& PhotoCapabilities() const;
        const Plugins::PhotoSettings& DefaultPhotoSettings() const;

    private:
        struct Impl;
        explicit CameraDevice(std::unique_ptr<Impl> impl);
        std::unique_ptr<Impl> m_impl;
    };
}
