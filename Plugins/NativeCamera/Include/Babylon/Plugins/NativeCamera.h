#pragma once

#include <napi/env.h>

namespace Babylon::Plugins
{
    struct ConstrainULong final {
        std::optional<unsigned long> min{};
        std::optional<unsigned long> max{};
        std::optional<unsigned long> exact{};
        std::optional<unsigned long> ideal{};
    };

    struct ConstrainString final {
        std::optional<std::string> exact{};
        std::optional<std::string> ideal{};
    };

    // This is a native representation of the MediaTrackConstraints Web API: https://developer.mozilla.org/en-US/docs/Web/API/MediaTrackConstraints
    // Only the properties currently supported need to be listed.
    struct CameraConstraints final {
        std::optional<ConstrainULong> width{};
        std::optional<ConstrainULong> height{};
        std::optional<ConstrainString> facingMode{};
    };

    class Camera final
    {
    public:
        class Impl;

        Camera(const Camera& other) = default;
        Camera& operator=(const Camera& other) = default;

        Camera(Camera&&) noexcept = default;
        Camera& operator=(Camera&&) noexcept = default;

        // Initialization with overrideCameraTexture set to true means the caller
        // is expected to override the camera source texture with a native texture.
        // In that case, the HW camera will not be open.
        static Camera Initialize(Napi::Env env);

        static Napi::Value GetUserMedia(const Napi::CallbackInfo& info);

    private:
        Camera();
    };
}
