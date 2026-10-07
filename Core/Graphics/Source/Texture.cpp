#include <Babylon/Graphics/Texture.h>
#include <Babylon/Graphics/DeviceContext.h>
#include <Babylon/Graphics/FrameBuffer.h>
#include <algorithm>
#include <cassert>
#include <cstring>
#include <utility>

namespace
{
    // Sampled MSAA color lives in a single-sample resolve image. Its mip chain is filled
    // only when resolve runs with BGFX_ATTACHMENT_AUTO_GEN_MIPS, which happens when the
    // backend leaves that framebuffer. A later readback resolves mip 0 and drops this flag.
    void SubmitResolveSwitch(Babylon::Graphics::DeviceContext& context)
    {
        context.FlushViewsIfNeeded();
        if (bgfx::getStats()->numFrameBuffers == bgfx::getCaps()->limits.maxFrameBuffers &&
            !context.ForceMidFrameFlush())
        {
            throw std::runtime_error{"No framebuffer available for render-target initialization"};
        }

        const auto texture = bgfx::createTexture2D(
            1, 1, false, 1, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_RT | BGFX_TEXTURE_RT_WRITE_ONLY);
        if (!bgfx::isValid(texture))
        {
            throw std::runtime_error{"Failed to create render-target initialization framebuffer"};
        }

        bgfx::Attachment attachment{};
        attachment.init(texture);
        const auto frameBuffer = bgfx::createFrameBuffer(1, &attachment, true);
        if (!bgfx::isValid(frameBuffer))
        {
            bgfx::destroy(texture);
            throw std::runtime_error{"Failed to create render-target initialization framebuffer"};
        }

        const auto viewId = context.AcquireNewViewId();
        bgfx::resetView(viewId);
        bgfx::setViewMode(viewId, bgfx::ViewMode::Sequential);
        bgfx::setViewFrameBuffer(viewId, frameBuffer);
        bgfx::setViewRect(viewId, 0, 0, 1, 1);
        bgfx::setViewClear(viewId, BGFX_CLEAR_NONE, 0, 0.0f, 0);
        constexpr uint8_t discardFlags{BGFX_DISCARD_ALL & ~(BGFX_DISCARD_BINDINGS | BGFX_DISCARD_STATE)};
        auto* encoder = context.GetActiveEncoder();
        encoder->discard(discardFlags);
        encoder->submit(viewId, BGFX_INVALID_HANDLE, 0, discardFlags);
        bgfx::destroy(frameBuffer);
    }

