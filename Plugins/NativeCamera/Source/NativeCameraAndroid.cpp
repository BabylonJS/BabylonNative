#include <napi/napi.h>
#include "NativeCamera.h"
#include <string>
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
#include <android/native_window_jni.h>
#include <AndroidExtensions/JavaWrappers.h>
#include <AndroidExtensions/Globals.h>
#include <AndroidExtensions/OpenGLHelpers.h>
#include <AndroidExtensions/Permissions.h>
#include <android/log.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <arcana/threading/dispatcher.h>
#include <Babylon/JsRuntimeScheduler.h>
#include <GraphicsImpl.h>
#include <arcana/threading/task_schedulers.h>
#include <arcana/macros.h>
#include <memory>

using namespace android;
using namespace android::global;

namespace Babylon::Plugins::Internal
{
    struct CameraInterfaceAndroid : public CameraInterface {
        CameraInterfaceAndroid(Napi::Env env, uint32_t width, uint32_t height, bool frontCamera);

        virtual ~CameraInterfaceAndroid();

        void UpdateCameraTexture(bgfx::TextureHandle textureHandle) override;

        static constexpr char CAMERA_VERT_SHADER[]{R"(#version 300 es
            precision highp float;
            out vec2 cameraFrameUV;
            void main() {
                cameraFrameUV = vec2(gl_VertexID&1, (gl_VertexID &2)>>1) * 2.f;
                gl_Position = vec4(cameraFrameUV * 2.f - 1.f, 0.0, 1.0);
            }
        )"};

        static constexpr char CAMERA_FRAG_SHADER[]{R"(#version 300 es
            #extension GL_OES_EGL_image_external_essl3 : require
            precision mediump float;
            in vec2 cameraFrameUV;
            uniform samplerExternalOES cameraTexture;
            layout(location = 0) out vec4 oFragColor;
            void main() {
                oFragColor = texture(cameraTexture, cameraFrameUV);
            }
        )"};

    private:

        std::string getCamId(bool frontCamera);

        GraphicsImpl &m_graphicsImpl;
        JsRuntimeScheduler m_runtimeScheduler;

        uint32_t width{};
        uint32_t height{};

#if __ANDROID_API__ >= 24
        ACameraManager* cameraManager{};
        ACameraDevice* cameraDevice{};
        ACameraOutputTarget* textureTarget{};
        ACaptureRequest* request{};
        ANativeWindow* textureWindow{};
        ACameraCaptureSession* textureSession{};
        ACaptureSessionOutput* textureOutput{};
        ACaptureSessionOutput* output{};
        ACaptureSessionOutputContainer* outputs{};
        android::graphics::SurfaceTexture surfaceTexture;
        android::view::Surface surface;
#endif
        GLuint cameraOESTextureId{};
        GLuint cameraRGBATextureId{};
        GLuint cameraShaderProgramId{};
        GLuint frameBufferId{};

        EGLContext context{};
        EGLDisplay display{};
    };

#if __ANDROID_API__ >= 24
    std::string CameraInterfaceAndroid::getCamId(bool frontCamera)
    {
        ACameraIdList *cameraIds = nullptr;
        ACameraManager_getCameraIdList(cameraManager, &cameraIds);

        std::string cameraId{};

        for (int i = 0; i < cameraIds->numCameras; ++i)
        {
            const char *id = cameraIds->cameraIds[i];

            ACameraMetadata *metadataObj;
            ACameraManager_getCameraCharacteristics(cameraManager, id, &metadataObj);

            ACameraMetadata_const_entry lensInfo = {};
            ACameraMetadata_getConstEntry(metadataObj, ACAMERA_LENS_FACING, &lensInfo);

            auto facing = static_cast<acamera_metadata_enum_android_lens_facing_t>(lensInfo.data.u8[0]);

            // Found a corresponding facing camera?
            if (facing == (frontCamera ? ACAMERA_LENS_FACING_FRONT : ACAMERA_LENS_FACING_BACK))
            {
                cameraId = id;
                break;
            }
        }

        ACameraManager_deleteCameraIdList(cameraIds);
        return cameraId;
    }

    // device callbacks
    static void onDisconnected(void* /*context*/, ACameraDevice* /*device*/)
    {
    }

    static void onError(void* /*context*/, ACameraDevice* /*device*/, int /*error*/)
    {
    }

    static ACameraDevice_stateCallbacks cameraDeviceCallbacks = {
            .context = nullptr,
            .onDisconnected = onDisconnected,
            .onError = onError
    };

