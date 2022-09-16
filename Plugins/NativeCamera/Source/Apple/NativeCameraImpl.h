#pragma once

#include <bgfx/bgfx.h>
#include <napi/napi.h>
#include <NativeCamera.h>
#include <Babylon/Graphics/DeviceContext.h>
#include <arcana/threading/task.h>

namespace Babylon::Plugins
{
    class Camera::Impl final : public std::enable_shared_from_this<Camera::Impl>
    {
    public:
        struct CameraDimensions {
            uint32_t width;
            uint32_t height;
        };

        struct ImplData;
        
        Impl(Napi::Env env, bool overrideCameraTexture);
        ~Impl();
        arcana::task<CameraDimensions, std::exception_ptr> Open(uint32_t maxWidth, uint32_t maxHeight, bool frontCamera);
        void SetTextureOverride(void* texturePtr);
        void UpdateCameraTexture(bgfx::TextureHandle textureHandle);
        void Close();

    private:
        Graphics::DeviceContext* m_deviceContext;
        Napi::Env m_env;

        std::unique_ptr<ImplData> m_implData;

        bool m_overrideCameraTexture{};

        CameraDimensions m_cameraDimensions{};

        arcana::background_dispatcher<32> m_cameraDispatcher{};
    };
}
