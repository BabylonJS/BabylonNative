#include <Babylon/Graphics/Texture.h>
#include <Babylon/Graphics/DeviceContext.h>
#include <cassert>
#include <cstring>

namespace
{
    const bgfx::Memory* GetZeroImageMemory(uint16_t width, uint16_t height, bool hasMips, uint16_t numLayers, bgfx::TextureFormat::Enum format)
    {
        bgfx::TextureInfo info{};
        bgfx::calcTextureSize(info, width, height, /*depth*/ 1, /*cubeMap*/ false, hasMips, numLayers, format);
        const bgfx::Memory* mem = bgfx::alloc(info.storageSize);
        std::memset(mem->data, 0, mem->size);
        return mem;
    }
}

namespace Babylon::Graphics
{
    Texture::Texture(DeviceContext& deviceContext)
        : m_deviceID{deviceContext.GetDeviceId()}
        , m_deviceContext{deviceContext}
    {
    }

    Texture::~Texture()
    {
        Dispose();
    }

    void Texture::Dispose()
    {
        if (m_ownsHandle && bgfx::isValid(m_handle) && m_deviceID == m_deviceContext.GetDeviceId())
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

    void Texture::SetMetadata(
        uint16_t width,
        uint16_t height,
        uint16_t depth,
        bool hasMips,
        bool isCube,
        bool is3D,
        uint16_t numLayers,
        bgfx::TextureFormat::Enum format,
        uint64_t flags)
    {
        m_width = width;
        m_height = height;
        m_depth = depth;
        m_hasMips = hasMips;
        m_isCube = isCube;
        m_is3D = is3D;
        m_numLayers = numLayers;
        m_format = format;
        m_flags = flags;
    }

    void Texture::Create2D(uint16_t width, uint16_t height, bool hasMips, uint16_t numLayers, bgfx::TextureFormat::Enum format, uint64_t flags, uintptr_t nativeTextureHandle)
    {
        Dispose();

        // Create Babylon-owned textures with BGFX_TEXTURE_BLIT_DST to match web behavior.
        const auto createFlags = nativeTextureHandle == 0 ? flags | BGFX_TEXTURE_BLIT_DST : flags;

        // Make sure render targets are filled with 0 : https://registry.khronos.org/webgl/specs/latest/1.0/#TEXIMAGE2D
        const auto* mem = nativeTextureHandle == 0 && (flags & BGFX_TEXTURE_RT) ? GetZeroImageMemory(width, height, hasMips, numLayers, format) : nullptr;

        m_handle = bgfx::createTexture2D(width, height, hasMips, numLayers, format, createFlags, mem, nativeTextureHandle);
        if (!bgfx::isValid(m_handle))
        {
            throw std::runtime_error{"Failed to create texture"};
        }

        m_ownsHandle = true;
        SetMetadata(width, height, 0, hasMips, false, false, numLayers, format, flags);
    }

    void Texture::Update2D(uint16_t layer, uint8_t mip, uint16_t x, uint16_t y, uint16_t width, uint16_t height, const bgfx::Memory* mem, uint16_t pitch)
    {
        bgfx::updateTexture2D(m_handle, layer, mip, x, y, width, height, mem, pitch);
    }

    void Texture::Create3D(uint16_t width, uint16_t height, uint16_t depth, bool hasMips, bgfx::TextureFormat::Enum format, uint64_t flags)
    {
        Dispose();

        m_handle = bgfx::createTexture3D(width, height, depth, hasMips, format, flags);
        if (!bgfx::isValid(m_handle))
        {
            throw std::runtime_error{"Failed to create 3D texture"};
        }

        m_ownsHandle = true;
        SetMetadata(width, height, depth, hasMips, false, true, 1, format, flags);
    }

    void Texture::Update3D(uint8_t mip, uint16_t x, uint16_t y, uint16_t z, uint16_t width, uint16_t height, uint16_t depth, const bgfx::Memory* mem)
    {
        bgfx::updateTexture3D(m_handle, mip, x, y, z, width, height, depth, mem);
    }

    void Texture::CreateCube(uint16_t size, bool hasMips, uint16_t numLayers, bgfx::TextureFormat::Enum format, uint64_t flags)
    {
        Dispose();

        m_handle = bgfx::createTextureCube(size, hasMips, numLayers, format, flags);
        if (!bgfx::isValid(m_handle))
        {
            throw std::runtime_error{"Failed to create cube texture"};
        }

        m_ownsHandle = true;
        SetMetadata(size, size, 0, hasMips, true, false, numLayers, format, flags);
    }

    void Texture::UpdateCube(uint16_t layer, uint8_t side, uint8_t mip, uint16_t x, uint16_t y, uint16_t width, uint16_t height, const bgfx::Memory* mem, uint16_t pitch)
    {
        bgfx::updateTextureCube(m_handle, layer, side, mip, x, y, width, height, mem, pitch);
    }

    void Texture::Attach(bgfx::TextureHandle handle, uint16_t width, uint16_t height, bool hasMips, uint16_t numLayers, bgfx::TextureFormat::Enum format, uint64_t flags)
    {
        Dispose();

        assert(bgfx::isValid(handle));
        m_handle = handle;

        m_ownsHandle = false;
        SetMetadata(width, height, 0, hasMips, false, false, numLayers, format, flags);
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

    bool Texture::HasMips() const
    {
        return m_hasMips;
    }

    bool Texture::IsCube() const
    {
        return m_isCube;
    }

    bool Texture::Is3D() const
    {
        return m_is3D;
    }

    uint16_t Texture::NumLayers() const
    {
        return m_numLayers;
    }

    uint16_t Texture::Depth() const
    {
        return m_depth;
    }

    bgfx::TextureFormat::Enum Texture::Format() const
    {
        return m_format;
    }

    uint64_t Texture::Flags() const
    {
        return m_flags;
    }

    uint32_t Texture::SamplerFlags() const
    {
        return m_samplerFlags;
    }

    void Texture::SamplerFlags(uint32_t value)
    {
        m_samplerFlags = value;
    }

    uint16_t Texture::ViewFirstLayer() const
    {
        return m_viewFirstLayer;
    }

    void Texture::ViewFirstLayer(uint16_t value)
    {
        m_viewFirstLayer = value;
    }

    uint16_t Texture::ViewNumLayers() const
    {
        return m_viewNumLayers;
    }

    void Texture::ViewNumLayers(uint16_t value)
    {
        m_viewNumLayers = value;
    }
}
