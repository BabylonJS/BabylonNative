#include <Babylon/Graphics/Texture.h>
#include <Babylon/Graphics/DeviceContext.h>
#include <cassert>
#include <cstring>
#include <mutex>
#include <utility>

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
    struct Texture::DeferredUpdateState final
    {
        explicit DeferredUpdateState(DeviceContext& deviceContext)
            : DeviceID{deviceContext.GetDeviceId()}
            , DeviceContextRef{deviceContext}
        {
        }

        std::mutex Mutex;
        bgfx::TextureHandle Handle{bgfx::kInvalidHandle};
        bool OwnsHandle{false};
        uint16_t Width{0};
        uint16_t Height{0};
        bool HasMips{false};
        bool IsCube{false};
        bool Is3D{false};
        uint16_t NumLayers{0};
        uint16_t Depth{0};
        bgfx::TextureFormat::Enum Format{bgfx::TextureFormat::Enum::Unknown};
        uint64_t Flags{BGFX_TEXTURE_NONE};
        uint32_t SamplerFlags{BGFX_SAMPLER_NONE};
        uint16_t ViewFirstLayer{0};
        uint16_t ViewNumLayers{0};
        bgfx::ViewId BlitViewId{UINT16_MAX};
        uint32_t BlitViewIdGeneration{0};
        uintptr_t DeviceID;
        DeviceContext& DeviceContextRef;
        uint64_t Generation{};
        bool UpdatesAllowed{false};
    };

    Texture::DeferredUpdate::DeferredUpdate(
        std::shared_ptr<DeferredUpdateState> state,
        uint64_t generation,
        bool hasMips,
        uint16_t numLayers,
        bgfx::TextureFormat::Enum format,
        uint64_t flags)
        : m_state{std::move(state)}
        , m_generation{generation}
        , m_hasMips{hasMips}
        , m_numLayers{numLayers}
        , m_format{format}
        , m_flags{flags}
    {
    }

    bool Texture::DeferredUpdate::Matches(const DeferredUpdate& other) const
    {
        return m_state == other.m_state && m_generation == other.m_generation;
    }

    bool Texture::DeferredUpdate::TryCreate2D(uint16_t width, uint16_t height, uintptr_t nativeTextureHandle)
    {
        std::scoped_lock lock{m_state->Mutex};
        if (!m_state->UpdatesAllowed ||
            m_state->Generation != m_generation)
        {
            return false;
        }

        Texture::Create2DLocked(*m_state, width, height, m_hasMips, m_numLayers, m_format, m_flags, nativeTextureHandle);
        m_generation = m_state->Generation;
        return true;
    }

    Texture::Texture(DeviceContext& deviceContext)
        : m_deferredUpdateState{std::make_shared<DeferredUpdateState>(deviceContext)}
    {
    }

    Texture::~Texture()
    {
        Dispose();
    }

    void Texture::Dispose()
    {
        std::scoped_lock lock{m_deferredUpdateState->Mutex};
        DisposeHandle(*m_deferredUpdateState);
        m_deferredUpdateState->UpdatesAllowed = false;
        ++m_deferredUpdateState->Generation;
    }

    void Texture::DisposeHandle(DeferredUpdateState& state)
    {
        if (state.OwnsHandle && bgfx::isValid(state.Handle) && state.DeviceID == state.DeviceContextRef.GetDeviceId())
        {
            bgfx::destroy(state.Handle);
            state.Handle = BGFX_INVALID_HANDLE;
            state.OwnsHandle = false;
        }
    }

    bool Texture::IsValid() const
    {
        std::scoped_lock lock{m_deferredUpdateState->Mutex};
        return bgfx::isValid(m_deferredUpdateState->Handle);
    }

    Texture::DeferredUpdate Texture::CreateDeferredUpdate() const
    {
        std::scoped_lock lock{m_deferredUpdateState->Mutex};
        return DeferredUpdate{
            m_deferredUpdateState,
            m_deferredUpdateState->Generation,
            m_deferredUpdateState->HasMips,
            m_deferredUpdateState->NumLayers,
            m_deferredUpdateState->Format,
            m_deferredUpdateState->Flags};
    }

    void Texture::SetMetadata(
        DeferredUpdateState& state,
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
        state.Width = width;
        state.Height = height;
        state.Depth = depth;
        state.HasMips = hasMips;
        state.IsCube = isCube;
        state.Is3D = is3D;
        state.NumLayers = numLayers;
        state.Format = format;
        state.Flags = flags;
    }

    void Texture::Create2D(uint16_t width, uint16_t height, bool hasMips, uint16_t numLayers, bgfx::TextureFormat::Enum format, uint64_t flags, uintptr_t nativeTextureHandle)
    {
        std::scoped_lock lock{m_deferredUpdateState->Mutex};
        Create2DLocked(*m_deferredUpdateState, width, height, hasMips, numLayers, format, flags, nativeTextureHandle);
    }

    void Texture::Create2DLocked(DeferredUpdateState& state, uint16_t width, uint16_t height, bool hasMips, uint16_t numLayers, bgfx::TextureFormat::Enum format, uint64_t flags, uintptr_t nativeTextureHandle)
    {
        DisposeHandle(state);
        ++state.Generation;
        state.UpdatesAllowed = false;

        // Create Babylon-owned textures with BGFX_TEXTURE_BLIT_DST to match web behavior.
        const auto createFlags = nativeTextureHandle == 0 ? flags | BGFX_TEXTURE_BLIT_DST : flags;

        // Make sure render targets are filled with 0 : https://registry.khronos.org/webgl/specs/latest/1.0/#TEXIMAGE2D
        const auto* mem = nativeTextureHandle == 0 && (flags & BGFX_TEXTURE_RT) ? GetZeroImageMemory(width, height, hasMips, numLayers, format) : nullptr;

        state.Handle = bgfx::createTexture2D(width, height, hasMips, numLayers, format, createFlags, mem, nativeTextureHandle);
        if (!bgfx::isValid(state.Handle))
        {
            throw std::runtime_error{"Failed to create texture"};
        }

        state.OwnsHandle = true;
        SetMetadata(state, width, height, 0, hasMips, false, false, numLayers, format, flags);
        state.UpdatesAllowed = true;
    }

    void Texture::Update2D(uint16_t layer, uint8_t mip, uint16_t x, uint16_t y, uint16_t width, uint16_t height, const bgfx::Memory* mem, uint16_t pitch)
    {
        std::scoped_lock lock{m_deferredUpdateState->Mutex};
        bgfx::updateTexture2D(m_deferredUpdateState->Handle, layer, mip, x, y, width, height, mem, pitch);
    }

    void Texture::Create3D(uint16_t width, uint16_t height, uint16_t depth, bool hasMips, bgfx::TextureFormat::Enum format, uint64_t flags)
    {
        std::scoped_lock lock{m_deferredUpdateState->Mutex};
        DisposeHandle(*m_deferredUpdateState);
        ++m_deferredUpdateState->Generation;
        m_deferredUpdateState->UpdatesAllowed = false;

        m_deferredUpdateState->Handle = bgfx::createTexture3D(width, height, depth, hasMips, format, flags);
        if (!bgfx::isValid(m_deferredUpdateState->Handle))
        {
            throw std::runtime_error{"Failed to create 3D texture"};
        }

        m_deferredUpdateState->OwnsHandle = true;
        SetMetadata(*m_deferredUpdateState, width, height, depth, hasMips, false, true, 1, format, flags);
        m_deferredUpdateState->UpdatesAllowed = true;
    }

    void Texture::Update3D(uint8_t mip, uint16_t x, uint16_t y, uint16_t z, uint16_t width, uint16_t height, uint16_t depth, const bgfx::Memory* mem)
    {
        std::scoped_lock lock{m_deferredUpdateState->Mutex};
        bgfx::updateTexture3D(m_deferredUpdateState->Handle, mip, x, y, z, width, height, depth, mem);
    }

    void Texture::CreateCube(uint16_t size, bool hasMips, uint16_t numLayers, bgfx::TextureFormat::Enum format, uint64_t flags)
    {
        std::scoped_lock lock{m_deferredUpdateState->Mutex};
        DisposeHandle(*m_deferredUpdateState);
        ++m_deferredUpdateState->Generation;
        m_deferredUpdateState->UpdatesAllowed = false;

        m_deferredUpdateState->Handle = bgfx::createTextureCube(size, hasMips, numLayers, format, flags);
        if (!bgfx::isValid(m_deferredUpdateState->Handle))
        {
            throw std::runtime_error{"Failed to create cube texture"};
        }

        m_deferredUpdateState->OwnsHandle = true;
        SetMetadata(*m_deferredUpdateState, size, size, 0, hasMips, true, false, numLayers, format, flags);
        m_deferredUpdateState->UpdatesAllowed = true;
    }

    void Texture::UpdateCube(uint16_t layer, uint8_t side, uint8_t mip, uint16_t x, uint16_t y, uint16_t width, uint16_t height, const bgfx::Memory* mem, uint16_t pitch)
    {
        std::scoped_lock lock{m_deferredUpdateState->Mutex};
        bgfx::updateTextureCube(m_deferredUpdateState->Handle, layer, side, mip, x, y, width, height, mem, pitch);
    }

    void Texture::Attach(bgfx::TextureHandle handle, uint16_t width, uint16_t height, bool hasMips, uint16_t numLayers, bgfx::TextureFormat::Enum format, uint64_t flags)
    {
        std::scoped_lock lock{m_deferredUpdateState->Mutex};
        DisposeHandle(*m_deferredUpdateState);
        ++m_deferredUpdateState->Generation;
        m_deferredUpdateState->UpdatesAllowed = false;

        assert(bgfx::isValid(handle));
        m_deferredUpdateState->Handle = handle;

        m_deferredUpdateState->OwnsHandle = false;
        SetMetadata(*m_deferredUpdateState, width, height, 0, hasMips, false, false, numLayers, format, flags);
        m_deferredUpdateState->UpdatesAllowed = true;
    }

    bgfx::TextureHandle Texture::Handle() const
    {
        std::scoped_lock lock{m_deferredUpdateState->Mutex};
        return m_deferredUpdateState->Handle;
    }

    uint16_t Texture::Width() const
    {
        std::scoped_lock lock{m_deferredUpdateState->Mutex};
        return m_deferredUpdateState->Width;
    }

    uint16_t Texture::Height() const
    {
        std::scoped_lock lock{m_deferredUpdateState->Mutex};
        return m_deferredUpdateState->Height;
    }

    bool Texture::HasMips() const
    {
        std::scoped_lock lock{m_deferredUpdateState->Mutex};
        return m_deferredUpdateState->HasMips;
    }

    bool Texture::IsCube() const
    {
        std::scoped_lock lock{m_deferredUpdateState->Mutex};
        return m_deferredUpdateState->IsCube;
    }

    bool Texture::Is3D() const
    {
        std::scoped_lock lock{m_deferredUpdateState->Mutex};
        return m_deferredUpdateState->Is3D;
    }

    uint16_t Texture::NumLayers() const
    {
        std::scoped_lock lock{m_deferredUpdateState->Mutex};
        return m_deferredUpdateState->NumLayers;
    }

    uint16_t Texture::Depth() const
    {
        std::scoped_lock lock{m_deferredUpdateState->Mutex};
        return m_deferredUpdateState->Depth;
    }

    bgfx::TextureFormat::Enum Texture::Format() const
    {
        std::scoped_lock lock{m_deferredUpdateState->Mutex};
        return m_deferredUpdateState->Format;
    }

    uint64_t Texture::Flags() const
    {
        std::scoped_lock lock{m_deferredUpdateState->Mutex};
        return m_deferredUpdateState->Flags;
    }

    uint32_t Texture::SamplerFlags() const
    {
        std::scoped_lock lock{m_deferredUpdateState->Mutex};
        return m_deferredUpdateState->SamplerFlags;
    }

    void Texture::SamplerFlags(uint32_t value)
    {
        std::scoped_lock lock{m_deferredUpdateState->Mutex};
        m_deferredUpdateState->SamplerFlags = value;
    }

    uint16_t Texture::ViewFirstLayer() const
    {
        std::scoped_lock lock{m_deferredUpdateState->Mutex};
        return m_deferredUpdateState->ViewFirstLayer;
    }

    void Texture::ViewFirstLayer(uint16_t value)
    {
        std::scoped_lock lock{m_deferredUpdateState->Mutex};
        m_deferredUpdateState->ViewFirstLayer = value;
    }

    uint16_t Texture::ViewNumLayers() const
    {
        std::scoped_lock lock{m_deferredUpdateState->Mutex};
        return m_deferredUpdateState->ViewNumLayers;
    }

    void Texture::ViewNumLayers(uint16_t value)
    {
        std::scoped_lock lock{m_deferredUpdateState->Mutex};
        m_deferredUpdateState->ViewNumLayers = value;
    }

    bgfx::ViewId Texture::BlitViewId() const
    {
        std::scoped_lock lock{m_deferredUpdateState->Mutex};
        return m_deferredUpdateState->BlitViewId;
    }

    uint32_t Texture::BlitViewIdGeneration() const
    {
        std::scoped_lock lock{m_deferredUpdateState->Mutex};
        return m_deferredUpdateState->BlitViewIdGeneration;
    }

    void Texture::BlitViewId(bgfx::ViewId viewId, uint32_t generation)
    {
        std::scoped_lock lock{m_deferredUpdateState->Mutex};
        m_deferredUpdateState->BlitViewId = viewId;
        m_deferredUpdateState->BlitViewIdGeneration = generation;
    }
}
