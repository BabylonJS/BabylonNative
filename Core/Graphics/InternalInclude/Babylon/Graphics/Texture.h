#pragma once

#include <bgfx/bgfx.h>
#include <cstdint>
#include <memory>

namespace Babylon::Graphics
{
    class DeviceContext;

    class Texture final
    {
        struct DeferredUpdateState;

    public:
        class DeferredUpdate final
        {
        public:
            // Matches only the texture state and generation captured by each token.
            bool Matches(const DeferredUpdate& other) const;

            // Returns false if the texture was disposed, destroyed, or recreated since
            // this update was captured. The token does not keep the Texture wrapper alive.
            bool TryCreate2D(uint16_t width, uint16_t height, uintptr_t nativeTextureHandle);

        private:
            friend class Texture;

            DeferredUpdate(
                std::shared_ptr<DeferredUpdateState> state,
                uint64_t generation,
                bool hasMips,
                uint16_t numLayers,
                bgfx::TextureFormat::Enum format,
                uint64_t flags);

            std::shared_ptr<DeferredUpdateState> m_state;
            uint64_t m_generation;
            bool m_hasMips;
            uint16_t m_numLayers;
            bgfx::TextureFormat::Enum m_format;
            uint64_t m_flags;
        };

        Texture(DeviceContext& deviceContext);
        ~Texture();

        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        void Dispose();

        bool IsValid() const;
        DeferredUpdate CreateDeferredUpdate() const;

        void Create2D(uint16_t width, uint16_t height, bool hasMips, uint16_t numLayers, bgfx::TextureFormat::Enum format, uint64_t flags, uintptr_t nativeTextureHandle = 0);
        void Update2D(uint16_t layer, uint8_t mip, uint16_t x, uint16_t y, uint16_t width, uint16_t height, const bgfx::Memory* mem, uint16_t pitch = UINT16_MAX);

        void Create3D(uint16_t width, uint16_t height, uint16_t depth, bool hasMips, bgfx::TextureFormat::Enum format, uint64_t flags);
        void Update3D(uint8_t mip, uint16_t x, uint16_t y, uint16_t z, uint16_t width, uint16_t height, uint16_t depth, const bgfx::Memory* mem);

        void CreateCube(uint16_t size, bool hasMips, uint16_t numLayers, bgfx::TextureFormat::Enum format, uint64_t flags);
        void UpdateCube(uint16_t layer, uint8_t side, uint8_t mip, uint16_t x, uint16_t y, uint16_t width, uint16_t height, const bgfx::Memory* mem, uint16_t pitch = UINT16_MAX);

        void Attach(bgfx::TextureHandle handle, uint16_t width, uint16_t height, bool hasMips, uint16_t numLayers, bgfx::TextureFormat::Enum format, uint64_t flags);

        bgfx::TextureHandle Handle() const;
        uint16_t Width() const;
        uint16_t Height() const;
        bool HasMips() const;
        bool IsCube() const;
        bool Is3D() const;
        uint16_t NumLayers() const;
        uint16_t Depth() const;
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
        // this texture, together with the view-id generation it was reserved in. UINT16_MAX
        // means "unset"; a generation mismatch means a mid-frame view flush has since reset the
        // view counter and the reservation no longer orders before later views. In both cases
        // consumers fall back to a freshly peeked view.
        bgfx::ViewId BlitViewId() const;
        uint32_t BlitViewIdGeneration() const;
        void BlitViewId(bgfx::ViewId viewId, uint32_t generation);

    private:
        static void DisposeHandle(DeferredUpdateState& state);
        static void Create2DLocked(DeferredUpdateState& state, uint16_t width, uint16_t height, bool hasMips, uint16_t numLayers, bgfx::TextureFormat::Enum format, uint64_t flags, uintptr_t nativeTextureHandle);

        static void SetMetadata(
            DeferredUpdateState& state,
            uint16_t width,
            uint16_t height,
            uint16_t depth,
            bool hasMips,
            bool isCube,
            bool is3D,
            uint16_t numLayers,
            bgfx::TextureFormat::Enum format,
            uint64_t flags);

        std::shared_ptr<DeferredUpdateState> m_deferredUpdateState;
    };
}
