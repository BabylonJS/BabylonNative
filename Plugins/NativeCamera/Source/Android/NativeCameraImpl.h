#pragma once

#include "CameraWrappers.h"
#include <media/NdkImageReader.h>
#include <bgfx/bgfx.h>
#include <napi/napi.h>
#include <NativeCamera.h>
#include <Babylon/Graphics/DeviceContext.h>
#include <AndroidExtensions/OpenGLHelpers.h>
#include <AndroidExtensions/JavaWrappers.h>
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

        Impl(Napi::Env env, bool overrideCameraTexture);
        ~Impl();
        arcana::task<CameraDimensions, std::exception_ptr> Open(uint32_t maxWidth, uint32_t maxHeight, bool frontCamera);
        void SetTextureOverride(void* texturePtr);
        void UpdateCameraTexture(bgfx::TextureHandle textureHandle);
        void Close();

    private:

        Graphics::DeviceContext* m_deviceContext;
        Napi::Env m_env;

        bool m_overrideCameraTexture;

        CameraDimensions m_cameraDimensions{};

        GLuint GenerateOESTexture();
        std::string GetCameraId(bool frontCamera);

        API24::ACameraManager* m_cameraManager{};
        API24::ACameraDevice* m_cameraDevice{};
        API24::ACameraOutputTarget* m_textureTarget{};
        API24::ACaptureRequest* m_request{};
        API24::ANativeWindow* m_textureWindow{};
        API24::ACameraCaptureSession* m_textureSession{};
        API24::ACaptureSessionOutput* m_textureOutput{};
        API24::ACaptureSessionOutput* m_output{};
        API24::ACaptureSessionOutputContainer* m_outputs{};
        android::graphics::SurfaceTexture m_surfaceTexture{};

        GLuint m_cameraOESTextureId{};
        GLuint m_cameraRGBATextureId{};
        GLuint m_cameraShaderProgramId{};
        GLuint m_frameBufferId{};

        EGLContext m_context{EGL_NO_CONTEXT};
        EGLDisplay m_display{};
    };
}