    // session callbacks
    static void onSessionActive(void* /*context*/, ACameraCaptureSession* /*session*/)
    {
    }

    static void onSessionReady(void* /*context*/, ACameraCaptureSession* /*session*/)
    {
    }

    static void onSessionClosed(void* /*context*/, ACameraCaptureSession* /*session*/)
    {
    }

    static ACameraCaptureSession_stateCallbacks sessionStateCallbacks {
        .context = nullptr,
        .onClosed = onSessionClosed,
        .onReady = onSessionReady,
        .onActive = onSessionActive
    };

    // capture callbacks
    static void onCaptureFailed(void* /*context*/, ACameraCaptureSession* /*session*/, ACaptureRequest* /*request*/, ACameraCaptureFailure* /*failure*/)
    {
    }

    static void onCaptureSequenceCompleted(void* /*context*/, ACameraCaptureSession* /*session*/, int /*sequenceId*/, int64_t /*frameNumber*/)
    {
    }

    static void onCaptureSequenceAborted(void* /*context*/, ACameraCaptureSession* /*session*/, int /*sequenceId*/)
    {
    }

    static void onCaptureCompleted (void* /*context*/, ACameraCaptureSession* /*session*/, ACaptureRequest* /*request*/, const ACameraMetadata* /*result*/)
    {
    }

    static ACameraCaptureSession_captureCallbacks captureCallbacks {
        .context = nullptr,
        .onCaptureStarted = nullptr,
        .onCaptureProgressed = nullptr,
        .onCaptureCompleted = onCaptureCompleted,
        .onCaptureFailed = onCaptureFailed,
        .onCaptureSequenceCompleted = onCaptureSequenceCompleted,
        .onCaptureSequenceAborted = onCaptureSequenceAborted,
        .onCaptureBufferLost = nullptr,
    };

#endif

