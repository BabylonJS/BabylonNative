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
        // Whether the image may be attached to a framebuffer. OpenGL cannot be asked this about
        // an existing texture, so the creator states it; D3D and Metal read the equivalent from
        // the resource itself (D3D11_BIND_RENDER_TARGET, MTL::TextureUsageRenderTarget).
        enum class Usage
        {
            Sampled,
            RenderTarget,
        };

        // handle is a GL texture name and format a GL internal format (GLenum), spelled as
        // unsigned int so this header stays independent of any GL headers.
        static Texture* Create(unsigned int handle, uint32_t width, uint32_t height,
            unsigned int format, Usage usage, std::function<void(unsigned int)> release = {})
        {
            return new Texture{handle, width, height, format, usage, std::move(release)};
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
                    m_release(m_handle);
                }
                delete this;
            }
        }

        unsigned int Handle() const noexcept { return m_handle; }
        uint32_t Width() const noexcept { return m_width; }
        uint32_t Height() const noexcept { return m_height; }
        unsigned int Format() const noexcept { return m_format; }
        bool IsRenderTarget() const noexcept { return m_usage == Usage::RenderTarget; }

        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

    private:
        Texture(unsigned int handle, uint32_t width, uint32_t height, unsigned int format,
            Usage usage, std::function<void(unsigned int)> release)
            : m_handle{handle}
            , m_width{width}
            , m_height{height}
            , m_format{format}
            , m_usage{usage}
            , m_release{std::move(release)}
        {
        }

        ~Texture() = default;

        std::atomic<uint32_t> m_refCount{1};
        unsigned int m_handle{};
        uint32_t m_width{};
        uint32_t m_height{};
        unsigned int m_format{};
        Usage m_usage{Usage::Sampled};
        std::function<void(unsigned int)> m_release{};
    };

    // Creates a Texture and takes over the reference Create returns.
    inline SharedPtr<Texture> MakeTexture(unsigned int handle, uint32_t width, uint32_t height,
        unsigned int format, Texture::Usage usage, std::function<void(unsigned int)> release = {})
    {
        SharedPtr<Texture> ptr;
        ptr.Attach(Texture::Create(handle, width, height, format, usage, std::move(release)));
        return ptr;
    }
}
