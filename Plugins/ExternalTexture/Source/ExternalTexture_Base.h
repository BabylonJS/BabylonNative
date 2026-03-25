#pragma once

#include <Babylon/DebugTrace.h>
#include <Babylon/Graphics/RendererType.h>
#include <Babylon/Graphics/Texture.h>
#include <bgfx/bgfx.h>
#include <set>
#include <cassert>
#include <cmath>

namespace Babylon::Plugins
{
    class ExternalTexture::ImplBase
    {
    public:
        uint16_t Width() const { return m_info.Width; }
        uint16_t Height() const { return m_info.Height; }
        bgfx::TextureFormat::Enum Format() const { return m_info.Format; }
        bool HasMips() const { return m_info.MipLevels != 1; }
        uint16_t NumLayers() const { return m_info.NumLayers; }
        uint64_t Flags() const { return m_info.Flags; }

        void AddTexture(Graphics::Texture* texture)
        {
            if (!m_textures.insert(texture).second)
            {
                assert(!"Failed to insert texture");
            }
        }

        void RemoveTexture(Graphics::Texture* texture)
        {
            auto it = m_textures.find(texture);
            if (it != m_textures.end())
            {
                m_textures.erase(it);
            }
        }

        std::mutex& Mutex() const { return m_mutex; }

    protected:
        static bool IsFullMipChain(uint16_t mipLevel, uint16_t width, uint16_t height)
        {
            return mipLevel == static_cast<uint16_t>(std::floor(std::log2(std::max(static_cast<float>(width), static_cast<float>(height))) + 1));
        }

        static auto RenderTargetSamplesToBgfxMsaaFlag(uint64_t renderTargetSamples)
        {
            switch (renderTargetSamples)
            {
                case 2:
                    return BGFX_TEXTURE_RT_MSAA_X2;
                case 4:
                    return BGFX_TEXTURE_RT_MSAA_X4;
                case 8:
                    return BGFX_TEXTURE_RT_MSAA_X8;
                case 16:
                    return BGFX_TEXTURE_RT_MSAA_X16;
            }

            return BGFX_TEXTURE_NONE;
        }

        void UpdateTextures(Graphics::TextureT ptr)
        {
            for (auto* texture : m_textures)
            {
                bgfx::TextureHandle handle = bgfx::createTexture2D(
                    Width(),
                    Height(),
                    HasMips(),
                    NumLayers(),
                    Format(),
                    Flags(),
                    0, // _mem
                    reinterpret_cast<uintptr_t>(ptr) // _external
                );

                if (!bgfx::isValid(handle))
                {
                    throw std::runtime_error{"Failed to create external texture"};
                }

                texture->Attach(handle, true, m_info.Width, m_info.Height, HasMips(), m_info.NumLayers, m_info.Format, m_info.Flags);
            }
        }

        struct Info
        {
            uint16_t Width{};
            uint16_t Height{};
            uint16_t MipLevels{};
            uint16_t NumLayers{};
            bgfx::TextureFormat::Enum Format{bgfx::TextureFormat::Unknown};
            uint64_t Flags{};
        };

        Info m_info{};

    private:
        mutable std::mutex m_mutex{};
        std::set<Graphics::Texture*> m_textures{};
    };
}
