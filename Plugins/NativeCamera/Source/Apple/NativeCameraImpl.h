#pragma once

#include <bgfx/bgfx.h>
#include <napi/napi.h>
#include <NativeCamera.h>
#include <GraphicsImpl.h>

namespace Babylon::Plugins
{
    class Camera::Impl final : public std::enable_shared_from_this<Camera::Impl>
    {
    public:
        struct ImplData;
        
        Impl(Napi::Env env, bool overrideCameraTexture);
        ~Impl();
        void Open(uint32_t width, uint32_t height, bool frontCamera);
        void SetTextureOverride(void* texturePtr);
        void UpdateCameraTexture(bgfx::TextureHandle textureHandle);
        void Close();

    private:
        GraphicsImpl& m_graphicsImpl;

        std::shared_ptr<ImplData> m_implData;

        bool m_overrideCameraTexture{};
    };
}
