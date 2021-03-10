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
#include <android/log.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <arcana/threading/task.h>
#include <arcana/threading/dispatcher.h>
#include <Babylon/JsRuntimeScheduler.h>
#include <GraphicsImpl.h>
#include <arcana/threading/task_schedulers.h>

using namespace android;
using namespace android::global;

namespace Babylon::Plugins::Internal
{
    const int PERMISSION_REQUEST_ID{ 8436 };

    struct CameraInterfaceAndroid : public CameraInterface
    {
        CameraInterfaceAndroid(Napi::Env env, uint32_t width, uint32_t height, bool frontCamera);
        virtual ~CameraInterfaceAndroid();
        void UpdateCameraTexture(bgfx::TextureHandle textureHandle) override;

        static constexpr char CAMERA_VERT_SHADER[]{ R"(#version 300 es
            precision highp float;
            out vec2 cameraFrameUV;
            void main() {
                cameraFrameUV = vec2(gl_VertexID&1, (gl_VertexID &2)>>1) * 2.f;
                gl_Position = vec4(cameraFrameUV * 2.f - 1.f, 0.0, 1.0);
            }
        )"};

        static constexpr char CAMERA_FRAG_SHADER[]{ R"(#version 300 es
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

        Graphics::Impl& m_graphicsImpl;
        JsRuntimeScheduler m_runtimeScheduler;

        uint32_t width;
        uint32_t height;
        ACameraManager* cameraManager{};
        ACameraDevice* cameraDevice{};
        ACameraOutputTarget* textureTarget{};
        ACaptureRequest* request{};
        ANativeWindow* textureWindow{};
        ACameraCaptureSession* textureSession{};
        ACaptureSessionOutput* textureOutput{};
        ACaptureSessionOutput* output{};
        ACaptureSessionOutputContainer* outputs{};
        GLuint cameraOESTextureId{};
        GLuint cameraRGBATextureId{};
        GLuint cameraShaderProgramId{};
        GLuint clearFrameBufferId{};

        android::graphics::SurfaceTexture surfaceTexture;
        android::view::Surface surface;

        std::atomic<bool> initComplete{};

        constexpr GLint GetTextureUnit(GLenum texture)
        {
            return texture - GL_TEXTURE0;
        }

        GLuint LoadShader(GLenum shader_type, const char* shader_source)
        {
            GLuint shader{ glCreateShader(shader_type) };
            if (!shader)
            {
                throw std::runtime_error{ "Failed to create shader" };
            }

            glShaderSource(shader, 1, &shader_source, nullptr);
            glCompileShader(shader);
            GLint compileStatus{ GL_FALSE };
            glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);

            if (compileStatus != GL_TRUE)
            {
                GLint infoLogLength{};

                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
                if (!infoLogLength)
                {
                    throw std::runtime_error{ "Unknown error compiling shader" };
                }

                std::string infoLog;
                infoLog.resize(static_cast<size_t>(infoLogLength));
                glGetShaderInfoLog(shader, infoLogLength, nullptr, infoLog.data());
                glDeleteShader(shader);
                //throw std::runtime_error("Error compiling shader: " + infoLog);
                __android_log_write(ANDROID_LOG_ERROR, "BabylonNative", infoLog.c_str());
            }

            return shader;
        }

        GLuint CreateShaderProgram(const char* vertShaderSource, const char* fragShaderSource)
        {
            GLuint vertShader{ LoadShader(GL_VERTEX_SHADER, vertShaderSource) };
            GLuint fragShader{ LoadShader(GL_FRAGMENT_SHADER, fragShaderSource) };

            GLuint program{ glCreateProgram() };
            if (!program)
            {
                throw std::runtime_error{ "Failed to create shader program" };
            }

            glAttachShader(program, vertShader);
            glAttachShader(program, fragShader);

            glLinkProgram(program);
            GLint linkStatus{ GL_FALSE };
            glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

            glDetachShader(program, vertShader);
            glDeleteShader(vertShader);
            glDetachShader(program, fragShader);
            glDeleteShader(fragShader);

            if (linkStatus != GL_TRUE)
            {
                GLint infoLogLength{};
                glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
                if (!infoLogLength)
                {
                    throw std::runtime_error{ "Unknown error linking shader program" };
                }

                std::string infoLog;
                infoLog.resize(static_cast<size_t>(infoLogLength));
                glGetProgramInfoLog(program, infoLogLength, nullptr, infoLog.data());
                glDeleteProgram(program);
                //throw std::runtime_error("Error linking shader program: " + infoLog);
                __android_log_write(ANDROID_LOG_ERROR, "BabylonNative", infoLog.c_str());
            }

            return program;
        }
    };