    static GLuint GenerateOESTexture()
    {
        GLuint oesTexture;
        glGenTextures(1, &oesTexture);
        glBindTexture(GL_TEXTURE_EXTERNAL_OES, oesTexture);
        glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_EXTERNAL_OES, 0);
        return oesTexture;
    }
    
    CameraInterfaceAndroid::CameraInterfaceAndroid(Napi::Env env, uint32_t width, uint32_t height, bool frontCamera)
        : m_graphicsImpl{GraphicsImpl::GetFromJavaScript(env)}
        , m_runtimeScheduler{JsRuntime::GetFromJavaScript(env)}
        , width{width}
        , height{height}
    {
        android::Permissions::CheckCameraPermissionAsync().then(arcana::inline_scheduler, arcana::cancellation::none(), [this, frontCamera]()
        {
            // Check if there is an already available context for this thread
            EGLContext currentContext = eglGetCurrentContext();
            if (currentContext == EGL_NO_CONTEXT)
            {
                // create a shared context with bgfx so JNI thread (by surfaceTexture) can update the texture
                display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
                eglInitialize(display, nullptr, nullptr);

                static const EGLint attrs[] ={
                    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT_KHR,
                    EGL_BLUE_SIZE, 8,
                    EGL_GREEN_SIZE, 8,
                    EGL_RED_SIZE, 8,
                    EGL_ALPHA_SIZE, 8,
                    EGL_DEPTH_SIZE, 16,
                    EGL_STENCIL_SIZE, 8,
                    EGL_NONE
                };

                EGLConfig  config;
                EGLint numConfig = 0;
                eglChooseConfig(display, attrs, &config, 1, &numConfig);

                static const EGLint contextAttribs[] = {
                    EGL_CONTEXT_MAJOR_VERSION_KHR,
                    3,
                    EGL_CONTEXT_MINOR_VERSION_KHR,
                    0,
                    EGL_NONE};

                context = eglCreateContext(display, config, bgfx::getInternalData()->context, contextAttribs);
                if (eglMakeCurrent(display, 0/*surface*/, 0/*surface*/, context) == EGL_FALSE)
                {
                    throw std::runtime_error{"Unable to create a shared GL context for camera texture."};
                }
            }

            cameraOESTextureId = GenerateOESTexture();
            glGenTextures(1, &cameraRGBATextureId);
            glBindTexture(GL_TEXTURE_2D, cameraRGBATextureId);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->width, this->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glGenerateMipmap(GL_TEXTURE_2D);

            glBindTexture(GL_TEXTURE_2D, 0);

            glGenFramebuffers(1, &frameBufferId);
            glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, cameraRGBATextureId, 0);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            cameraShaderProgramId = android::OpenGLHelpers::CreateShaderProgram(CAMERA_VERT_SHADER, CAMERA_FRAG_SHADER);

#if __ANDROID_API__ >= 24
            // Create the surface and surface texture that will receive the camera preview
            surfaceTexture.initWithTexture(cameraOESTextureId);
            surface.initWithSurfaceTexture(surfaceTexture);

            // open the front or back camera
            cameraManager = ACameraManager_create();
            auto id = getCamId(frontCamera);
            ACameraManager_openCamera(cameraManager, id.c_str(), &cameraDeviceCallbacks, &cameraDevice);

            textureWindow = surface.getNativeWindow();

            // Prepare request for texture target
            ACameraDevice_createCaptureRequest(cameraDevice, TEMPLATE_PREVIEW, &request);

            // Prepare outputs for session
            ACaptureSessionOutput_create(textureWindow, &textureOutput);
            ACaptureSessionOutputContainer_create(&outputs);
            ACaptureSessionOutputContainer_add(outputs, textureOutput);

            // Prepare target surface
            ANativeWindow_acquire(textureWindow);
            ACameraOutputTarget_create(textureWindow, &textureTarget);
            ACaptureRequest_addTarget(request, textureTarget);

            // Create the session
            ACameraDevice_createCaptureSession(cameraDevice, outputs, &sessionStateCallbacks, &textureSession);

            // Start capturing continuously
            ACameraCaptureSession_setRepeatingRequest(textureSession, &captureCallbacks, 1, &request, nullptr);
#else

            UNUSED(frontCamera);
#pragma message("Warning: Android Platform level < 24. No HW Camera support. Only camera texture override is available.")

#endif
            if (eglMakeCurrent(display, 0/*surface*/, 0/*surface*/, currentContext) == EGL_FALSE)
            {
                throw std::runtime_error{"Unable to restore GL context for camera texture init."};
            }
        });
    }

    CameraInterfaceAndroid::~CameraInterfaceAndroid()
    {
#if __ANDROID_API__ >= 24
        // Stop recording to SurfaceTexture and do some cleanup
        ACameraCaptureSession_stopRepeating(textureSession);
        ACameraCaptureSession_close(textureSession);
        ACaptureSessionOutputContainer_free(outputs);
        ACaptureSessionOutput_free(output);

        ACameraDevice_close(cameraDevice);
        ACameraManager_delete(cameraManager);

        // Capture request for SurfaceTexture
        ANativeWindow_release(textureWindow);
        ACaptureRequest_free(request);
#endif
        if (context)
        {
            eglDestroyContext(display, context);
        }
    }

    void CameraInterfaceAndroid::UpdateCameraTexture(bgfx::TextureHandle textureHandle)
    {
        EGLContext currentContext = eglGetCurrentContext();
        if (context != EGL_NO_CONTEXT)
        {
            // use the newly created shared context
            if (eglMakeCurrent(display, 0/*surface*/, 0/*surface*/, context) == EGL_FALSE)
            {
                throw std::runtime_error{"Unable to make current shared GL context for camera texture."};
            }
        }

#if __ANDROID_API__ >= 24
        surfaceTexture.updateTexture();
#endif
        glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
        glViewport(0, 0, width, height);
        glUseProgram(cameraShaderProgramId);

        // Configure the camera texture
        auto cameraTextureUniformLocation{glGetUniformLocation(cameraShaderProgramId, "cameraTexture")};
        glUniform1i(cameraTextureUniformLocation, android::OpenGLHelpers::GetTextureUnit(GL_TEXTURE0));
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_EXTERNAL_OES, cameraOESTextureId);
        auto bindSamplerTransaction{ android::OpenGLHelpers::GLTransactions::BindSampler(GL_TEXTURE0, 0) };

        // Draw the quad
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);

        glUseProgram(0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // bind previously bound context
        if (eglMakeCurrent(display, 0/*surface*/, 0/*surface*/, currentContext) == EGL_FALSE)
        {
            throw std::runtime_error{"Unable to make current shared GL context for camera texture."};
        }

        arcana::make_task(m_graphicsImpl.BeforeRenderScheduler(), arcana::cancellation::none(), [this, textureHandle] {
            bgfx::overrideInternal(textureHandle, cameraRGBATextureId);
        });
    }

    std::unique_ptr<CameraInterface> CameraInterface::CreateInterface(Napi::Env env, uint32_t width, uint32_t height, bool frontCamera)
    {
        return std::unique_ptr<CameraInterface>(new CameraInterfaceAndroid(env, width, height, frontCamera));
    }
}