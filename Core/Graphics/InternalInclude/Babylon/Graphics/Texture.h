#pragma once

#include <bgfx/bgfx.h>

namespace Babylon::Graphics
{
    class Texture final
    {
    public:
        Texture() = default;
        ~Texture();

        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        void Dispose();

        bool IsValid() const;

        void Create2D(uint16_t width, uint16_t height, bool hasMips, uint16_t numLayers, bgfx::TextureFormat::Enum format, uint64_t flags);
        void Update2D(uint16_t layer, uint8_t mip, uint16_t x, uint16_t y, uint16_t width, uint16_t height, const bgfx::Memory* mem, uint16_t pitch = UINT16_MAX);

        void CreateCube(uint16_t size, bool hasMips, uint16_t numLayers, bgfx::TextureFormat::Enum format, uint64_t flags);
        void UpdateCube(uint16_t layer, uint8_t side, uint8_t mip, uint16_t x, uint16_t y, uint16_t width, uint16_t height, const bgfx::Memory* mem, uint16_t pitch = UINT16_MAX);

        void Attach(bgfx::TextureHandle handle, bool ownsHandle, uint16_t width, uint16_t height);

        bgfx::TextureHandle Handle() const;
        uint16_t Width() const;
        uint16_t Height() const;

        uint32_t SamplerFlags() const;
        void SamplerFlags(uint32_t);

    private:
        bgfx::TextureHandle m_handle{bgfx::kInvalidHandle};
        bool m_ownsHandle{false};
        uint16_t m_width{0};
        uint16_t m_height{0};
        uint32_t m_samplerFlags{BGFX_SAMPLER_NONE};
    };
}
