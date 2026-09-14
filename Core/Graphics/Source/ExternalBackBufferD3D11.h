#pragma once

#include <Babylon/Graphics/BgfxCallback.h>
#include <Babylon/Graphics/RendererType.h>

#include <bgfx/bgfx.h>

#include <functional>
#include <memory>

namespace Babylon::Graphics
{
    class ExternalBackBufferD3D11 final
    {
    public:
        ExternalBackBufferD3D11(BackBufferColorT color, BackBufferDepthStencilT depth, const bgfx::SwapChain& descriptor);
        ~ExternalBackBufferD3D11();

        ExternalBackBufferD3D11(const ExternalBackBufferD3D11&) = delete;
        ExternalBackBufferD3D11& operator=(const ExternalBackBufferD3D11&) = delete;
        ExternalBackBufferD3D11(ExternalBackBufferD3D11&&) = delete;
        ExternalBackBufferD3D11& operator=(ExternalBackBufferD3D11&&) = delete;

        bgfx::FrameBufferHandle GetFrameBuffer() const;
        bgfx::TextureHandle GetDepthTexture() const;
        void ReadPixels(const std::function<void(const BgfxCallback::CaptureData&)>& callback) const;

    private:
        class Impl;
        std::unique_ptr<Impl> m_impl;
    };
}