    std::string CameraInterfaceAndroid::getCamId(bool frontCamera)
    {
        ACameraIdList *cameraIds = nullptr;
        ACameraManager_getCameraIdList(cameraManager, &cameraIds);

        std::string backId;

        for (int i = 0; i < cameraIds->numCameras; ++i)
        {
            const char *id = cameraIds->cameraIds[i];

            ACameraMetadata *metadataObj;
            ACameraManager_getCameraCharacteristics(cameraManager, id, &metadataObj);

            ACameraMetadata_const_entry lensInfo = {};
            ACameraMetadata_getConstEntry(metadataObj, ACAMERA_LENS_FACING, &lensInfo);

            auto facing = static_cast<acamera_metadata_enum_android_lens_facing_t>(lensInfo.data.u8[0]);

            // Found a back-facing camera?
            if (facing == (frontCamera?ACAMERA_LENS_FACING_FRONT:ACAMERA_LENS_FACING_BACK))
            {
                backId = id;
                break;
            }
        }

        ACameraManager_deleteCameraIdList(cameraIds);
        return backId;
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

    arcana::task<void, std::exception_ptr> CheckCameraPermissionAsync()
    {
        auto task{ arcana::task_from_result<std::exception_ptr>() };

        // Check if permissions are already granted.
        if (!GetAppContext().checkSelfPermission(ManifestPermission::CAMERA()))
        {
            // Register for the permission callback request.
            arcana::task_completion_source<void, std::exception_ptr> permissionTcs;
            auto permissionTicket
            {
                AddRequestPermissionsResultCallback(
                [permissionTcs](int32_t requestCode, const std::vector<std::string>& /*permissionList*/, const std::vector<int32_t>& results) mutable
                {
                    // Check if this is our permission request ID.
                    if (requestCode == PERMISSION_REQUEST_ID)
                    {
                        // If the permission is found and granted complete the task.
                        if (results[0] == 0 /* PackageManager.PERMISSION_GRANTED */)
                        {
                            permissionTcs.complete();
                            return;
                        }

                        // Permission was denied.  Complete the task with an error.
                        permissionTcs.complete(arcana::make_unexpected(make_exception_ptr(std::runtime_error{"Camera permission not acquired successfully"})));
                    }
                })
            };

            // Kick off the permission check request, and set the task for our caller to wait on.
            GetCurrentActivity().requestPermissions(ManifestPermission::CAMERA(), PERMISSION_REQUEST_ID);
            task = permissionTcs.as_task().then(arcana::inline_scheduler, arcana::cancellation::none(), [ticket{ std::move(permissionTicket) }](){
                return;
            });
        }

        return task;
    }

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
        : m_graphicsImpl{Graphics::Impl::GetFromJavaScript(env)}
        , m_runtimeScheduler{JsRuntime::GetFromJavaScript(env)}
        , width{width}
        , height{height}
    {
        CheckCameraPermissionAsync().then(arcana::inline_scheduler, arcana::cancellation::none(), [this, frontCamera]()
        {
            arcana::make_task(m_graphicsImpl.BeforeRenderScheduler(), arcana::cancellation::none(), [this] {
                cameraOESTextureId = GenerateOESTexture();
                glGenTextures(1, &cameraRGBATextureId);
                glBindTexture(GL_TEXTURE_2D, cameraRGBATextureId);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->width, this->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glBindTexture(GL_TEXTURE_2D, 0);

                glGenFramebuffers(1, &clearFrameBufferId);
                glBindFramebuffer(GL_FRAMEBUFFER, clearFrameBufferId);
                //auto bindFrameBufferTransaction{ GLTransactions::BindFrameBuffer(clearFrameBufferId) };
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, cameraRGBATextureId, 0);

                glBindFramebuffer(GL_FRAMEBUFFER, 0);

                cameraShaderProgramId = CreateShaderProgram(CAMERA_VERT_SHADER, CAMERA_FRAG_SHADER);
            }).then(m_runtimeScheduler, arcana::cancellation::none(), [this, frontCamera] {
                surfaceTexture.initWithTexture(cameraOESTextureId);
                surface.initWithSurfaceTexture(surfaceTexture);

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

                initComplete = true;
            });
        });
    }

    CameraInterfaceAndroid::~CameraInterfaceAndroid()
    {
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
    }

    void CameraInterfaceAndroid::UpdateCameraTexture(bgfx::TextureHandle textureHandle)
    {
        arcana::make_task(m_graphicsImpl.BeforeRenderScheduler(), arcana::cancellation::none(), [this, textureHandle] {
            if (initComplete && cameraRGBATextureId) {
                surfaceTexture.updateTexture();

                glBindFramebuffer(GL_FRAMEBUFFER, clearFrameBufferId);
                glViewport(0, 0, width, height);
                glUseProgram(cameraShaderProgramId);

                // Configure the camera texture
                auto cameraTextureUniformLocation{glGetUniformLocation(cameraShaderProgramId, "cameraTexture")};
                glUniform1i(cameraTextureUniformLocation, GetTextureUnit(GL_TEXTURE0));
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_EXTERNAL_OES, cameraOESTextureId);
                glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glBindSampler(0, 0);

                // Draw the quad
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);

                glUseProgram(0);
                glBindFramebuffer(GL_FRAMEBUFFER, 0);

                bgfx::overrideInternal(textureHandle, cameraRGBATextureId);
            }
        });
    }

    CameraInterface* CameraInterface::CreateInterface(Napi::Env env, uint32_t width, uint32_t height, bool frontCamera)
    {
        return new CameraInterfaceAndroid(env, width, height, frontCamera);
    }
}