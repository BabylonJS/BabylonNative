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
    struct CameraConfiguration
    {
        std::string cameraID;
        uint32_t width;
        uint32_t height;
        int32_t sensorRotation;
    };

    struct Camera::Impl::ImplData {
        Graphics::DeviceContext* m_deviceContext;
        Napi::Env m_env;

        bool m_overrideCameraTexture{};

        CameraDimensions m_cameraDimensions{};

        API24::ACameraManager* m_cameraManager{};
        API24::ACameraDevice* m_cameraDevice{};
        API24::ACameraOutputTarget* m_textureTarget{};
        API24::ACaptureRequest* m_request{};
        ANativeWindow* m_textureWindow{};
        API24::ACameraCaptureSession* m_textureSession{};
        API24::ACaptureSessionOutput* m_textureOutput{};
        API24::ACaptureSessionOutput* m_output{};
        API24::ACaptureSessionOutputContainer* m_outputs{};
        android::graphics::SurfaceTexture m_surfaceTexture{};

        GLuint m_cameraOESTextureId{};
        GLuint m_cameraRGBATextureId{};
        GLuint m_cameraShaderProgramId{};
        GLuint m_frameBufferId{};
        const GLfloat* m_cameraUVs{};

        EGLContext m_context{EGL_NO_CONTEXT};
        EGLDisplay m_display{};

        ImplData(Napi::Env env, bool overrideCameraTexture)
            : m_deviceContext{nullptr}
            , m_env{env}
            , m_overrideCameraTexture{overrideCameraTexture} {};

        GLuint GenerateOESTexture(){
            GLuint oesTexture;
            glGenTextures(1, &oesTexture);
            glBindTexture(GL_TEXTURE_EXTERNAL_OES, oesTexture);
            glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_EXTERNAL_OES, 0);
            return oesTexture;
        }

        CameraConfiguration GetCameraConfiguration(uint32_t maxWidth, uint32_t maxHeight, bool frontCamera){
            // Get the list of available cameras
            API24::ACameraIdList *cameraIds = nullptr;
            GET_CAMERA_FUNCTION(ACameraManager_getCameraIdList)(m_cameraManager, &cameraIds);

            const char* bestCameraId{nullptr};
            uint32_t bestPixelCount{0};
            uint32_t bestDimDiff{0};
            uint32_t bestWidth{0};
            uint32_t bestHeight{0};
            int32_t bestSensorOrientation{0};
            bool foundExactMatch{false};

            // Iterate over all of the cameras and find the one that has the best stream configuration
            for (int i = 0; i < cameraIds->numCameras; ++i)
            {
                const char* id = cameraIds->cameraIds[i];

                API24::ACameraMetadata *metadataObj;
                GET_CAMERA_FUNCTION(ACameraManager_getCameraCharacteristics)(m_cameraManager, id, &metadataObj);

                API24::ACameraMetadata_const_entry lensInfo = {};
                GET_CAMERA_FUNCTION(ACameraMetadata_getConstEntry)(metadataObj, API24::ACAMERA_LENS_FACING, &lensInfo);

                API24::ACameraMetadata_const_entry sensorOrientation = {};
                GET_CAMERA_FUNCTION(ACameraMetadata_getConstEntry)(metadataObj, API24::ACAMERA_SENSOR_ORIENTATION, &sensorOrientation);

                auto facing = static_cast<API24::acamera_metadata_enum_android_lens_facing_t>(lensInfo.data.u8[0]);
                if (facing != (frontCamera ? API24::ACAMERA_LENS_FACING_FRONT : API24::ACAMERA_LENS_FACING_BACK))
                {
                    // Ignore cameras facing the wrong direction
                    continue;
                }

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

                    if (input || format != AIMAGE_FORMAT_YUV_420_888 || static_cast<uint32_t>(width) > maxWidth || static_cast<uint32_t>(height) > maxHeight)
                    {
                        // Ignore the configuration if it is either an input type or not a preview format
                        // Also ignore the configuration if either the width or height is beyond the max allowed
                        continue;
                    }

                    // Calculate pixel count and dimension differential and take the best qualifying one.
                    uint32_t pixelCount{static_cast<uint32_t>(width * height)};
                    uint32_t dimDiff{(maxWidth - width) + (maxHeight - height)};
                    if (bestCameraId == nullptr || pixelCount > bestPixelCount || (pixelCount == bestPixelCount && dimDiff < bestDimDiff))
                    {
                        bestPixelCount = pixelCount;
                        bestCameraId = id;
                        bestDimDiff = dimDiff;
                        bestWidth = width;
                        bestHeight = height;
                        bestSensorOrientation = sensorOrientation.data.i32[0];

                        // Check if we got an exact match, and exit the loop early in this case.
                        if (static_cast<uint32_t>(width) == maxWidth && static_cast<uint32_t>(height) == maxHeight)
                        {
                            foundExactMatch = true;
                            break;
                        }
                    }
                }

                if (foundExactMatch)
                {
                    break;
                }
            }

            GET_CAMERA_FUNCTION(ACameraManager_deleteCameraIdList)(cameraIds);
            return {bestCameraId, bestWidth, bestHeight, bestSensorOrientation};
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
    }

    Camera::Impl::~Impl()
    {
    }

    arcana::task<Camera::Impl::CameraDimensions, std::exception_ptr> Camera::Impl::Open(uint32_t maxWidth, uint32_t maxHeight, bool frontCamera)
    {
        if (!m_implData->m_deviceContext){
            m_implData->m_deviceContext = &Graphics::DeviceContext::GetFromJavaScript(m_implData->m_env);
        }

        if (maxWidth == 0 || maxWidth > std::numeric_limits<int32_t>::max()) {
            maxWidth = std::numeric_limits<int32_t>::max();
        }
        if (maxHeight == 0 || maxHeight > std::numeric_limits<int32_t>::max()) {
            maxHeight = std::numeric_limits<int32_t>::max();
        }

        return android::Permissions::CheckCameraPermissionAsync().then(arcana::inline_scheduler, arcana::cancellation::none(), [this, maxWidth, maxHeight, frontCamera]()
        {
            // Get the phone's current rotation so we can determine if the camera image needs to be rotated based on the sensor's natural orientation
            int phoneRotation{ GetAppContext().getSystemService<android::view::WindowManager>().getDefaultDisplay().getRotation() * 90 };
            CameraConfiguration bestCameraConfiguration{"", maxWidth, maxHeight, phoneRotation};

            if (API_LEVEL >= 24 && libCamera2NDK && !m_implData->m_overrideCameraTexture) {
                m_implData->m_cameraManager = GET_CAMERA_FUNCTION(ACameraManager_create)();
                bestCameraConfiguration = m_implData->GetCameraConfiguration(maxWidth, maxHeight, frontCamera);

                // If no matching device, throw an error with the message "ConstraintError" which matches the behavior in the browser.
                if (bestCameraConfiguration.cameraID.empty())
                {
                    throw std::runtime_error{"ConstraintError: Unable to match constraints to a supported camera configuration."};
                }
            }

            // The sensor rotation dictates the orientation of the camera when the phone is in it's default orientation
            // Subtracting the phone's rotation from the camera's rotation will give us the current orientation
            // of the sensor. Then add 360 and modulus 360 to ensure we're always talking about positive degrees.
            int sensorRotationDiff{(bestCameraConfiguration.sensorRotation - phoneRotation + 360) % 360};
            bool sensorIsPortrait{sensorRotationDiff == 90 || sensorRotationDiff == 270};
            if (frontCamera && !sensorIsPortrait && !m_implData->m_overrideCameraTexture)
            {
                // Compensate for the front facing camera being naturally mirrored. In the portrait orientation
                // the mirrored behavior matches the browser, but in landscape it would result in the image rendering
                // upside down. Rotate the image by 180 to compensate.
                sensorRotationDiff = (sensorRotationDiff + 180) % 360;
            }

            // To match the web implementation if the sensor is rotated into a portrait orientation then the width and height
            // of the video should be swapped
            m_implData->m_cameraDimensions.width = !sensorIsPortrait ? bestCameraConfiguration.width : bestCameraConfiguration.height;
            m_implData->m_cameraDimensions.height = !sensorIsPortrait ? bestCameraConfiguration.height : bestCameraConfiguration.width;

            // Check if there is an already available context for this thread
            EGLContext currentContext = eglGetCurrentContext();
            if (currentContext == EGL_NO_CONTEXT)
            {
                // create a shared context with bgfx so JNI thread (by surfaceTexture) can update the texture
                m_implData->m_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
                eglInitialize(m_implData->m_display, nullptr, nullptr);

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
                eglChooseConfig(m_implData->m_display, attrs, &config, 1, &numConfig);

                static const EGLint contextAttribs[] = {
                    EGL_CONTEXT_MAJOR_VERSION_KHR,
                    3,
                    EGL_CONTEXT_MINOR_VERSION_KHR,
                    0,
                    EGL_NONE};

                m_implData->m_context = eglCreateContext(m_implData->m_display, config, bgfx::getInternalData()->context, contextAttribs);
                if (eglMakeCurrent(m_implData->m_display, 0/*surface*/, 0/*surface*/, m_implData->m_context) == EGL_FALSE)
                {
                    throw std::runtime_error{"Unable to create a shared GL context for camera texture."};
                }
            }

            glGenTextures(1, &m_implData->m_cameraRGBATextureId);
            glBindTexture(GL_TEXTURE_2D, m_implData->m_cameraRGBATextureId);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_implData->m_cameraDimensions.width, m_implData->m_cameraDimensions.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glGenerateMipmap(GL_TEXTURE_2D);

            glBindTexture(GL_TEXTURE_2D, 0);

            glGenFramebuffers(1, &m_implData->m_frameBufferId);
            glBindFramebuffer(GL_FRAMEBUFFER, m_implData->m_frameBufferId);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_implData->m_cameraRGBATextureId, 0);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            m_implData->m_cameraUVs = sensorRotationDiff == 90 ? CAMERA_UVS_ROTATION_90 :
                          sensorRotationDiff == 180 ? CAMERA_UVS_ROTATION_180 :
                          sensorRotationDiff == 270 ? CAMERA_UVS_ROTATION_270 :
                          CAMERA_UVS_ROTATION_0;

            m_implData->m_cameraShaderProgramId = android::OpenGLHelpers::CreateShaderProgram(CAMERA_VERT_SHADER, CAMERA_FRAG_SHADER);

            if (API_LEVEL >= 24 && libCamera2NDK && !m_implData->m_overrideCameraTexture) {
                m_implData->m_cameraOESTextureId = m_implData->GenerateOESTexture();

                // Create the surface and surface texture that will receive the camera preview
                m_implData->m_surfaceTexture.InitWithTexture(m_implData->m_cameraOESTextureId);
                m_implData->m_surfaceTexture.setDefaultBufferSize(bestCameraConfiguration.width, bestCameraConfiguration.height);
                android::view::Surface surface(m_implData->m_surfaceTexture);

                // open the front or back camera
                GET_CAMERA_FUNCTION(ACameraManager_openCamera)(m_implData->m_cameraManager, bestCameraConfiguration.cameraID.c_str(),
                                                               &cameraDeviceCallbacks,
                                                               &m_implData->m_cameraDevice);

                m_implData->m_textureWindow = reinterpret_cast<ANativeWindow *>(ANativeWindow_fromSurface(
                        GetEnvForCurrentThread(), surface));

                // Prepare request for texture target
                GET_CAMERA_FUNCTION(ACameraDevice_createCaptureRequest)(m_implData->m_cameraDevice,
                                                                        API24::TEMPLATE_PREVIEW,
                                                                        &m_implData->m_request);

                // Prepare outputs for session
                GET_CAMERA_FUNCTION(ACaptureSessionOutput_create)(m_implData->m_textureWindow,
                                                                  &m_implData->m_textureOutput);
                GET_CAMERA_FUNCTION(ACaptureSessionOutputContainer_create)(&m_implData->m_outputs);
                GET_CAMERA_FUNCTION(ACaptureSessionOutputContainer_add)(m_implData->m_outputs, m_implData->m_textureOutput);

                // Prepare target surface
                ANativeWindow_acquire(m_implData->m_textureWindow);
                GET_CAMERA_FUNCTION(ACameraOutputTarget_create)(m_implData->m_textureWindow, &m_implData->m_textureTarget);
                GET_CAMERA_FUNCTION(ACaptureRequest_addTarget)(m_implData->m_request, m_implData->m_textureTarget);

                // Create the session
                GET_CAMERA_FUNCTION(ACameraDevice_createCaptureSession)(m_implData->m_cameraDevice, m_implData->m_outputs,
                                                                        &sessionStateCallbacks,
                                                                        &m_implData->m_textureSession);

                // Start capturing continuously
                GET_CAMERA_FUNCTION(ACameraCaptureSession_setRepeatingRequest)(m_implData->m_textureSession,
                                                                               &captureCallbacks,
                                                                               1, &m_implData->m_request,
                                                                               nullptr);
            }

            if (eglMakeCurrent(m_implData->m_display, 0/*surface*/, 0/*surface*/, currentContext) == EGL_FALSE)
            {
                throw std::runtime_error{"Unable to restore GL context for camera texture init."};
            }

            return m_implData->m_cameraDimensions;
        });
    }

    void Camera::Impl::SetTextureOverride(void* texturePtr)
    {
        if (!m_implData->m_overrideCameraTexture)
        {
            throw std::runtime_error{"Trying to override NativeCamera Texture."};
        }
        m_implData->m_cameraOESTextureId = reinterpret_cast<uintptr_t>(texturePtr);
    }

    void Camera::Impl::UpdateCameraTexture(bgfx::TextureHandle textureHandle)
    {
        EGLContext currentContext = eglGetCurrentContext();
        if (m_implData->m_context != EGL_NO_CONTEXT)
        {
            // use the newly created shared context
            if (eglMakeCurrent(m_implData->m_display, 0/*surface*/, 0/*surface*/, m_implData->m_context) == EGL_FALSE)
            {
                throw std::runtime_error{"Unable to make current shared GL context for camera texture."};
            }
        }

        if (API_LEVEL >= 24 && !m_implData->m_overrideCameraTexture)
        {
            m_implData->m_surfaceTexture.updateTexImage();
        }

        glBindFramebuffer(GL_FRAMEBUFFER, m_implData->m_frameBufferId);
        glViewport(0, 0, m_implData->m_cameraDimensions.width, m_implData->m_cameraDimensions.height);
        glUseProgram(m_implData->m_cameraShaderProgramId);

        auto vertexPositionsUniformLocation{ glGetUniformLocation(m_implData->m_cameraShaderProgramId, "positions") };
        glUniform2fv(vertexPositionsUniformLocation, CAMERA_VERTEX_COUNT, CAMERA_VERTEX_POSITIONS);

        auto uvsUniformLocation{ glGetUniformLocation(m_implData->m_cameraShaderProgramId, "uvs") };
        glUniform2fv(uvsUniformLocation, CAMERA_UVS_COUNT, m_implData->m_cameraUVs);

        // Configure the camera texture
        auto cameraTextureUniformLocation{glGetUniformLocation(m_implData->m_cameraShaderProgramId, "cameraTexture")};
        glUniform1i(cameraTextureUniformLocation, android::OpenGLHelpers::GetTextureUnit(GL_TEXTURE0));
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_EXTERNAL_OES, m_implData->m_cameraOESTextureId);
        glBindSampler(android::OpenGLHelpers::GetTextureUnit(GL_TEXTURE0), 0);

        // Draw the quad
        glDrawArrays(GL_TRIANGLE_STRIP, 0, CAMERA_VERTEX_COUNT);

        glUseProgram(0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // bind previously bound context
        if (eglMakeCurrent(m_implData->m_display, 0/*surface*/, 0/*surface*/, currentContext) == EGL_FALSE)
        {
            throw std::runtime_error{"Unable to make current shared GL context for camera texture."};
        }

        arcana::make_task(m_implData->m_deviceContext->BeforeRenderScheduler(), arcana::cancellation::none(), [this, textureHandle] {
            bgfx::overrideInternal(textureHandle, m_implData->m_cameraRGBATextureId);
        });
    }

    void Camera::Impl::Close()
    {
        if (API_LEVEL >= 24 && !m_implData->m_overrideCameraTexture)
        {
            // Stop recording to SurfaceTexture and do some cleanup
            GET_CAMERA_FUNCTION(ACameraCaptureSession_stopRepeating)(m_implData->m_textureSession);
            GET_CAMERA_FUNCTION(ACameraCaptureSession_close)(m_implData->m_textureSession);
            GET_CAMERA_FUNCTION(ACaptureSessionOutputContainer_free)(m_implData->m_outputs);
            GET_CAMERA_FUNCTION(ACaptureSessionOutput_free)(m_implData->m_output);

            GET_CAMERA_FUNCTION(ACameraDevice_close)(m_implData->m_cameraDevice);
            GET_CAMERA_FUNCTION(ACameraManager_delete)(m_implData->m_cameraManager);

            // Capture request for SurfaceTexture
            ANativeWindow_release(m_implData->m_textureWindow);
            GET_CAMERA_FUNCTION(ACaptureRequest_free)(m_implData->m_request);
        }

        if (m_implData->m_context != EGL_NO_CONTEXT)
        {
            eglDestroyContext(m_implData->m_display, m_implData->m_context);
        }
    }
}