    void ClearRenderTarget(Babylon::Graphics::DeviceContext& context, bgfx::TextureHandle handle,
        uint16_t width, uint16_t height, bool hasMips, uint16_t numLayers, bgfx::TextureFormat::Enum format, uint64_t flags)
    {
        const bool depthStencil = format > bgfx::TextureFormat::UnknownDepth;
        const bool multisampled = (flags & BGFX_TEXTURE_RT_MSAA_MASK) > BGFX_TEXTURE_RT;
        // bgfx::clear covers sampled color on every backend, including OpenGL.
        // It does not cover depth, or a sampled MSAA resolve image: D3D12 and Vulkan
        // clear the multisample image, which has only mip 0.
        if (!depthStencil && !multisampled)
        {
            bgfx::clear(handle);
            return;
        }

        const bool depth = depthStencil && format != bgfx::TextureFormat::D0S8;
        const bool stencil = format == bgfx::TextureFormat::D24S8 ||
            format == bgfx::TextureFormat::D32FS8 || format == bgfx::TextureFormat::D0S8;
        const uint16_t clearFlags = depthStencil ?
            (depth ? BGFX_CLEAR_DEPTH : 0) | (stencil ? BGFX_CLEAR_STENCIL : 0) : BGFX_CLEAR_COLOR;
        bgfx::TextureInfo info{};
        bgfx::calcTextureSize(info, width, height, /*depth*/ 1, /*cubeMap*/ false, hasMips, numLayers, format);
        // BGFX_TEXTURE_MSAA_SAMPLE keeps the sampled image multisampled, so it has no resolve mip chain.
        const bool resolveMipChain = multisampled && !depthStencil && info.numMips > 1 &&
            (flags & BGFX_TEXTURE_MSAA_SAMPLE) == 0;
        auto scope = context.AcquireFrameCompletionScope();
        for (uint16_t layer = 0; layer < info.numLayers; ++layer)
        {
            for (uint8_t mip = 0; mip < info.numMips; ++mip)
            {
                context.FlushViewsIfNeeded();
                const auto mipWidth = static_cast<uint16_t>(std::max(1, width >> mip));
                const auto mipHeight = static_cast<uint16_t>(std::max(1, height >> mip));
                if (resolveMipChain && mip != 0)
                {
                    continue;
                }
                if (multisampled && mip != 0)
                {
                    // Depth/stencil has no single-sample resolve image. Only mip 0 has MSAA storage.
                    bgfx::TextureRegion source{};
                    source.init(handle, 0, 0, mipWidth, mipHeight);
                    source.z = layer;
                    bgfx::TextureRegion destination = source;
                    destination.mip = mip;
                    const auto viewId = context.AcquireNewViewId();
                    bgfx::resetView(viewId);
                    context.GetActiveEncoder()->blit(viewId, destination, source);
                    continue;
                }

                bgfx::Attachment attachment{};
                attachment.init(handle, bgfx::Access::Write, layer, 1, mip,
                    resolveMipChain ? BGFX_ATTACHMENT_AUTO_GEN_MIPS : BGFX_ATTACHMENT_NONE);
                if (bgfx::getStats()->numFrameBuffers == bgfx::getCaps()->limits.maxFrameBuffers &&
                    !context.ForceMidFrameFlush())
                {
                    throw std::runtime_error{"No framebuffer available for render-target initialization"};
                }
                const auto frameBufferHandle = bgfx::createFrameBuffer(1, &attachment);
                if (!bgfx::isValid(frameBufferHandle))
                {
                    throw std::runtime_error{"Failed to create render-target initialization framebuffer"};
                }
                Babylon::Graphics::FrameBuffer frameBuffer{context, frameBufferHandle, mipWidth, mipHeight, false, depth, stencil, -1, multisampled};
                frameBuffer.Clear(*context.GetActiveEncoder(), clearFlags, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0);
            }
        }

        if (resolveMipChain)
        {
            SubmitResolveSwitch(context);
        }
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

        if (m_nativeTextureOwner)
        {
            // Cross a full render boundary, even when Dispose is called from AfterRender.
            // Only the native owner is deferred; the Texture wrapper is never accessed.
            arcana::make_task(m_deviceContext.BeforeRenderScheduler(), arcana::cancellation::none(), [] {})
                .then(m_deviceContext.AfterRenderScheduler(), arcana::cancellation::none(),
                    [owner = std::move(m_nativeTextureOwner)] { (void)owner; });
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

    void Texture::Create2D(uint16_t width, uint16_t height, bool hasMips, uint16_t numLayers, bgfx::TextureFormat::Enum format, uint64_t flags, uintptr_t nativeTextureHandle, std::shared_ptr<void> nativeTextureOwner)
    {
        Dispose();

        // Create Babylon-owned textures with BGFX_TEXTURE_BLIT_DST to match web behavior.
        const auto createFlags = nativeTextureHandle == 0 ? flags | BGFX_TEXTURE_BLIT_DST : flags;

        m_handle = bgfx::createTexture2D(width, height, hasMips, numLayers, format, createFlags, nullptr, nativeTextureHandle);
        if (!bgfx::isValid(m_handle))
        {
            throw std::runtime_error{"Failed to create texture"};
        }

        m_ownsHandle = true;
        m_nativeTextureOwner = std::move(nativeTextureOwner);
        SetMetadata(width, height, 0, hasMips, false, false, numLayers, format, flags);

        // Make sure render targets are filled with 0 : https://registry.khronos.org/webgl/specs/latest/1.0/#TEXIMAGE2D
        if (nativeTextureHandle == 0 && (flags & BGFX_TEXTURE_RT_MASK) != 0)
        {
            ClearRenderTarget(m_deviceContext, m_handle, width, height, hasMips, numLayers, format, flags);
        }
    }

    void Texture::Update2D(uint16_t layer, uint8_t mip, uint16_t x, uint16_t y, uint16_t width, uint16_t height, const bgfx::Memory* mem, uint16_t pitch)
    {
        bgfx::updateTexture2D(m_handle, layer, mip, x, y, width, height, mem, pitch);
    }

    void Texture::Create3D(uint16_t width, uint16_t height, uint16_t depth, bool hasMips, bgfx::TextureFormat::Enum format, uint64_t flags)
    {
        Dispose();

        // bgfx needs two physical slices for a 3D view; retain the logical depth.
        const uint16_t physicalDepth = std::max<uint16_t>(2, depth);
        auto createFlags = flags;
        if (depth == 1 && (flags & BGFX_TEXTURE_RT_MASK) != 0)
        {
            createFlags |= BGFX_TEXTURE_BLIT_DST;
        }

        m_handle = bgfx::createTexture3D(width, height, physicalDepth, hasMips, format, createFlags);
        if (!bgfx::isValid(m_handle))
        {
            throw std::runtime_error{"Failed to create 3D texture"};
        }

        m_ownsHandle = true;
        SetMetadata(width, height, depth, hasMips, false, true, 1, format, flags);
        if (depth == 1)
        {
            bgfx::TextureInfo info{};
            bgfx::calcTextureSize(info, width, height, physicalDepth, false, hasMips, 1, format);
            for (uint8_t mip = 0; mip < info.numMips; ++mip)
            {
                const auto mipWidth = static_cast<uint16_t>(std::max(1, width >> mip));
                const auto mipHeight = static_cast<uint16_t>(std::max(1, height >> mip));
                const auto mipDepth = static_cast<uint16_t>(std::max(1, physicalDepth >> mip));
                bgfx::TextureInfo mipInfo{};
                bgfx::calcTextureSize(mipInfo, mipWidth, mipHeight, mipDepth, false, false, 1, format);
                const auto memory = bgfx::alloc(mipInfo.storageSize);
                std::memset(memory->data, 0, memory->size);
                bgfx::updateTexture3D(m_handle, mip, 0, 0, 0, mipWidth, mipHeight, mipDepth, memory);
            }
        }
    }

    void Texture::Update3D(uint8_t mip, uint16_t x, uint16_t y, uint16_t z, uint16_t width, uint16_t height, uint16_t depth, const bgfx::Memory* mem)
    {
        // Keep the padding identical, including partial updates, for normalized filtering.
        const auto padding = m_depth == 1 && mip == 0 && z == 0 && depth == 1
            ? bgfx::copy(mem->data, mem->size) : nullptr;
        bgfx::updateTexture3D(m_handle, mip, x, y, z, width, height, depth, mem);
        if (padding)
        {
            bgfx::updateTexture3D(m_handle, mip, x, y, 1, width, height, 1, padding);
        }
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

    void Texture::Attach(bgfx::TextureHandle handle, bool ownsHandle, uint16_t width, uint16_t height, bool hasMips, uint16_t numLayers, bgfx::TextureFormat::Enum format, uint64_t flags)
    {
        Dispose();

        assert(bgfx::isValid(handle));
        m_handle = handle;
        m_ownsHandle = ownsHandle;
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

    uint8_t Texture::SamplerMaxLod() const
    {
        return m_samplerMaxLod;
    }

    void Texture::SamplerMaxLod(uint8_t value)
    {
        m_samplerMaxLod = value;
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
