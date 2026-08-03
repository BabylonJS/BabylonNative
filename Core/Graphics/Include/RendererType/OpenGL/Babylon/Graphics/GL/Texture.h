#pragma once

#include <Babylon/Graphics/GL/SharedPtr.h>

#include <atomic>
#include <cstdint>
#include <functional>

namespace Babylon::Graphics::GL
{
    // An OpenGL texture name is an integer in a per-context namespace. It carries no reference
    // count, and on the OpenGL ES 3.0 context BabylonNative renders with it cannot even report
    // its own dimensions (glGetTexLevelParameteriv is ES 3.1). GL::Texture supplies both, so
    // that TextureT means the same thing on every backend: a pointer to a reference-counted
    // image that also knows the properties D3D and Metal read back from the resource itself.
    //
    // Ownership is shared, and each holder manages only its own reference: whoever receives a
    // GL::Texture* and keeps it adds a reference, and removes it when done, so no participant
    // needs to know about any other. Create returns an image with one reference, already held
    // by the caller.
    //
    // The release callback runs when the last reference goes away, and decides what releasing
    // means: glDeleteTextures for an image the creator owns, or nothing at all for one owned
    // elsewhere (an Android SurfaceTexture, say). It runs on whichever thread removes that last
    // reference, so a callback that issues GL calls must marshal to the graphics thread.
    class Texture final
    {
    public:
        // Note: This API is experimental and its contract is subject to change in coming
        // versions. Descriptor will grow fields as the OpenGL backend catches up with the other
        // renderers -- mip level count and sample count are not described yet -- and methods
        // might change.

        // Whether the image may be attached to a framebuffer. OpenGL cannot be asked this about
        // an existing texture, so the creator states it; D3D and Metal read the equivalent from
        // the resource itself (D3D11_BIND_RENDER_TARGET, MTL::TextureUsageRenderTarget).
        enum class Usage
        {
            Sampled,
            RenderTarget,
        };

        // Describes the image a Texture stands for. Every property is named at the call site, so
        // a field can be added without silently changing what an existing call means:
        //
        //     GL::Texture::Descriptor{.Handle = name, .Width = w, .Height = h, .Format = GL_RGBA8}
        //
        // Handle is a GL texture name and Format a GL internal format (GLenum), both spelled as
        // unsigned int so this header stays independent of any GL headers.
        //
        // Layers is the number of array slices: 1 for a GL_TEXTURE_2D image, more for a
        // GL_TEXTURE_2D_ARRAY one. The target is derived from it rather than described, so the
        // caller never has to name a GL enum.
        struct Descriptor
        {
            unsigned int Handle{};
            uint32_t Width{};
            uint32_t Height{};
            uint32_t Layers{1};
            unsigned int Format{};
            Texture::Usage Usage{Texture::Usage::Sampled};
        };

        static Texture* Create(const Descriptor& descriptor, std::function<void(unsigned int)> release = {})
        {
            return new Texture{descriptor, std::move(release)};
        }

        void AddReference() noexcept
        {
            m_refCount.fetch_add(1, std::memory_order_relaxed);
        }

        // Removes this holder's reference, destroying the image once the last one is gone.
        void RemoveReference() noexcept
        {
            if (m_refCount.fetch_sub(1, std::memory_order_acq_rel) == 1)
            {
                if (m_release)
                {
                    m_release(m_descriptor.Handle);
                }
                delete this;
            }
        }

        unsigned int Handle() const noexcept { return m_descriptor.Handle; }
        uint32_t Width() const noexcept { return m_descriptor.Width; }
        uint32_t Height() const noexcept { return m_descriptor.Height; }
        uint32_t Layers() const noexcept { return m_descriptor.Layers; }
        unsigned int Format() const noexcept { return m_descriptor.Format; }
        bool IsRenderTarget() const noexcept { return m_descriptor.Usage == Usage::RenderTarget; }

        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

    private:
        Texture(const Descriptor& descriptor, std::function<void(unsigned int)> release)
            : m_descriptor{descriptor}
            , m_release{std::move(release)}
        {
        }

        ~Texture() = default;

        std::atomic<uint32_t> m_refCount{1};
        Descriptor m_descriptor{};
        std::function<void(unsigned int)> m_release{};
    };

    // Creates a Texture and takes over the reference Create returns.
    inline SharedPtr<Texture> MakeTexture(const Texture::Descriptor& descriptor,
        std::function<void(unsigned int)> release = {})
    {
        SharedPtr<Texture> ptr;
        ptr.Attach(Texture::Create(descriptor, std::move(release)));
        return ptr;
    }
}
