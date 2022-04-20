#pragma once

#include <Babylon/JsRuntime.h>
#include <Babylon/Graphics/RendererType.h>
#include <memory>

namespace Babylon::Plugins
{
    class ExternalTexture final
    {
    public:
        ExternalTexture(Graphics::TextureT);
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
        // NOTE: Must call this from the JavaScript thread.
        Napi::Promise AddToContext(Napi::Env&) const;

    private:
        class Impl;
        std::shared_ptr<Impl> m_impl{};
    };
}
