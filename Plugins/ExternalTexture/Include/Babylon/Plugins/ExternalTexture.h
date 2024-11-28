#pragma once

#include <Babylon/JsRuntime.h>
#include <Babylon/Graphics/RendererType.h>
#include <memory>
#include <optional>

namespace Babylon::Plugins
{
    class ExternalTexture final
    {
    public:
        // NOTE: Must call from the Graphics thread.
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

        // Adds this texture to the graphics context of the given N-API environment.
        // NOTE: Must call from the JavaScript thread.
        Napi::Promise AddToContextAsync(Napi::Env) const;

        // Updates to a new texture.
        // Texture attributes (width, height, and mip levels) must match.
        // Returns true if update succeeds or false otherwise.
        // NOTE: Must call from the Graphics thread.
        bool Update(Graphics::TextureT, std::optional<Graphics::TextureFormatT> = {});

    private:
        class Impl;
        class ImplBase;

        std::shared_ptr<Impl> m_impl{};
    };
}
