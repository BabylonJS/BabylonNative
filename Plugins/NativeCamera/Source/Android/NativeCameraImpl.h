#pragma once

#include <camera/NdkCameraManager.h>
#include <camera/NdkCameraCaptureSession.h>
#include <camera/NdkCameraDevice.h>
#include <camera/NdkCameraError.h>
#include <camera/NdkCameraManager.h>
#include <camera/NdkCameraMetadata.h>
#include <camera/NdkCameraMetadataTags.h>
#include <camera/NdkCameraWindowType.h>
#include <camera/NdkCaptureRequest.h>
#include <media/NdkImageReader.h>
#include <bgfx/bgfx.h>
#include <napi/napi.h>
#include <NativeCamera.h>
#include <Babylon/Graphics/DeviceContext.h>
#include <AndroidExtensions/OpenGLHelpers.h>
#include <AndroidExtensions/JavaWrappers.h>

namespace Babylon::Plugins
{
    class Camera::Impl final : public std::enable_shared_from_this<Camera::Impl>
    {
    public:
        Impl(Napi::Env env, bool overrideCameraTexture);
        ~Impl();
        void Open(uint32_t width, uint32_t height, bool frontCamera);
        void SetTextureOverride(void* texturePtr);
        void UpdateCameraTexture(bgfx::TextureHandle textureHandle);
        void Close();

    private:

        Graphics::DeviceContext &m_deviceContext;

        bool m_overrideCameraTexture;

        uint32_t m_width{};
        uint32_t m_height{};

#if __ANDROID_API__ >= 24
        GLuint GenerateOESTexture();
        std::string GetCameraId(bool frontCamera);

        ACameraManager* m_cameraManager{};
        ACameraDevice* m_cameraDevice{};
        ACameraOutputTarget* m_textureTarget{};
        ACaptureRequest* m_request{};
        ANativeWindow* m_textureWindow{};
        ACameraCaptureSession* m_textureSession{};
        ACaptureSessionOutput* m_textureOutput{};
        ACaptureSessionOutput* m_output{};
        ACaptureSessionOutputContainer* m_outputs{};
        android::graphics::SurfaceTexture m_surfaceTexture;
#endif
        GLuint m_cameraOESTextureId{};
        GLuint m_cameraRGBATextureId{};
        GLuint m_cameraShaderProgramId{};
        GLuint m_frameBufferId{};

        EGLContext m_context{EGL_NO_CONTEXT};
        EGLDisplay m_display{};
    };
}