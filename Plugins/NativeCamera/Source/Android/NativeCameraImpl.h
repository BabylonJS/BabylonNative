#pragma once

// The camera API's in the NDK are only supported in API24 and higher. We
// use dlopen and dlsym to link the camera lib at runtime only if the current
// device supports it. In order to avoid redefining the type information the
// below macro's forcefully import the camera header files as if the min supported
// SDK was set to 24.
#define __ORIG_ANDROID_API__ __ANDROID_API__
#undef __ANDROID_API__
#define __ANDROID_API__ 24
namespace API24 {
    #include <camera/NdkCameraManager.h>
    #include <camera/NdkCameraCaptureSession.h>
    #include <camera/NdkCameraDevice.h>
    #include <camera/NdkCameraError.h>
    #include <camera/NdkCameraManager.h>
    #include <camera/NdkCameraMetadata.h>
    #include <camera/NdkCameraMetadataTags.h>
    #include <camera/NdkCameraWindowType.h>
    #include <camera/NdkCaptureRequest.h>
}
#undef __ANDROID_API__
#define __ANDROID_API__ __ORIG_ANDROID_API__

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
        Impl(Napi::Env env, bool overrideCameraTexture);
        ~Impl();
        arcana::task<void, std::exception_ptr> Open(uint32_t width, uint32_t height, bool frontCamera);
        void SetTextureOverride(void* texturePtr);
        void UpdateCameraTexture(bgfx::TextureHandle textureHandle);
        void Close();

    private:

        Graphics::DeviceContext* m_deviceContext;
        Napi::Env m_env;

        bool m_overrideCameraTexture;

        uint32_t m_width{};
        uint32_t m_height{};

        GLuint GenerateOESTexture();
        std::string GetCameraId(bool frontCamera);
        void* GetCameraDynamicFunction(const char* functionName);

        std::map<std::string, void*> m_cameraDynamicFunctions{};
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