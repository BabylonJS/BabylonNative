#include <napi/napi.h>
#include "NativeCamera.h"
#include "../NativeCameraImpl.h"
#include <string>
#include <android/native_window_jni.h>
#include <AndroidExtensions/Globals.h>
#include <AndroidExtensions/Permissions.h>
#include <AndroidExtensions/OpenGLHelpers.h>
#include <AndroidExtensions/JavaWrappers.h>
#include <media/NdkImageReader.h>
#include <android/log.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <arcana/threading/dispatcher.h>
#include <Babylon/JsRuntimeScheduler.h>
#include <Babylon/Graphics/DeviceContext.h>
#include <arcana/threading/task_schedulers.h>
#include <arcana/macros.h>
#include <memory>
#include "CameraWrappers.h"

using namespace android;
using namespace android::global;

namespace Babylon::Plugins
{
    struct CameraTrack::ImplData{};

    CameraTrack::~CameraTrack() = default;

    struct CameraDevice::ImplData {
        std::string cameraID;
        int32_t sensorRotation;
        bool facingUser;
    };

    CameraDevice::~CameraDevice() = default;

    struct Camera::Impl::ImplData {
        Graphics::DeviceContext* deviceContext;
        Napi::Env env;

        bool overrideCameraTexture{};

        CameraDimensions cameraDimensions{};

        API24::ACameraManager* cameraManager{};
        API24::ACameraDevice* cameraDevice{};
        API24::ACameraOutputTarget* textureTarget{};
        API24::ACaptureRequest* request{};
        ANativeWindow* textureWindow{};
        API24::ACameraCaptureSession* textureSession{};
        API24::ACaptureSessionOutput* textureOutput{};
        API24::ACaptureSessionOutput* output{};
        API24::ACaptureSessionOutputContainer* outputs{};
        android::graphics::SurfaceTexture surfaceTexture{};

        GLuint cameraOESTextureId{};
        GLuint cameraRGBATextureId{};
        GLuint cameraShaderProgramId{};
        GLuint frameBufferId{};
        const GLfloat* cameraUVs{};

        EGLContext context{EGL_NO_CONTEXT};
        EGLDisplay display{};

        ImplData(Napi::Env env, bool overrideCameraTexture)
            : deviceContext{nullptr}
            , env{env}
            , overrideCameraTexture{overrideCameraTexture} {};

