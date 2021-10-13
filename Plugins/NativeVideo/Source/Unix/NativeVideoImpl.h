#pragma once

#include <bgfx/bgfx.h>
#include <napi/napi.h>
#include <NativeCamera.h>

namespace Babylon::Plugins
{
    class Camera::Impl final : public std::enable_shared_from_this<Camera::Impl>
    {
    public:
        Impl(Napi::Env env, bool overrideCameraTexture);
        ~Impl();
        void Open(const std::string& source);
        void UpdateTexture(bgfx::TextureHandle textureHandle);
        void AddFrame(uint8_t* data, size_t dataLength);
        void Stop();
        void Close();
    };
}