#pragma once

#include <napi/env.h>

namespace Babylon::Plugins
{
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
