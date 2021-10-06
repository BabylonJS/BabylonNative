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
        void Open(uint32_t width, uint32_t height, bool frontCamera);
        void UpdateTexture(bgfx::TextureHandle textureHandle);
        void Close();
    };
}