#include "Texture.h"
#include <cassert>

namespace Babylon::Graphics
{
    Texture::~Texture()
    {
        Dispose();
    }

    void Texture::Dispose()
    {
        if (m_ownsHandle && bgfx::isValid(m_handle))
        {
            bgfx::destroy(m_handle);
            m_handle = BGFX_INVALID_HANDLE;
            m_ownsHandle = false;
        }
    }

    bool Texture::IsValid() const
    {
        return bgfx::isValid(m_handle);
    }

    void Texture::Create2D(uint16_t width, uint16_t height, bool hasMips, uint16_t numLayers, bgfx::TextureFormat::Enum format, uint64_t flags)
    {
        Dispose();

        // Always create with BGFX_TEXTURE_BLIT_DST to match web behavior.
        m_handle = bgfx::createTexture2D(width, height, hasMips, numLayers, format, flags | BGFX_TEXTURE_BLIT_DST);
        m_ownsHandle = true;
        m_width = width;
        m_height = height;
    }

    void Texture::Update2D(uint16_t layer, uint8_t mip, uint16_t x, uint16_t y, uint16_t width, uint16_t height, const bgfx::Memory* mem, uint16_t pitch)
    {
        bgfx::updateTexture2D(m_handle, layer, mip, x, y, width, height, mem, pitch);
    }

    void Texture::CreateCube(uint16_t size, bool hasMips, uint16_t numLayers, bgfx::TextureFormat::Enum format, uint64_t flags)
    {
        Dispose();

        m_handle = bgfx::createTextureCube(size, hasMips, numLayers, format, flags);
        m_ownsHandle = true;
        m_width = size;
        m_height = size;
    }

    void Texture::UpdateCube(uint16_t layer, uint8_t side, uint8_t mip, uint16_t x, uint16_t y, uint16_t width, uint16_t height, const bgfx::Memory* mem, uint16_t pitch)
    {
        bgfx::updateTextureCube(m_handle, layer, side, mip, x, y, width, height, mem, pitch);
    }

    void Texture::Attach(bgfx::TextureHandle handle, bool ownsHandle, uint16_t width, uint16_t height)
    {
        Dispose();

        assert(bgfx::isValid(handle));
        m_handle = handle;
        m_ownsHandle = ownsHandle;
        m_width = width;
        m_height = height;
    }

    bgfx::TextureHandle Texture::Handle() const
    {
        return m_handle;
    }

    uint16_t Texture::Width() const
    {
        return m_width;
    }

    uint16_t Texture::Height() const
    {
        return m_height;
    }

    uint32_t Texture::SamplerFlags() const
    {
        return m_samplerFlags;
    }

    void Texture::SamplerFlags(uint32_t value)
    {
        m_samplerFlags = value;
    }
}
