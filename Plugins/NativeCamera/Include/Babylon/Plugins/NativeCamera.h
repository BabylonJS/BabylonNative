#pragma once

#include <napi/env.h>

namespace Babylon::Plugins
{
    class Camera final
    {
    public:
        class Impl;

        Camera(const Camera& other) = default;
        Camera(Camera&&) = default;
        ~Camera() = default;

        // Initialization with overrideCameraTexture set to true means the caller
        // is expected to override the camera source texture with a native texture.
        // In that case, the HW camera will not be open.
        static Camera Initialize(Napi::Env env, bool overrideCameraTexture = false);

        void SetTextureOverride(void* texturePtr);

    private:
        Camera(std::shared_ptr<Impl> impl);
        std::shared_ptr<Impl> m_impl{};
    };
}
