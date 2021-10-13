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
        void Open(const std::string& contentURL, size_t width, size_t height);
        void Open(const std::string& contentURL);
        void UpdateTexture(bgfx::TextureHandle textureHandle);
        void AddFrame(uint8_t* data, size_t dataLength);
        void Close();

    private:
        struct WriterProcessor;
        struct ReaderProcessor;
        
        std::unique_ptr<WriterProcessor> m_writer;
        std::unique_ptr<ReaderProcessor> m_reader;
    };
}
