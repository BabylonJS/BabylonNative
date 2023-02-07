#include <napi/napi.h>
#include "NativeCamera.h"
#include "../CameraDevice.h"
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
    struct CameraTrack::Impl
    {
        int32_t width{};
        int32_t height{};
    };

    struct CameraDevice::Impl {
        Impl(Napi::Env env)
                : env{env}
        {
        }

        GLuint GenerateOESTexture() {
            GLuint oesTexture;
            glGenTextures(1, &oesTexture);
            glBindTexture(GL_TEXTURE_EXTERNAL_OES, oesTexture);
            glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_EXTERNAL_OES, 0);
            return oesTexture;
        }

        int GetCurrentSensorRotationDiff() {
            // Get the phone's current rotation so we can determine if the camera image needs to be rotated based on the sensor's natural orientation
            int32_t phoneRotation{GetAppContext().getSystemService<android::view::WindowManager>().getDefaultDisplay().getRotation() * 90};

            // The sensor rotation dictates the orientation of the camera when the phone is in it's default orientation
            // Subtracting the phone's rotation from the camera's rotation will give us the current orientation
            // of the sensor. Then add 360 and modulus 360 to ensure we're always talking about positive degrees.
            int currentSensorRotationDiff{(sensorRotation - phoneRotation + 360) % 360};
            bool sensorIsPortrait{currentSensorRotationDiff == 90 || currentSensorRotationDiff == 270};
            if (facingUser && !sensorIsPortrait)
            {
                // Compensate for the front facing camera being naturally mirrored. In the portrait orientation
                // the mirrored behavior matches the browser, but in landscape it would result in the image rendering
                // upside down. Rotate the image by 180 to compensate.
                currentSensorRotationDiff = (currentSensorRotationDiff + 180) % 360;
            }

            return currentSensorRotationDiff;
        }

        Napi::Env env;

        arcana::affinity threadAffinity{};

        std::vector<CameraTrack> supportedResolutions{};
        std::vector<std::unique_ptr<Capability>> capabilities{};
        std::string cameraID{};
        int32_t sensorRotation{};
        bool facingUser{};
        CameraDimensions cameraDimensions{};
        int32_t sensorRotationDiff{};
        bool updateTextureDimensions{true};

        Graphics::DeviceContext* deviceContext{};

        API24::ACameraDevice* aCameraDevice{};
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

        EGLContext context{EGL_NO_CONTEXT};
        EGLDisplay display{};
    };

    // Vertex positions for the camera texture
    constexpr size_t CAMERA_VERTEX_COUNT{4};
    constexpr GLfloat CAMERA_VERTEX_POSITIONS[CAMERA_VERTEX_COUNT * 2]{-1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f};

    // UV mappings to correct for the different orientations of the screen versus the camera sensor
    constexpr size_t CAMERA_UVS_COUNT{4};
    constexpr GLfloat CAMERA_UVS_ROTATION_0[CAMERA_UVS_COUNT * 2]{0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f};
    constexpr GLfloat CAMERA_UVS_ROTATION_90[CAMERA_UVS_COUNT * 2]{0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f};
    constexpr GLfloat CAMERA_UVS_ROTATION_180[CAMERA_UVS_COUNT * 2]{1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f};
    constexpr GLfloat CAMERA_UVS_ROTATION_270[CAMERA_UVS_COUNT * 2]{1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};

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

    arcana::task<CameraDevice::CameraDimensions, std::exception_ptr> CameraDevice::OpenAsync(const CameraTrack& track)
    {
        if (!m_impl->deviceContext){
            m_impl->deviceContext = &Graphics::DeviceContext::GetFromJavaScript(m_impl->env);
            m_impl->threadAffinity = std::this_thread::get_id();
        }

        return android::Permissions::CheckCameraPermissionAsync().then(arcana::inline_scheduler, arcana::cancellation::none(), [this, &track]()
        {
            // Get the phone's current rotation so we can determine if the camera image needs to be rotated based on the sensor's natural orientation
            m_impl->sensorRotationDiff = m_impl->GetCurrentSensorRotationDiff();

            m_impl->cameraDimensions.width = track.Width();
            m_impl->cameraDimensions.height = track.Height();

            // Check if there is an already available context for this thread
            EGLContext currentContext = eglGetCurrentContext();
            if (currentContext == EGL_NO_CONTEXT)
            {
                // create a shared context with bgfx so JNI thread (by surfaceTexture) can update the texture
                m_impl->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
                eglInitialize(m_impl->display, nullptr, nullptr);

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
                eglChooseConfig(m_impl->display, attrs, &config, 1, &numConfig);

                static const EGLint contextAttribs[] = {
                    EGL_CONTEXT_MAJOR_VERSION_KHR,
                    3,
                    EGL_CONTEXT_MINOR_VERSION_KHR,
                    0,
                    EGL_NONE};

                m_impl->context = eglCreateContext(m_impl->display, config, bgfx::getInternalData()->context, contextAttribs);
                if (eglMakeCurrent(m_impl->display, 0/*surface*/, 0/*surface*/, m_impl->context) == EGL_FALSE)
                {
                    throw std::runtime_error{"Unable to create a shared GL context for camera texture."};
                }
            }

            m_impl->cameraShaderProgramId = android::OpenGLHelpers::CreateShaderProgram(CAMERA_VERT_SHADER, CAMERA_FRAG_SHADER);

            m_impl->cameraOESTextureId = m_impl->GenerateOESTexture();

            // Create the surface and surface texture that will receive the camera preview
            m_impl->surfaceTexture.InitWithTexture(m_impl->cameraOESTextureId);
            m_impl->surfaceTexture.setDefaultBufferSize(track.Width(), track.Height());
            android::view::Surface surface(m_impl->surfaceTexture);

            // open the camera stream
            auto cameraManager{GET_CAMERA_FUNCTION(ACameraManager_create)()};
            GET_CAMERA_FUNCTION(ACameraManager_openCamera)(cameraManager, m_impl->cameraID.c_str(),
                                                           &cameraDeviceCallbacks,
                                                           &m_impl->aCameraDevice);

            m_impl->textureWindow = reinterpret_cast<ANativeWindow*>(ANativeWindow_fromSurface(
                    GetEnvForCurrentThread(), surface));

            // Prepare request for texture target
            GET_CAMERA_FUNCTION(ACameraDevice_createCaptureRequest)(m_impl->aCameraDevice,
                                                                    API24::TEMPLATE_PREVIEW,
                                                                    &m_impl->request);

            // Prepare outputs for session
            GET_CAMERA_FUNCTION(ACaptureSessionOutput_create)(m_impl->textureWindow,
                                                              &m_impl->textureOutput);
            GET_CAMERA_FUNCTION(ACaptureSessionOutputContainer_create)(&m_impl->outputs);
            GET_CAMERA_FUNCTION(ACaptureSessionOutputContainer_add)(m_impl->outputs, m_impl->textureOutput);

            // Prepare target surface
            ANativeWindow_acquire(m_impl->textureWindow);
            GET_CAMERA_FUNCTION(ACameraOutputTarget_create)(m_impl->textureWindow, &m_impl->textureTarget);
            GET_CAMERA_FUNCTION(ACaptureRequest_addTarget)(m_impl->request, m_impl->textureTarget);

            // Create the session
            GET_CAMERA_FUNCTION(ACameraDevice_createCaptureSession)(m_impl->aCameraDevice, m_impl->outputs,
                                                                    &sessionStateCallbacks,
                                                                    &m_impl->textureSession);

            // Start capturing continuously
            GET_CAMERA_FUNCTION(ACameraCaptureSession_setRepeatingRequest)(m_impl->textureSession,
                                                                           &captureCallbacks,
                                                                           1, &m_impl->request,
                                                                           nullptr);

            GET_CAMERA_FUNCTION(ACameraManager_delete)(cameraManager);

            if (eglMakeCurrent(m_impl->display, 0/*surface*/, 0/*surface*/, currentContext) == EGL_FALSE)
            {
                throw std::runtime_error{"Unable to restore GL context for camera texture init."};
            }

            // To match the web implementation if the sensor is rotated into a portrait orientation then the width and height
            // of the video should be swapped
            bool sensorIsPortrait{m_impl->sensorRotationDiff == 90 || m_impl->sensorRotationDiff == 270};
            return !sensorIsPortrait
                   ? CameraDimensions{m_impl->cameraDimensions.width, m_impl->cameraDimensions.height}
                   : CameraDimensions{m_impl->cameraDimensions.height, m_impl->cameraDimensions.width};
        });
    }

    std::vector<CameraDevice> CameraDevice::GetCameraDevices(Napi::Env env)
    {
        if (API_LEVEL < 24)
        {
            throw std::runtime_error{"Android Platform level < 24. NativeCameraPlugin is only supported on Android devices running API 24+"};
        }

        auto cameraManager{GET_CAMERA_FUNCTION(ACameraManager_create)()};

        std::vector<CameraDevice> cameraDevices{};

        // Get the list of available cameras
        API24::ACameraIdList* cameraIds = nullptr;
        GET_CAMERA_FUNCTION(ACameraManager_getCameraIdList)(cameraManager, &cameraIds);

        for (int i = 0; i < cameraIds->numCameras; ++i)
        {
            const char* id = cameraIds->cameraIds[i];
            auto cameraDeviceImpl{std::make_unique<CameraDevice::Impl>(env)};

            API24::ACameraMetadata* metadataObj;
            GET_CAMERA_FUNCTION(ACameraManager_getCameraCharacteristics)(cameraManager, id, &metadataObj);

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

                auto cameraTrackImpl{std::make_unique<CameraTrack::Impl>()};
                cameraTrackImpl->width = width;
                cameraTrackImpl->height = height;
                cameraDeviceImpl->supportedResolutions.push_back(CameraTrack{std::move(cameraTrackImpl)});
            }

            // Get camera hardware info
            API24::ACameraMetadata_const_entry metaDataEntry{};
            GET_CAMERA_FUNCTION(ACameraMetadata_getConstEntry)(metadataObj, API24::ACAMERA_LENS_FACING, &metaDataEntry);
            auto facing{static_cast<API24::acamera_metadata_enum_android_lens_facing_t>(metaDataEntry.data.u8[0])};

            GET_CAMERA_FUNCTION(ACameraMetadata_getConstEntry)(metadataObj, API24::ACAMERA_SENSOR_ORIENTATION, &metaDataEntry);
            int32_t sensorOrientation{metaDataEntry.data.i32[0]};

            GET_CAMERA_FUNCTION(ACameraMetadata_getConstEntry)(metadataObj, API24::ACAMERA_FLASH_INFO_AVAILABLE, &metaDataEntry);
            bool torchSupported = metaDataEntry.data.u8[0];

            GET_CAMERA_FUNCTION(ACameraMetadata_getConstEntry)(metadataObj, API24::ACAMERA_CONTROL_ZOOM_RATIO, &metaDataEntry);
            float zoomRatio{metaDataEntry.data.f[0]};

            GET_CAMERA_FUNCTION(ACameraMetadata_getConstEntry)(metadataObj, API24::ACAMERA_CONTROL_ZOOM_RATIO_RANGE, &metaDataEntry);
            float minZoomRatio{metaDataEntry.data.f[0]};
            float maxZoomRatio{metaDataEntry.data.f[1]};

            // Update the cameraDevice information
            cameraDeviceImpl->cameraID = id;
            cameraDeviceImpl->sensorRotation = sensorOrientation;
            cameraDeviceImpl->facingUser = facing == API24::ACAMERA_LENS_FACING_FRONT;

            // Create the capabilities
            cameraDeviceImpl->capabilities.push_back(std::make_unique<CameraCapabilityTemplate<std::string>>
            (
                Capability::Feature::FacingMode,
                facing == API24::ACAMERA_LENS_FACING_FRONT ? "user" : "environment",
                facing == API24::ACAMERA_LENS_FACING_FRONT ? "user" : "environment",
                facing == API24::ACAMERA_LENS_FACING_FRONT ? std::vector<std::string>{"user"} : std::vector<std::string>{"environment"}
            ));

            cameraDeviceImpl->capabilities.push_back(std::make_unique<CameraCapabilityTemplate<bool>>
            (
                Capability::Feature::Torch,
                false,
                false,
                torchSupported ? std::vector<bool>{false, true} : std::vector<bool>{false},
                    [impl{cameraDeviceImpl.get()}](bool newValue)
                {
                    uint8_t torchMode = newValue
                                        ? API24::acamera_metadata_enum_android_flash_mode_t::ACAMERA_FLASH_MODE_TORCH
                                        : API24::acamera_metadata_enum_android_flash_mode_t::ACAMERA_FLASH_MODE_OFF;
                    GET_CAMERA_FUNCTION(ACaptureRequest_setEntry_u8)(impl->request, API24::ACAMERA_FLASH_MODE, 1, &torchMode);

                    // Update the camera request
                    GET_CAMERA_FUNCTION(ACameraCaptureSession_setRepeatingRequest)(impl->textureSession, &captureCallbacks, 1, &impl->request, nullptr);
                    return true;
                }
            ));

            cameraDeviceImpl->capabilities.push_back(std::make_unique<CameraCapabilityTemplate<double>>
            (
                Capability::Feature::Zoom,
                zoomRatio,
                1.0, // Set the default target zoom to 1.0 (no zoom)
                std::vector<double>{minZoomRatio, maxZoomRatio},
                    [impl{cameraDeviceImpl.get()}](double newValue)
                {
                    float newZoomRatio{static_cast<float>(newValue)};
                    GET_CAMERA_FUNCTION(ACaptureRequest_setEntry_float)(impl->request, API24::ACAMERA_CONTROL_ZOOM_RATIO, 1, &newZoomRatio);

                    // Update the camera request
                    GET_CAMERA_FUNCTION(ACameraCaptureSession_setRepeatingRequest)(impl->textureSession, &captureCallbacks, 1, &impl->request, nullptr);
                    return true;
                }
            ));

            cameraDevices.push_back(CameraDevice{std::move(cameraDeviceImpl)});
        }

        GET_CAMERA_FUNCTION(ACameraManager_deleteCameraIdList)(cameraIds);
        GET_CAMERA_FUNCTION(ACameraManager_delete)(cameraManager);

        return cameraDevices;
    }

    CameraDevice::CameraDimensions CameraDevice::UpdateCameraTexture(bgfx::TextureHandle textureHandle)
    {
        EGLContext currentContext = eglGetCurrentContext();
        if (m_impl->context != EGL_NO_CONTEXT)
        {
            // use the newly created shared context
            if (eglMakeCurrent(m_impl->display, 0/*surface*/, 0/*surface*/, m_impl->context) == EGL_FALSE)
            {
                throw std::runtime_error{"Unable to make current shared GL context for camera texture."};
            }
        }

        int currentSensorRotationDiff = m_impl->GetCurrentSensorRotationDiff();

        // The UI Orientation has changed. Update our internal texture
        if (currentSensorRotationDiff != m_impl->sensorRotationDiff)
        {
            m_impl->sensorRotationDiff = currentSensorRotationDiff;
            m_impl->updateTextureDimensions = true;
        }

        bool sensorIsPortrait{m_impl->sensorRotationDiff == 90 || m_impl->sensorRotationDiff == 270};

        if (m_impl->updateTextureDimensions)
        {
            glGenTextures(1, &m_impl->cameraRGBATextureId);
            glBindTexture(GL_TEXTURE_2D, m_impl->cameraRGBATextureId);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, !sensorIsPortrait ? m_impl->cameraDimensions.width : m_impl->cameraDimensions.height, !sensorIsPortrait ? m_impl->cameraDimensions.height : m_impl->cameraDimensions.width, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glGenerateMipmap(GL_TEXTURE_2D);

            glBindTexture(GL_TEXTURE_2D, 0);

            glGenFramebuffers(1, &m_impl->frameBufferId);
            glBindFramebuffer(GL_FRAMEBUFFER, m_impl->frameBufferId);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_impl->cameraRGBATextureId, 0);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            m_impl->updateTextureDimensions = false;
        }

        m_impl->surfaceTexture.updateTexImage();

        glBindFramebuffer(GL_FRAMEBUFFER, m_impl->frameBufferId);
        glViewport(0, 0, !sensorIsPortrait ? m_impl->cameraDimensions.width : m_impl->cameraDimensions.height, !sensorIsPortrait ? m_impl->cameraDimensions.height : m_impl->cameraDimensions.width);
        glUseProgram(m_impl->cameraShaderProgramId);

        auto vertexPositionsUniformLocation{glGetUniformLocation(m_impl->cameraShaderProgramId, "positions")};
        glUniform2fv(vertexPositionsUniformLocation, CAMERA_VERTEX_COUNT, CAMERA_VERTEX_POSITIONS);

        auto uvsUniformLocation{glGetUniformLocation(m_impl->cameraShaderProgramId, "uvs")};
        glUniform2fv(uvsUniformLocation, CAMERA_UVS_COUNT,
             m_impl->sensorRotationDiff == 90 ? CAMERA_UVS_ROTATION_90 :
             m_impl->sensorRotationDiff == 180 ? CAMERA_UVS_ROTATION_180 :
             m_impl->sensorRotationDiff == 270 ? CAMERA_UVS_ROTATION_270 :
            CAMERA_UVS_ROTATION_0);

        // Configure the camera texture
        auto cameraTextureUniformLocation{glGetUniformLocation(m_impl->cameraShaderProgramId, "cameraTexture")};
        glUniform1i(cameraTextureUniformLocation, android::OpenGLHelpers::GetTextureUnit(GL_TEXTURE0));
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_EXTERNAL_OES, m_impl->cameraOESTextureId);
        glBindSampler(android::OpenGLHelpers::GetTextureUnit(GL_TEXTURE0), 0);

        // Draw the quad
        glDrawArrays(GL_TRIANGLE_STRIP, 0, CAMERA_VERTEX_COUNT);

        glUseProgram(0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // bind previously bound context
        if (eglMakeCurrent(m_impl->display, 0/*surface*/, 0/*surface*/, currentContext) == EGL_FALSE)
        {
            throw std::runtime_error{"Unable to make current shared GL context for camera texture."};
        }

        arcana::make_task(m_impl->deviceContext->BeforeRenderScheduler(), arcana::cancellation::none(), [rgbaTextureId = m_impl->cameraRGBATextureId, textureHandle] {
            bgfx::overrideInternal(textureHandle, rgbaTextureId);
        });

        return !sensorIsPortrait
            ? CameraDimensions{m_impl->cameraDimensions.width, m_impl->cameraDimensions.height}
            : CameraDimensions{m_impl->cameraDimensions.height, m_impl->cameraDimensions.width};
    }

    void CameraDevice::Close()
    {
        if (m_impl->textureSession == nullptr)
        {
            // This device was either never opened, or has already been closed.
            // No action is required.
            return;
        }

        // Stop recording to SurfaceTexture and do some cleanup
        GET_CAMERA_FUNCTION(ACameraCaptureSession_stopRepeating)(m_impl->textureSession);
        GET_CAMERA_FUNCTION(ACameraCaptureSession_close)(m_impl->textureSession);
        GET_CAMERA_FUNCTION(ACaptureSessionOutputContainer_free)(m_impl->outputs);
        GET_CAMERA_FUNCTION(ACaptureSessionOutput_free)(m_impl->output);

        GET_CAMERA_FUNCTION(ACameraDevice_close)(m_impl->aCameraDevice);

        // Capture request for SurfaceTexture
        ANativeWindow_release(m_impl->textureWindow);
        GET_CAMERA_FUNCTION(ACaptureRequest_free)(m_impl->request);

        if (m_impl->context != EGL_NO_CONTEXT)
        {
            eglDestroyContext(m_impl->display, m_impl->context);
        }
    }
}

#include "../CameraDeviceSharedPImpl.h"