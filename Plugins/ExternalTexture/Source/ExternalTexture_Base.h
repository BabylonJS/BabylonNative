#pragma once

#include <Babylon/DebugTrace.h>
#include <Babylon/Graphics/RendererType.h>
#include <Babylon/Graphics/Texture.h>
#include <bgfx/bgfx.h>
#include <set>
#include <cassert>
#include <cmath>
#include <type_traits>

namespace Babylon::Plugins
{
    namespace
    {
        template<typename T>
        uintptr_t NativeHandleToUintPtr(T value)
        {
            if constexpr (std::is_pointer_v<T>)
            {
                return reinterpret_cast<uintptr_t>(value);
            }
            else
            {
                return static_cast<uintptr_t>(value);
            }
        }
    }

    class ExternalTexture::ImplBase
    {
    public:
        // Per the ExternalTexture public contract, all read-only accessors are called
        // only from the graphics thread, where they are serialized against Update() (also
        // graphics-thread). They do not need to lock against themselves; the mutex below
        // exists solely to bridge graphics-thread state to the JS thread for
        // CreateTexture / DestroyTexture.
        uint16_t Width() const { return m_info.Width; }
        uint16_t Height() const { return m_info.Height; }
        bgfx::TextureFormat::Enum Format() const { return m_info.Format; }
        bool HasMips() const { return m_info.MipLevels != 1; }
        uint16_t NumLayers() const { return m_info.NumLayers; }
        uint64_t Flags() const { return m_info.Flags; }

        // JS-thread entry points. These lock internally to read graphics-thread-written
        // state (m_info, derived m_ptr, m_textures) consistently and to mutate m_textures.
        // They contain no JS callouts, so the lock is never held across user-visible JS
        // execution -- preventing the recursive-mutex / finalizer-reentrancy bug class.
        Graphics::Texture* CreateTexture(Graphics::DeviceContext& context);
        void DestroyTexture(Graphics::Texture* texture);

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

        // Re-attaches every registered Graphics::Texture to a fresh bgfx handle backed by
        // `ptr`. Caller must hold m_mutex (called from the locked region of Impl::Update).
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
                    0,
                    NativeHandleToUintPtr(ptr)
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
        mutable std::mutex m_mutex{};

    private:
        std::set<Graphics::Texture*> m_textures{};
    };
}
