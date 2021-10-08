#pragma once

#include <bgfx/bgfx.h>
#include <napi/napi.h>
#include <NativeVideo.h>

namespace Babylon::Plugins
{
    class Video::Impl final : public std::enable_shared_from_this<Video::Impl>
    {
    public:
        Impl(Napi::Env env);
        ~Impl();
        void Open(const std::string& source);
        void UpdateTexture(bgfx::TextureHandle textureHandle);
        void Close();
    };
}