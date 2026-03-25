#pragma once

#include <napi/env.h>
#include <Babylon/Graphics/RendererType.h>
#include <memory>
#include <optional>

namespace Babylon::Plugins
{
    // All operations of this class must be called from the graphics thread unless otherwise noted.
    class ExternalTexture final
    {
    public:
        ExternalTexture(Graphics::TextureT, std::optional<Graphics::TextureFormatT> = {});
        ~ExternalTexture();

        // Copy semantics
        ExternalTexture(const ExternalTexture&);
        ExternalTexture& operator=(const ExternalTexture&);

        // Move semantics
        ExternalTexture(ExternalTexture&&) noexcept;
        ExternalTexture& operator=(ExternalTexture&&) noexcept;

        // Returns the width of the texture.
        uint32_t Width() const;

        // Returns the height of the texture.
        uint32_t Height() const;

        // Returns the underlying texture.
        Graphics::TextureT Get() const;

        // Creates a JavaScript value wrapping this external texture.
        // Wrap the returned value with `engine.wrapNativeTexture` on the JS side to get a Babylon.js `InternalTexture`.
        // This method must be called from the JavaScript thread. The caller must ensure no other thread
        // is concurrently calling any other operations on this object, including move operations.
        Napi::Value CreateForJavaScript(Napi::Env) const;

        // Updates to a new texture.
        void Update(Graphics::TextureT, std::optional<Graphics::TextureFormatT> = {});

    private:
        class Impl;
        class ImplBase;

        std::shared_ptr<Impl> m_impl{};
    };
}
