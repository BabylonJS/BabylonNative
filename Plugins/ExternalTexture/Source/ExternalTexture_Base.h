#pragma once

#include <Babylon/DebugTrace.h>
#include <Babylon/Graphics/RendererType.h>
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

        void AddHandle(bgfx::TextureHandle handle)
        {
            std::scoped_lock lock{m_mutex};

            if (!m_handles.insert(handle).second)
            {
                assert(!"Failed to insert handle");
            }
        }

        void RemoveHandle(bgfx::TextureHandle handle)
        {
            std::scoped_lock lock{m_mutex};

            auto it = m_handles.find(handle);
            if (it != m_handles.end())
            {
                m_handles.erase(it);
            }
        }

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

        void UpdateHandles(Graphics::TextureT ptr, std::optional<uint16_t> layerIndex)
        {
            std::scoped_lock lock{m_mutex};

            for (auto handle : m_handles)
            {
                if (bgfx::overrideInternal(handle, uintptr_t(ptr), layerIndex.value_or(0)) == 0)
                {
                    assert(!"Failed to override texture");
                }
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
        struct TextureHandleLess
        {
            bool operator()(const bgfx::TextureHandle& a, const bgfx::TextureHandle& b) const
            {
                return a.idx < b.idx;
            }
        };

        mutable std::mutex m_mutex{};
        std::set<bgfx::TextureHandle, TextureHandleLess> m_handles{};
    };
}
