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

        // Returns the underlying texture.
        Graphics::TextureT Get() const;

        // Adds this texture to the graphics context of the given N-API environment.
        // NOTE: Must call from the JavaScript thread.
        Napi::Promise AddToContextAsync(Napi::Env, std::optional<uint16_t> layerIndex = {}) const;

        // Updates to a new texture.
        // NOTE: Must call from the Graphics thread.
        void Update(Graphics::TextureT, std::optional<Graphics::TextureFormatT> = {}, std::optional<uint16_t> layerIndex = {});

    private:
        class Impl;
        class ImplBase;

        std::shared_ptr<Impl> m_impl{};
    };
}
