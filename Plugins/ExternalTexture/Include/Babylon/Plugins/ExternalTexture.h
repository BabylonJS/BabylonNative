#pragma once

#include <Babylon/JsRuntime.h>
#include <Babylon/Graphics/RendererType.h>
#include <memory>

namespace Babylon::Plugins
{
    class ExternalTexture final
    {
    public:
        // NOTE: Must call from the Graphics thread.
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
        // NOTE: Must call from the JavaScript thread.
        Napi::Promise AddToContextAsync(Napi::Env) const;

        void AddToContextAsync(Napi::Env, 
                               std::function<void(Napi::Env, Napi::Value)> succeed, 
                               std::function<void(Napi::Env, Napi::Value)> fail) const;
        // Updates to a new texture.
        // Texture attributes (width, height, format, etc.) must match.
        // NOTE: Must call from the Graphics thread.
        void Update(Graphics::TextureT);

    private:
        class Impl;
        class ImplBase;

        std::shared_ptr<Impl> m_impl{};
    };
}
