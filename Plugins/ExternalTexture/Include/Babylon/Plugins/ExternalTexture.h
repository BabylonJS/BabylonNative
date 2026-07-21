#pragma once

#include <napi/env.h>
#include <Babylon/Graphics/RendererType.h>
#include <memory>
#include <optional>

namespace Babylon::Plugins
{
    // All operations of this class must be called from the graphics thread unless otherwise noted.
    //
    // Ownership: on the OpenGL/OpenGL ES backend, ExternalTexture does NOT take ownership of the
    // texture handle -- OpenGL has no way to reference-count a texture name -- so the caller must
    // keep the source texture alive for as long as this ExternalTexture (and any JavaScript texture
    // created from it) is in use. Deleting the handle earlier leaves a dangling GL name that bgfx
    // later dereferences (e.g. glObjectLabel under a debug build), which is a fatal error on strict
    // drivers. The D3D11/D3D12/Metal backends retain the native texture, so on those backends the
    // caller may release its own reference immediately after construction.
    class ExternalTexture final
    {
    public:
        // width/height are required on the OpenGL/OpenGL ES backend and ignored on all
        // others. BabylonNative's OpenGL renderer runs on an OpenGL ES 3.0 context, which
        // provides no way to query a texture's dimensions from a bare handle
        // (glGetTexLevelParameteriv was only added in ES 3.1). The caller that created the
        // texture already knows its size, so it must supply it here. The D3D/Metal backends
        // introspect the native texture and ignore these hints.
        ExternalTexture(Graphics::TextureT, std::optional<Graphics::TextureFormatT> = {}, std::optional<uint32_t> width = {}, std::optional<uint32_t> height = {});
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
        // If layerIndex is set, the JavaScript texture views only that array layer (single-slice); otherwise the entire texture.
        // This method must be called from the JavaScript thread. The caller must ensure no other thread
        // is concurrently calling any other operations on this object, including move operations.
        Napi::Value CreateForJavaScript(Napi::Env, std::optional<uint16_t> layerIndex = {}) const;

        // Deprecated: use CreateForJavaScript instead. Retained as a shim for existing consumers.
        // Returns a Promise that is already resolved with the value from CreateForJavaScript.
        // If layerIndex is set, the JavaScript texture views only that array layer (single-slice).
        [[deprecated("Use CreateForJavaScript instead.")]]
        Napi::Promise AddToContextAsync(Napi::Env, std::optional<uint16_t> layerIndex = {}) const;

        // Updates to a new texture. If layerIndex is set, views only that array layer (single-slice).
        // width/height follow the same backend-specific contract as the constructor: required on
        // OpenGL/OpenGL ES, ignored elsewhere.
        void Update(Graphics::TextureT, std::optional<Graphics::TextureFormatT> = {}, std::optional<uint16_t> layerIndex = {}, std::optional<uint32_t> width = {}, std::optional<uint32_t> height = {});

    private:
        class Impl;
        class ImplBase;

        std::shared_ptr<Impl> m_impl{};
    };
}
