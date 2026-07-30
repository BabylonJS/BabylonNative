#pragma once

#include <bgfx/bgfx.h>

namespace Babylon::Graphics
{
    class DeviceContext;

    class Texture final
    {
    public:
        Texture(DeviceContext& deviceContext);
        ~Texture();

        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        void Dispose();

        bool IsValid() const;

        void Create2D(uint16_t width, uint16_t height, bool hasMips, uint16_t numLayers, bgfx::TextureFormat::Enum format, uint64_t flags);
        void Update2D(uint16_t layer, uint8_t mip, uint16_t x, uint16_t y, uint16_t width, uint16_t height, const bgfx::Memory* mem, uint16_t pitch = UINT16_MAX);

        void CreateCube(uint16_t size, bool hasMips, uint16_t numLayers, bgfx::TextureFormat::Enum format, uint64_t flags);
        void UpdateCube(uint16_t layer, uint8_t side, uint8_t mip, uint16_t x, uint16_t y, uint16_t width, uint16_t height, const bgfx::Memory* mem, uint16_t pitch = UINT16_MAX);

        void Attach(bgfx::TextureHandle handle, bool ownsHandle, uint16_t width, uint16_t height, bool hasMips, uint16_t numLayers, bgfx::TextureFormat::Enum format, uint64_t flags);

        bgfx::TextureHandle Handle() const;
        uint16_t Width() const;
        uint16_t Height() const;
        bool HasMips() const;
        uint16_t NumLayers() const;
        bgfx::TextureFormat::Enum Format() const;
        uint64_t Flags() const;
        uint32_t SamplerFlags() const;
        void SamplerFlags(uint32_t);

        // View sub-range used at bind time to select a single array slice of a multi-layer
        // texture (bgfx setTexture view API). ViewNumLayers == 0 means "no override; bind whole texture".
        uint16_t ViewFirstLayer() const;
        void ViewFirstLayer(uint16_t);
        uint16_t ViewNumLayers() const;
        void ViewNumLayers(uint16_t);

        // View id reserved (by the Canvas polyfill) for the canvas->texture blit that fills
        // this texture. UINT16_MAX means "unset"; consumers fall back to a freshly peeked view.
        bgfx::ViewId BlitViewId() const { return m_blitViewId; }
        void BlitViewId(bgfx::ViewId viewId) { m_blitViewId = viewId; }

    private:
        bgfx::TextureHandle m_handle{bgfx::kInvalidHandle};
        bool m_ownsHandle{false};
        uint16_t m_width{0};
        uint16_t m_height{0};
        bool m_hasMips{false};
        uint16_t m_numLayers{0};
        bgfx::TextureFormat::Enum m_format{bgfx::TextureFormat::Enum::Unknown};
        uint64_t m_flags{BGFX_TEXTURE_NONE};
        uint32_t m_samplerFlags{BGFX_SAMPLER_NONE};
        uint16_t m_viewFirstLayer{0};
        uint16_t m_viewNumLayers{0};
        bgfx::ViewId m_blitViewId{UINT16_MAX};
        uintptr_t m_deviceID;
        DeviceContext& m_deviceContext;
    };
}