        GLuint GenerateOESTexture() {
            GLuint oesTexture;
            glGenTextures(1, &oesTexture);
            glBindTexture(GL_TEXTURE_EXTERNAL_OES, oesTexture);
            glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_EXTERNAL_OES, 0);
            return oesTexture;
        }
    };

    // Vertex positions for the camera texture
    constexpr size_t CAMERA_VERTEX_COUNT{ 4 };
    constexpr GLfloat CAMERA_VERTEX_POSITIONS[CAMERA_VERTEX_COUNT * 2]{ -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f };

    // UV mappings to correct for the different orientations of the screen versus the camera sensor
    constexpr size_t CAMERA_UVS_COUNT{ 4 };
    constexpr GLfloat CAMERA_UVS_ROTATION_0[CAMERA_UVS_COUNT  * 2]{ 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f };
    constexpr GLfloat CAMERA_UVS_ROTATION_90[CAMERA_UVS_COUNT  * 2]{ 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f };
    constexpr GLfloat CAMERA_UVS_ROTATION_180[CAMERA_UVS_COUNT  * 2]{ 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f };
    constexpr GLfloat CAMERA_UVS_ROTATION_270[CAMERA_UVS_COUNT  * 2]{ 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f };

    static constexpr char CAMERA_VERT_SHADER[]{R"(#version 300 es
        precision highp float;
        uniform vec2 positions[4];
        uniform vec2 uvs[4];
        out vec2 uv;
        void main() {
            gl_Position = vec4(positions[gl_VertexID], 0.0, 1.0);
            uv = uvs[gl_VertexID];
        }
    )"};

    static constexpr char CAMERA_FRAG_SHADER[]{R"(#version 300 es
        #extension GL_OES_EGL_image_external_essl3 : require
        precision mediump float;
        in vec2 uv;
        uniform samplerExternalOES cameraTexture;
        // Location 0 is GL_COLOR_ATTACHMENT0, which in turn is the babylonTexture
        layout(location = 0) out vec4 oFragColor;
        void main() {
            oFragColor = texture(cameraTexture, uv);
        }
    )"};

    // device callbacks
    static void onDisconnected(void* /*context*/, API24::ACameraDevice* /*device*/)
    {
    }

    static void onError(void* /*context*/, API24::ACameraDevice* /*device*/, int /*error*/)
    {
    }

    static API24::ACameraDevice_stateCallbacks cameraDeviceCallbacks = {
            .context = nullptr,
            .onDisconnected = onDisconnected,
            .onError = onError
    };

    // session callbacks
    static void onSessionActive(void* /*context*/, API24::ACameraCaptureSession* /*session*/)
    {
    }

    static void onSessionReady(void* /*context*/, API24::ACameraCaptureSession* /*session*/)
    {
    }

    static void onSessionClosed(void* /*context*/, API24::ACameraCaptureSession* /*session*/)
    {
    }

    static API24::ACameraCaptureSession_stateCallbacks sessionStateCallbacks {
        .context = nullptr,
        .onClosed = onSessionClosed,
        .onReady = onSessionReady,
        .onActive = onSessionActive
    };

    // capture callbacks
    static void onCaptureFailed(void* /*context*/, API24::ACameraCaptureSession* /*session*/, API24::ACaptureRequest* /*request*/, API24::ACameraCaptureFailure* /*failure*/)
    {
    }

    static void onCaptureSequenceCompleted(void* /*context*/, API24::ACameraCaptureSession* /*session*/, int /*sequenceId*/, int64_t /*frameNumber*/)
    {
    }

    static void onCaptureSequenceAborted(void* /*context*/, API24::ACameraCaptureSession* /*session*/, int /*sequenceId*/)
    {
    }

    static void onCaptureCompleted (void* /*context*/, API24::ACameraCaptureSession* /*session*/, API24::ACaptureRequest* /*request*/, const API24::ACameraMetadata* /*result*/)
    {
    }

    static API24::ACameraCaptureSession_captureCallbacks captureCallbacks {
        .context = nullptr,
        .onCaptureStarted = nullptr,
        .onCaptureProgressed = nullptr,
        .onCaptureCompleted = onCaptureCompleted,
        .onCaptureFailed = onCaptureFailed,
        .onCaptureSequenceCompleted = onCaptureSequenceCompleted,
        .onCaptureSequenceAborted = onCaptureSequenceAborted,
        .onCaptureBufferLost = nullptr,
    };

    Camera::Impl::Impl(Napi::Env env, bool overrideCameraTexture)
        : m_implData{std::make_unique<ImplData>(env, overrideCameraTexture)}
    {
        if (API_LEVEL < 24 && !overrideCameraTexture)
        {
            throw std::runtime_error{"Android Platform level < 24. Only camera texture override is available."};
        }

        if (!overrideCameraTexture)
        {
            m_implData->cameraManager = GET_CAMERA_FUNCTION(ACameraManager_create)();
        }
    }

    Camera::Impl::~Impl()
    {
        GET_CAMERA_FUNCTION(ACameraManager_delete)(m_implData->cameraManager);
    }

    arcana::task<Camera::Impl::CameraDimensions, std::exception_ptr> Camera::Impl::Open(
            std::shared_ptr<CameraDevice> cameraDevice, std::shared_ptr<CameraTrack> track)
    {
        if (!m_implData->deviceContext){
            m_implData->deviceContext = &Graphics::DeviceContext::GetFromJavaScript(m_implData->env);
        }

        return android::Permissions::CheckCameraPermissionAsync().then(arcana::inline_scheduler, arcana::cancellation::none(), [this, cameraDevice, track]()
        {
            // Get the phone's current rotation so we can determine if the camera image needs to be rotated based on the sensor's natural orientation
            int phoneRotation{ GetAppContext().getSystemService<android::view::WindowManager>().getDefaultDisplay().getRotation() * 90 };

            // The sensor rotation dictates the orientation of the camera when the phone is in it's default orientation
            // Subtracting the phone's rotation from the camera's rotation will give us the current orientation
            // of the sensor. Then add 360 and modulus 360 to ensure we're always talking about positive degrees.
            int sensorRotationDiff{(cameraDevice->implData->sensorRotation - phoneRotation + 360) % 360};
            bool sensorIsPortrait{sensorRotationDiff == 90 || sensorRotationDiff == 270};
            if (cameraDevice->implData->facingUser && !sensorIsPortrait && !m_implData->overrideCameraTexture)
            {
                // Compensate for the front facing camera being naturally mirrored. In the portrait orientation
                // the mirrored behavior matches the browser, but in landscape it would result in the image rendering
                // upside down. Rotate the image by 180 to compensate.
                sensorRotationDiff = (sensorRotationDiff + 180) % 360;
            }

            // To match the web implementation if the sensor is rotated into a portrait orientation then the width and height
            // of the video should be swapped
            m_implData->cameraDimensions.width = !sensorIsPortrait ? track->width : track->height;
            m_implData->cameraDimensions.height = !sensorIsPortrait ? track->height : track->width;

            // Check if there is an already available context for this thread
            EGLContext currentContext = eglGetCurrentContext();
            if (currentContext == EGL_NO_CONTEXT)
            {
                // create a shared context with bgfx so JNI thread (by surfaceTexture) can update the texture
                m_implData->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
                eglInitialize(m_implData->display, nullptr, nullptr);

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
                eglChooseConfig(m_implData->display, attrs, &config, 1, &numConfig);

                static const EGLint contextAttribs[] = {
                    EGL_CONTEXT_MAJOR_VERSION_KHR,
                    3,
                    EGL_CONTEXT_MINOR_VERSION_KHR,
                    0,
                    EGL_NONE};

                m_implData->context = eglCreateContext(m_implData->display, config, bgfx::getInternalData()->context, contextAttribs);
                if (eglMakeCurrent(m_implData->display, 0/*surface*/, 0/*surface*/, m_implData->context) == EGL_FALSE)
                {
                    throw std::runtime_error{"Unable to create a shared GL context for camera texture."};
                }
            }

            glGenTextures(1, &m_implData->cameraRGBATextureId);
            glBindTexture(GL_TEXTURE_2D, m_implData->cameraRGBATextureId);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_implData->cameraDimensions.width, m_implData->cameraDimensions.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glGenerateMipmap(GL_TEXTURE_2D);

            glBindTexture(GL_TEXTURE_2D, 0);

            glGenFramebuffers(1, &m_implData->frameBufferId);
            glBindFramebuffer(GL_FRAMEBUFFER, m_implData->frameBufferId);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_implData->cameraRGBATextureId, 0);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            m_implData->cameraUVs = sensorRotationDiff == 90 ? CAMERA_UVS_ROTATION_90 :
                          sensorRotationDiff == 180 ? CAMERA_UVS_ROTATION_180 :
                          sensorRotationDiff == 270 ? CAMERA_UVS_ROTATION_270 :
                          CAMERA_UVS_ROTATION_0;

            m_implData->cameraShaderProgramId = android::OpenGLHelpers::CreateShaderProgram(CAMERA_VERT_SHADER, CAMERA_FRAG_SHADER);

            if (API_LEVEL >= 24 && libCamera2NDK && !m_implData->overrideCameraTexture) {
                m_implData->cameraOESTextureId = m_implData->GenerateOESTexture();

                // Create the surface and surface texture that will receive the camera preview
                m_implData->surfaceTexture.InitWithTexture(m_implData->cameraOESTextureId);
                m_implData->surfaceTexture.setDefaultBufferSize(track->width, track->height);
                android::view::Surface surface(m_implData->surfaceTexture);

                // open the front or back camera
                GET_CAMERA_FUNCTION(ACameraManager_openCamera)(m_implData->cameraManager, cameraDevice->implData->cameraID.c_str(),
                                                               &cameraDeviceCallbacks,
                                                               &m_implData->cameraDevice);

                m_implData->textureWindow = reinterpret_cast<ANativeWindow *>(ANativeWindow_fromSurface(
                        GetEnvForCurrentThread(), surface));

                // Prepare request for texture target
                GET_CAMERA_FUNCTION(ACameraDevice_createCaptureRequest)(m_implData->cameraDevice,
                                                                        API24::TEMPLATE_PREVIEW,
                                                                        &m_implData->request);

                // Prepare outputs for session
                GET_CAMERA_FUNCTION(ACaptureSessionOutput_create)(m_implData->textureWindow,
                                                                  &m_implData->textureOutput);
                GET_CAMERA_FUNCTION(ACaptureSessionOutputContainer_create)(&m_implData->outputs);
                GET_CAMERA_FUNCTION(ACaptureSessionOutputContainer_add)(m_implData->outputs, m_implData->textureOutput);

                // Prepare target surface
                ANativeWindow_acquire(m_implData->textureWindow);
                GET_CAMERA_FUNCTION(ACameraOutputTarget_create)(m_implData->textureWindow, &m_implData->textureTarget);
                GET_CAMERA_FUNCTION(ACaptureRequest_addTarget)(m_implData->request, m_implData->textureTarget);

                // Create the session
                GET_CAMERA_FUNCTION(ACameraDevice_createCaptureSession)(m_implData->cameraDevice, m_implData->outputs,
                                                                        &sessionStateCallbacks,
                                                                        &m_implData->textureSession);

                // Start capturing continuously
                GET_CAMERA_FUNCTION(ACameraCaptureSession_setRepeatingRequest)(m_implData->textureSession,
                                                                               &captureCallbacks,
                                                                               1, &m_implData->request,
                                                                               nullptr);
            }

            if (eglMakeCurrent(m_implData->display, 0/*surface*/, 0/*surface*/, currentContext) == EGL_FALSE)
            {
                throw std::runtime_error{"Unable to restore GL context for camera texture init."};
            }

            return m_implData->cameraDimensions;
        });
    }

    std::vector<std::shared_ptr<CameraDevice>> Camera::Impl::GetCameraDevices()
    {
        std::vector<std::shared_ptr<CameraDevice>> cameraDevices{};

        // Get the list of available cameras
        API24::ACameraIdList *cameraIds = nullptr;
        GET_CAMERA_FUNCTION(ACameraManager_getCameraIdList)(m_implData->cameraManager, &cameraIds);

        for (int i = 0; i < cameraIds->numCameras; ++i)
        {
            const char* id = cameraIds->cameraIds[i];
            auto cameraDevice{ std::make_shared<CameraDevice>()};

            API24::ACameraMetadata *metadataObj;
            GET_CAMERA_FUNCTION(ACameraManager_getCameraCharacteristics)(m_implData->cameraManager, id, &metadataObj);

            // Get all available stream configurations supported by the camera
            API24::ACameraMetadata_const_entry streamConfigurations = {};
            GET_CAMERA_FUNCTION(ACameraMetadata_getConstEntry)(metadataObj, API24::ACAMERA_SCALER_AVAILABLE_STREAM_CONFIGURATIONS, &streamConfigurations);
            // format of the data:
            // 0: format
            // 1: width
            // 2: height
            // 3: input?

            for (uint32_t j = 0; j < streamConfigurations.count; j += 4) {
                int32_t format{streamConfigurations.data.i32[j + 0]};
                int32_t width{streamConfigurations.data.i32[j + 1]};
                int32_t height{streamConfigurations.data.i32[j + 2]};
                int32_t input{streamConfigurations.data.i32[j + 3]};

                if (input || format != AIMAGE_FORMAT_YUV_420_888)
                {
                    // Ignore the configuration if it is either an input type or not a preview format
                    continue;
                }

                auto cameraResolution{ std::make_shared<CameraTrack>() };
                cameraResolution->width = width;
                cameraResolution->height = height;

                cameraDevice->supportedResolutions.push_back(cameraResolution);
            }

            // Get camera hardware info
            API24::ACameraMetadata_const_entry metaDataEntry = {};
            GET_CAMERA_FUNCTION(ACameraMetadata_getConstEntry)(metadataObj, API24::ACAMERA_LENS_FACING, &metaDataEntry);
            auto facing{ static_cast<API24::acamera_metadata_enum_android_lens_facing_t>(metaDataEntry.data.u8[0]) };

            GET_CAMERA_FUNCTION(ACameraMetadata_getConstEntry)(metadataObj, API24::ACAMERA_SENSOR_ORIENTATION, &metaDataEntry);
            int32_t sensorOrientation{ metaDataEntry.data.i32[0] };

            GET_CAMERA_FUNCTION(ACameraMetadata_getConstEntry)(metadataObj, API24::ACAMERA_FLASH_INFO_AVAILABLE, &metaDataEntry);
            bool torchSupported = metaDataEntry.data.u8[0];

            GET_CAMERA_FUNCTION(ACameraMetadata_getConstEntry)(metadataObj, API24::ACAMERA_CONTROL_ZOOM_RATIO, &metaDataEntry);
            float zoomRatio{ metaDataEntry.data.f[0] };

            GET_CAMERA_FUNCTION(ACameraMetadata_getConstEntry)(metadataObj, API24::ACAMERA_CONTROL_ZOOM_RATIO_RANGE, &metaDataEntry);
            float minZoomRatio{ metaDataEntry.data.f[0] };
            float maxZoomRatio{ metaDataEntry.data.f[1] };

            // update the cameraDevice information
            cameraDevice->implData = std::make_unique<CameraDevice::ImplData>();
            cameraDevice->implData->cameraID = id;
            cameraDevice->implData->sensorRotation = sensorOrientation;
            cameraDevice->implData->facingUser = facing == API24::ACAMERA_LENS_FACING_FRONT;

            cameraDevice->capabilities.push_back(std::make_shared<CameraCapabilityTemplate<std::string>>(
                CameraCapability::Capability::FacingMode,
                facing == API24::ACAMERA_LENS_FACING_FRONT ? "user" : "environment",
                facing == API24::ACAMERA_LENS_FACING_FRONT ? "user" : "environment",
                facing == API24::ACAMERA_LENS_FACING_FRONT ? std::vector<std::string>{"user"} : std::vector<std::string>{"environment"})
            );

            cameraDevice->capabilities.push_back( std::make_shared<CameraCapabilityTemplate<bool>>
            (
                CameraCapability::Capability::Torch,
                false,
                false,
                torchSupported ? std::vector<bool>{false, true} : std::vector<bool>{false},
                [this](bool newValue)
                {
                    uint8_t torchMode = newValue
                                        ? API24::acamera_metadata_enum_android_flash_mode_t::ACAMERA_FLASH_MODE_TORCH
                                        : API24::acamera_metadata_enum_android_flash_mode_t::ACAMERA_FLASH_MODE_OFF;
                    GET_CAMERA_FUNCTION(ACaptureRequest_setEntry_u8)(m_implData->request, API24::ACAMERA_FLASH_MODE, 1, &torchMode);

                    // Update the camera request
                    GET_CAMERA_FUNCTION(ACameraCaptureSession_setRepeatingRequest)(m_implData->textureSession, &captureCallbacks, 1, &m_implData->request, nullptr);
                    return true;
                }
            ));

            cameraDevice->capabilities.push_back( std::make_shared<CameraCapabilityTemplate<double>>
            (
                    CameraCapability::Capability::Zoom,
                            zoomRatio,
                            1.0, // Set the default target zoom to 1.0 (no zoom)
                            std::vector<double>{minZoomRatio, maxZoomRatio},
                            [this](double newValue)
                            {
                                float newZoomRatio{ static_cast<float>(newValue) };
                                GET_CAMERA_FUNCTION(ACaptureRequest_setEntry_float)(m_implData->request, API24::ACAMERA_CONTROL_ZOOM_RATIO, 1, &newZoomRatio);

                                // Update the camera request
                                GET_CAMERA_FUNCTION(ACameraCaptureSession_setRepeatingRequest)(m_implData->textureSession, &captureCallbacks, 1, &m_implData->request, nullptr);
                                return true;
                            }
            ));

            cameraDevices.push_back(cameraDevice);
        }

        GET_CAMERA_FUNCTION(ACameraManager_deleteCameraIdList)(cameraIds);

        return cameraDevices;
    }

    void Camera::Impl::SetTextureOverride(void* texturePtr)
    {
        if (!m_implData->overrideCameraTexture)
        {
            throw std::runtime_error{"Trying to override NativeCamera Texture."};
        }
        m_implData->cameraOESTextureId = reinterpret_cast<uintptr_t>(texturePtr);
    }

    void Camera::Impl::UpdateCameraTexture(bgfx::TextureHandle textureHandle)
    {
        EGLContext currentContext = eglGetCurrentContext();
        if (m_implData->context != EGL_NO_CONTEXT)
        {
            // use the newly created shared context
            if (eglMakeCurrent(m_implData->display, 0/*surface*/, 0/*surface*/, m_implData->context) == EGL_FALSE)
            {
                throw std::runtime_error{"Unable to make current shared GL context for camera texture."};
            }
        }

        if (API_LEVEL >= 24 && !m_implData->overrideCameraTexture)
        {
            m_implData->surfaceTexture.updateTexImage();
        }

        glBindFramebuffer(GL_FRAMEBUFFER, m_implData->frameBufferId);
        glViewport(0, 0, m_implData->cameraDimensions.width, m_implData->cameraDimensions.height);
        glUseProgram(m_implData->cameraShaderProgramId);

        auto vertexPositionsUniformLocation{ glGetUniformLocation(m_implData->cameraShaderProgramId, "positions") };
        glUniform2fv(vertexPositionsUniformLocation, CAMERA_VERTEX_COUNT, CAMERA_VERTEX_POSITIONS);

        auto uvsUniformLocation{ glGetUniformLocation(m_implData->cameraShaderProgramId, "uvs") };
        glUniform2fv(uvsUniformLocation, CAMERA_UVS_COUNT, m_implData->cameraUVs);

        // Configure the camera texture
        auto cameraTextureUniformLocation{glGetUniformLocation(m_implData->cameraShaderProgramId, "cameraTexture")};
        glUniform1i(cameraTextureUniformLocation, android::OpenGLHelpers::GetTextureUnit(GL_TEXTURE0));
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_EXTERNAL_OES, m_implData->cameraOESTextureId);
        glBindSampler(android::OpenGLHelpers::GetTextureUnit(GL_TEXTURE0), 0);

        // Draw the quad
        glDrawArrays(GL_TRIANGLE_STRIP, 0, CAMERA_VERTEX_COUNT);

        glUseProgram(0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // bind previously bound context
        if (eglMakeCurrent(m_implData->display, 0/*surface*/, 0/*surface*/, currentContext) == EGL_FALSE)
        {
            throw std::runtime_error{"Unable to make current shared GL context for camera texture."};
        }

        arcana::make_task(m_implData->deviceContext->BeforeRenderScheduler(), arcana::cancellation::none(), [this, textureHandle] {
            bgfx::overrideInternal(textureHandle, m_implData->cameraRGBATextureId);
        });
    }

    void Camera::Impl::Close()
    {
        if (API_LEVEL >= 24 && !m_implData->overrideCameraTexture)
        {
            // Stop recording to SurfaceTexture and do some cleanup
            GET_CAMERA_FUNCTION(ACameraCaptureSession_stopRepeating)(m_implData->textureSession);
            GET_CAMERA_FUNCTION(ACameraCaptureSession_close)(m_implData->textureSession);
            GET_CAMERA_FUNCTION(ACaptureSessionOutputContainer_free)(m_implData->outputs);
            GET_CAMERA_FUNCTION(ACaptureSessionOutput_free)(m_implData->output);

            GET_CAMERA_FUNCTION(ACameraDevice_close)(m_implData->cameraDevice);

            // Capture request for SurfaceTexture
            ANativeWindow_release(m_implData->textureWindow);
            GET_CAMERA_FUNCTION(ACaptureRequest_free)(m_implData->request);
        }

        if (m_implData->context != EGL_NO_CONTEXT)
        {
            eglDestroyContext(m_implData->display, m_implData->context);
        }
    }
}