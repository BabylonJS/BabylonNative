#include <XR.h>
#include <XRHelpers.h>

#include <assert.h>
#include <optional>
#include <sstream>
#include <chrono>
#include <arcana/threading/task.h>
#include <arcana/threading/dispatcher.h>
#include <thread>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

#include <AndroidExtensions/Globals.h>
#include <AndroidExtensions/JavaWrappers.h>

#include <android/native_window.h>
#include <android/log.h>
#include <arcore_c_api.h>

#include <gsl/gsl>

#define GLM_FORCE_RADIANS 1
#define GLM_ENABLE_EXPERIMENTAL
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/quaternion.hpp>
#include <arcana/threading/task_schedulers.h>

using namespace android;
using namespace android::global;

extern ANativeWindow* g_xrWindowPtr;
bool g_isXRActive = false;

namespace xr
{
    // Permission request ID used to uniquely identify our request in the callback when calling requestPermissions.
    const int PERMISSION_REQUEST_ID{ 8435 };

    struct System::Impl
    {
        Impl(const std::string& /*applicationName*/)
        {
        }

        bool IsInitialized() const
        {
            return true;
        }

        bool TryInitialize()
        {
            return true;
        }
    };

    namespace
    {
        constexpr GLfloat VERTEX_POSITIONS[]{ -1.0f, -1.0f, +1.0f, -1.0f, -1.0f, +1.0f, +1.0f, +1.0f };
        constexpr size_t VERTEX_COUNT{ std::size(VERTEX_POSITIONS) / 2 };

        constexpr char CAMERA_VERT_SHADER[]{ R"(#version 300 es
            precision highp float;
            uniform vec2 vertexPositions[4];
            uniform vec2 cameraFrameUVs[4];
            out vec2 cameraFrameUV;
            void main() {
                gl_Position = vec4(vertexPositions[gl_VertexID], 0.0, 1.0);
                cameraFrameUV = cameraFrameUVs[gl_VertexID];
            }
        )"};

        constexpr char BABYLON_VERT_SHADER[]{ R"(#version 300 es
            precision highp float;
            uniform vec2 vertexPositions[4];
            out vec2 babylonUV;
            void main() {
                gl_Position = vec4(vertexPositions[gl_VertexID], 0.0, 1.0);
                babylonUV = vec2(gl_Position.x + 1.0, gl_Position.y + 1.0) * 0.5;
            }
        )"};

        constexpr char CAMERA_FRAG_SHADER[]{ R"(#version 300 es
            #extension GL_OES_EGL_image_external_essl3 : require
            precision mediump float;
            in vec2 cameraFrameUV;
            uniform samplerExternalOES cameraTexture;
            // Location 0 is GL_COLOR_ATTACHMENT0, which in turn is the babylonTexture
            layout(location = 0) out vec4 oFragColor;
            void main() {
                oFragColor = texture(cameraTexture, cameraFrameUV);
            }
        )"};

        constexpr char BABYLON_FRAG_SHADER[]{ R"(#version 300 es
            #extension GL_OES_EGL_image_external_essl3 : require
            precision mediump float;
            in vec2 babylonUV;
            uniform sampler2D babylonTexture;
            out vec4 oFragColor;
            void main() {
                oFragColor = texture(babylonTexture, babylonUV);
            }
        )"};

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
                throw std::runtime_error("Error compiling shader: " + infoLog);
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
                throw std::runtime_error("Error linking shader program: " + infoLog);
            }

            return program;
        }

        constexpr GLint GetTextureUnit(GLenum texture)
        {
            return texture - GL_TEXTURE0;
        }

        namespace GLTransactions
        {
            auto SetCapability(GLenum capability, bool isEnabled)
            {
                const auto setCapability{ [capability](bool isEnabled)
                {
                    if (isEnabled)
                    {
                        glEnable(capability);
                    }
                    else
                    {
                        glDisable(capability);
                    }
                }};

                const auto wasEnabled{ glIsEnabled(capability) };
                setCapability(isEnabled);
                return gsl::finally([wasEnabled, setCapability]() { setCapability(wasEnabled); });
            }

            auto BindFrameBuffer(GLuint frameBufferId)
            {
                GLint previousFrameBufferId;
                glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFrameBufferId);
                glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
                return gsl::finally([previousFrameBufferId]() { glBindFramebuffer(GL_FRAMEBUFFER, static_cast<GLuint>(previousFrameBufferId)); });
            }

            auto DepthMask(GLboolean depthMask)
            {
                GLboolean previousDepthMask;
                glGetBooleanv(GL_DEPTH_WRITEMASK, &previousDepthMask);
                glDepthMask(depthMask);
                return gsl::finally([previousDepthMask]() { glDepthMask(previousDepthMask); });
            }

            auto BindSampler(GLenum unit, GLuint id)
            {
                glActiveTexture(unit);
                GLint previousId;
                glGetIntegerv(GL_SAMPLER_BINDING, &previousId);
                glBindSampler(unit - GL_TEXTURE0, id);
                return gsl::finally([unit, id{ previousId }]() { glActiveTexture(unit); glBindSampler(unit - GL_TEXTURE0, id); });
            }

            auto MakeCurrent(EGLDisplay display, EGLSurface drawSurface, EGLSurface readSurface, EGLContext context)
            {
                EGLDisplay previousDisplay{ eglGetDisplay(EGL_DEFAULT_DISPLAY) };
                EGLSurface previousDrawSurface{ eglGetCurrentSurface(EGL_DRAW) };
                EGLSurface previousReadSurface{ eglGetCurrentSurface(EGL_READ) };
                EGLContext previousContext{ eglGetCurrentContext() };
                eglMakeCurrent(display, drawSurface, readSurface, context);
                return gsl::finally([previousDisplay, previousDrawSurface, previousReadSurface, previousContext]() { eglMakeCurrent(previousDisplay, previousDrawSurface, previousReadSurface, previousContext); });
            }
        }

        bool CheckARCoreInstallStatus(bool requestInstall)
        {
            ArInstallStatus install_status;
            ArStatus installStatus{ ArCoreApk_requestInstall(GetEnvForCurrentThread(), GetCurrentActivity(), requestInstall, &install_status) };
            return installStatus == AR_SUCCESS && install_status == AR_INSTALL_STATUS_INSTALLED;
        }

        arcana::task<void, std::exception_ptr> CheckAndInstallARCoreAsync()
        {
            auto task{ arcana::task_from_result<std::exception_ptr>() };

            // Check if ARCore is already installed.
            if (!CheckARCoreInstallStatus(false))
            {
                arcana::task_completion_source<void, std::exception_ptr> installTcs{};

                // Add a resume callback, which will check if ARCore has been successfully installed upon app resume.
                auto resumeTicket{AddResumeCallback([installTcs]() mutable {
                    if (!CheckARCoreInstallStatus(false))
                    {
                        // ARCore not installed, throw an error.
                        std::ostringstream message;
                        message << "ARCore not installed.";
                        installTcs.complete(arcana::make_unexpected(make_exception_ptr(std::runtime_error{message.str()})));
                    }
                    else
                    {
                        // ARCore installed successfully, complete the promise.
                        installTcs.complete();
                    }
                })};

                // Kick off the install request, and set the task for our caller to wait on.
                CheckARCoreInstallStatus(true);
                task = installTcs.as_task().then(arcana::inline_scheduler, arcana::cancellation::none(), [resumeTicket = std::move(resumeTicket)](){
                    return;
                });
            }

            return task;
        }

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
                            std::ostringstream message;
                            message << "Camera permission not acquired successfully";
                            permissionTcs.complete(arcana::make_unexpected(make_exception_ptr(std::runtime_error{message.str()})));
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
    }

    struct System::Session::Impl
    {
        const System::Impl& SystemImpl;
        std::vector<Frame::View> ActiveFrameViews{ {} };
        std::vector<Frame::InputSource> InputSources{};
        std::vector<Frame::Plane> Planes{};
        std::vector<Frame::Mesh> Meshes{};
        std::vector<FeaturePoint> FeaturePointCloud{};
        float DepthNearZ{ DEFAULT_DEPTH_NEAR_Z };
        float DepthFarZ{ DEFAULT_DEPTH_FAR_Z };
        bool PlaneDetectionEnabled{ false };
        bool FeaturePointCloudEnabled{ false };

        Impl(System::Impl& systemImpl, void* graphicsContext)
            : SystemImpl{ systemImpl }
            , parentContext{reinterpret_cast<EGLContext>(graphicsContext) }
            , pauseTicket{AddPauseCallback([this]() { this->PauseSession(); }) }
            , resumeTicket{AddResumeCallback([this]() { this->ResumeSession(); })}
        {
            g_isXRActive = true;
        }

        ~Impl()
        {
            g_isXRActive = false;

            if (isInitialized)
            {
                Planes.clear();
                CleanupAnchor(nullptr);
                CleanupFrameTrackables();
                ArPose_destroy(cameraPose);
                ArPose_destroy(tempPose);
                ArHitResult_destroy(hitResult);
                ArHitResultList_destroy(hitResultList);
                ArTrackableList_destroy(trackableList);
                ArFrame_destroy(frame);
                ArSession_destroy(session);

                glDeleteTextures(1, &cameraTextureId);
                glDeleteProgram(cameraShaderProgramId);
                glDeleteProgram(babylonShaderProgramId);
                glDeleteFramebuffers(1, &clearFrameBufferId);

                DestroyDisplayResources();
            }

            if (context)
            {
                eglDestroyContext(display, context);
            }
        }

        void Initialize()
        {
            {
                display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

                EGLint attributes[]
                {
                    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT_KHR,

                    EGL_BLUE_SIZE, 8,
                    EGL_GREEN_SIZE, 8,
                    EGL_RED_SIZE, 8,
                    EGL_ALPHA_SIZE, 8,

                    EGL_DEPTH_SIZE, 16,
                    EGL_STENCIL_SIZE, 8,

                    EGL_NONE
                };

                EGLint numConfigs{};
                if (!eglChooseConfig(display, attributes, &config, 1, &numConfigs))
                {
                    throw std::runtime_error{"Failed to choose EGL config."};
                }

                eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

                EGLint contextAttributes[]
                {
                    EGL_CONTEXT_MAJOR_VERSION_KHR, 3,
                    EGL_CONTEXT_MINOR_VERSION_KHR, 0,

                    EGL_NONE
                };

                context = eglCreateContext(display, config, parentContext, contextAttributes);
            }

            // Generate a texture id for the camera texture (ARCore will allocate the texture itself)
            {
                glGenTextures(1, &cameraTextureId);
                glBindTexture(GL_TEXTURE_EXTERNAL_OES, cameraTextureId);
                glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glBindTexture(GL_TEXTURE_EXTERNAL_OES, 0);
            }

            // Create the shader program used for drawing the full screen quad that is the camera frame + Babylon render texture
            cameraShaderProgramId = CreateShaderProgram(CAMERA_VERT_SHADER, CAMERA_FRAG_SHADER);
            babylonShaderProgramId = CreateShaderProgram(BABYLON_VERT_SHADER, BABYLON_FRAG_SHADER);

            // Create the ARCore ArSession
            {
                ArStatus status{ ArSession_create(GetEnvForCurrentThread(), GetAppContext(), &session) };
                if (status != ArStatus::AR_SUCCESS)
                {
                    std::ostringstream message;
                    message << "Failed to create ArSession with status: " << status;
                    throw std::runtime_error{ message.str() };
                }
            }

            // Create a frame buffer used for clearing the color texture
            glGenFramebuffers(1, &clearFrameBufferId);

            // Create the ARCore ArFrame (this gets reused each time we query for the latest frame)
            ArFrame_create(session, &frame);

            // Create the ARCore ArPose that tracks camera position
            ArPose_create(session, nullptr, &cameraPose);

            // Create the hit result list, and hit result.
            ArHitResultList_create(session, &hitResultList);
            ArHitResult_create(session, &hitResult);

            // Create the trackable list used to process planes.
            ArTrackableList_create(session, &trackableList);

            // Create the reusable ARCore ArPose used for short term operations
            // (i.e. pulling out hit test results, and updating anchors)
            ArPose_create(session, nullptr, &tempPose);

            // Set the texture ID that should be used for the camera frame
            ArSession_setCameraTextureName(session, static_cast<uint32_t>(cameraTextureId));

            // Start the ArSession
            {
                ArStatus status{ ArSession_resume(session) };
                if (status != ArStatus::AR_SUCCESS)
                {
                    std::ostringstream message;
                    message << "Failed to start ArSession with status: " << status;
                    throw std::runtime_error{ message.str() };
                }
            }

            isInitialized = true;
        }

        std::unique_ptr<Session::Frame> GetNextFrame(bool& shouldEndSession, bool& shouldRestartSession, std::function<void(void* texturePointer)> deletedTextureCallback)
        {
            if (!isInitialized)
            {
                Initialize();
            }

            if (g_xrWindowPtr != window)
            {
                window = g_xrWindowPtr;

                if (surface)
                {
                    eglDestroySurface(display, surface);
                    surface = nullptr;
                }

                if (window)
                {
                    surface = eglCreateWindowSurface(display, config, g_xrWindowPtr, nullptr);
                }
            }

            shouldEndSession = sessionEnded;
            shouldRestartSession = false;

            // Update the ArSession to get a new frame
            ArSession_update(session, frame);

            ArCamera* camera{};
            ArFrame_acquireCamera(session, frame, &camera);

            {
                // Get the current pose of the device
                ArCamera_getDisplayOrientedPose(session, camera, cameraPose);

                // The raw pose is exactly 7 floats: 4 for the orientation quaternion, and 3 for the position vector
                float rawPose[7]{};
                ArPose_getPoseRaw(session, cameraPose, rawPose);

                // Set the orientation and position
                RawToPose(rawPose, ActiveFrameViews[0].Space.Pose);
            }

            // Get the current window dimensions
            size_t width{}, height{};
            if (window)
            {
                int32_t _width{ANativeWindow_getWidth(window)};
                int32_t _height{ANativeWindow_getHeight(window)};
                if (_width > 0 && _height > 0)
                {
                    width = static_cast<size_t>(_width);
                    height = static_cast<size_t>(_height);
                }
            }

            // min size for a RT is 8x8. eglQuerySurface may return a width or height of 0 which will assert in bgfx
            width = std::max(width, size_t(8));
            height = std::max(height, size_t(8));

            // Check whether the dimensions have changed
            if ((ActiveFrameViews[0].ColorTextureSize.Width != width || ActiveFrameViews[0].ColorTextureSize.Height != height) && width && height)
            {
                DestroyDisplayResources(deletedTextureCallback);

                int rotation{ GetAppContext().getSystemService<android::view::WindowManager>().getDefaultDisplay().getRotation() };

                // Update the width and height of the display with ARCore (this is used to adjust the UVs for the camera texture so we can draw a portion of the camera frame that matches the size of the UI element displaying it)
                ArSession_setDisplayGeometry(session, rotation, static_cast<int32_t>(width), static_cast<int32_t>(height));

                // Allocate and store the render texture
                {
                    GLuint colorTextureId{};
                    glGenTextures(1, &colorTextureId);
                    glBindTexture(GL_TEXTURE_2D, colorTextureId);
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                    glBindTexture(GL_TEXTURE_2D, 0);
                    ActiveFrameViews[0].ColorTexturePointer = reinterpret_cast<void *>(colorTextureId);
                    ActiveFrameViews[0].ColorTextureFormat = TextureFormat::RGBA8_SRGB;
                    ActiveFrameViews[0].ColorTextureSize = {width, height};
                }

                // Allocate and store the depth texture
                {
                    GLuint depthTextureId{};
                    glGenTextures(1, &depthTextureId);
                    glBindTexture(GL_TEXTURE_2D, depthTextureId);
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8_OES, width, height, 0, GL_DEPTH_STENCIL_OES, GL_UNSIGNED_INT_24_8_OES, nullptr);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glBindTexture(GL_TEXTURE_2D, 0);
                    ActiveFrameViews[0].DepthTexturePointer = reinterpret_cast<void*>(depthTextureId);
                    ActiveFrameViews[0].DepthTextureFormat = TextureFormat::D24S8;
                    ActiveFrameViews[0].DepthTextureSize = {width, height};
                }

                // Bind the color and depth texture to the clear color frame buffer
                auto bindFrameBufferTransaction{ GLTransactions::BindFrameBuffer(clearFrameBufferId) };
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, static_cast<GLuint>(reinterpret_cast<uintptr_t>(ActiveFrameViews[0].ColorTexturePointer)), 0);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, static_cast<GLuint>(reinterpret_cast<uintptr_t>(ActiveFrameViews[0].DepthTexturePointer)), 0);
            }

            int32_t geometryChanged{0};
            ArFrame_getDisplayGeometryChanged(session, frame, &geometryChanged);

            // Check whether the projection matrix needs to be updated
            if (geometryChanged || ActiveFrameViews[0].DepthNearZ != DepthNearZ || ActiveFrameViews[0].DepthFarZ != DepthFarZ)
            {
                // Get the current projection matrix
                ArCamera_getProjectionMatrix(session, camera, DepthNearZ, DepthFarZ, ActiveFrameViews[0].ProjectionMatrix.data());
            }

            ActiveFrameViews[0].DepthNearZ = DepthNearZ;
            ActiveFrameViews[0].DepthFarZ = DepthFarZ;

            if (geometryChanged)
            {
                // Transform the UVs for the vertex positions given the current display size
                ArFrame_transformCoordinates2d(
                    session, frame, AR_COORDINATES_2D_OPENGL_NORMALIZED_DEVICE_COORDINATES,
                    VERTEX_COUNT, VERTEX_POSITIONS, AR_COORDINATES_2D_TEXTURE_NORMALIZED, CameraFrameUVs);
            }

            ArCamera_release(camera);

            // Draw the camera texture to the Babylon render texture, but only if the session has started providing AR frames.
            int64_t frameTimestamp{};
            ArFrame_getTimestamp(session, frame, &frameTimestamp);
            if (frameTimestamp)
            {
                // Draw the camera texture to the color texture and clear the depth texture before handing them off to Babylon.
                auto bindFrameBufferTransaction{ GLTransactions::BindFrameBuffer(clearFrameBufferId) };
                auto cullFaceTransaction{ GLTransactions::SetCapability(GL_CULL_FACE, false) };
                auto depthTestTransaction{ GLTransactions::SetCapability(GL_DEPTH_TEST, false) };
                auto blendTransaction{ GLTransactions::SetCapability(GL_BLEND, false) };
                auto depthMaskTransaction{ GLTransactions::DepthMask(GL_FALSE) };

                glUseProgram(cameraShaderProgramId);

                // Configure the quad vertex positions
                auto vertexPositionsUniformLocation{ glGetUniformLocation(cameraShaderProgramId, "vertexPositions") };
                glUniform2fv(vertexPositionsUniformLocation, VERTEX_COUNT, VERTEX_POSITIONS);

                // Configure the camera texture
                auto cameraTextureUniformLocation{ glGetUniformLocation(cameraShaderProgramId, "cameraTexture") };
                glUniform1i(cameraTextureUniformLocation, GetTextureUnit(GL_TEXTURE0));
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_EXTERNAL_OES, cameraTextureId);
                auto bindSamplerTransaction{ GLTransactions::BindSampler(GL_TEXTURE0, 0) };

                // Configure the camera frame UVs
                auto cameraFrameUVsUniformLocation{ glGetUniformLocation(cameraShaderProgramId, "cameraFrameUVs") };
                glUniform2fv(cameraFrameUVsUniformLocation, VERTEX_COUNT, CameraFrameUVs);

                // Draw the quad
                glDrawArrays(GL_TRIANGLE_STRIP, 0, VERTEX_COUNT);

                glUseProgram(0);
            }

            return std::make_unique<Session::Frame>(*this);
        }

        void RequestEndSession()
        {
            // Note the end session has been requested, and respond to the request in the next call to GetNextFrame
            sessionEnded = true;

            if (surface)
            {
                eglDestroySurface(display, surface);
            }
        }

        Size GetWidthAndHeightForViewIndex(size_t /*viewIndex*/) const
        {
            // Return a valid (non-zero) size, but otherwise it doesn't matter as the render texture created from this isn't currently used
            return {1,1};
        }

        void DrawFrame()
        {
            // Draw the Babylon render texture to the display, but only if the session has started providing AR frames.
            int64_t frameTimestamp{};
            ArFrame_getTimestamp(session, frame, &frameTimestamp);
            if (frameTimestamp && surface)
            {
                auto surfaceTransaction{ GLTransactions::MakeCurrent(eglGetDisplay(EGL_DEFAULT_DISPLAY), surface, surface, context) };

                auto bindFrameBufferTransaction{ GLTransactions::BindFrameBuffer(0) };
                auto cullFaceTransaction{ GLTransactions::SetCapability(GL_CULL_FACE, false) };
                auto depthTestTransaction{ GLTransactions::SetCapability(GL_DEPTH_TEST, false) };
                auto blendTransaction{ GLTransactions::SetCapability(GL_BLEND, false) };
                auto depthMaskTransaction{ GLTransactions::DepthMask(GL_FALSE) };

                glViewport(0, 0, ActiveFrameViews[0].ColorTextureSize.Width, ActiveFrameViews[0].ColorTextureSize.Height);
                glUseProgram(babylonShaderProgramId);

                // Configure the quad vertex positions
                auto vertexPositionsUniformLocation{ glGetUniformLocation(babylonShaderProgramId, "vertexPositions") };
                glUniform2fv(vertexPositionsUniformLocation, VERTEX_COUNT, VERTEX_POSITIONS);

                // Configure the babylon render texture
                auto babylonTextureUniformLocation{ glGetUniformLocation(babylonShaderProgramId, "babylonTexture") };
                glUniform1i(babylonTextureUniformLocation, GetTextureUnit(GL_TEXTURE0));
                glActiveTexture(GL_TEXTURE0);
                auto babylonTextureId{ (GLuint)(size_t)ActiveFrameViews[0].ColorTexturePointer };
                glBindTexture(GL_TEXTURE_2D, babylonTextureId);
                auto bindSamplerTransaction{ GLTransactions::BindSampler(GL_TEXTURE0, 0) };

                // Draw the quad
                glDrawArrays(GL_TRIANGLE_STRIP, 0, VERTEX_COUNT);

                // Present to the screen
                // NOTE: For a yet to be determined reason, bgfx is also doing an eglSwapBuffers when running in the regular Android Babylon Native Playground playground app.
                //       The "double" eglSwapBuffers causes rendering issues, so until we figure out this issue, comment out this line while testing in the regular playground app.
                eglSwapBuffers(eglGetCurrentDisplay(), eglGetCurrentSurface(EGL_DRAW));

                glUseProgram(0);
            }
        }

        void GetHitTestResults(std::vector<HitResult>& filteredResults, xr::Ray offsetRay, xr::HitTestTrackableType validHitTestTypes)
        {
            if (!IsTracking())
            {
                return;
            }

            // Push the camera orientation into a glm quaternion.
            glm::quat cameraOrientationQuaternion
            {
                ActiveFrameViews[0].Space.Pose.Orientation.W,
                ActiveFrameViews[0].Space.Pose.Orientation.X,
                ActiveFrameViews[0].Space.Pose.Orientation.Y,
                ActiveFrameViews[0].Space.Pose.Orientation.Z
            };

            // Pull out the direction from the offset ray into a GLM Vector3.
            glm::vec3 direction{ offsetRay.Direction.X, offsetRay.Direction.Y, offsetRay.Direction.Z };

            // Multiply the camera rotation quaternion by the direction vector to calculate the direction vector in viewer space.
            glm::vec3 cameraOrientedDirection{cameraOrientationQuaternion * glm::normalize(direction)};
            float cameraOrientedDirectionArray[3]{ cameraOrientedDirection.x, cameraOrientedDirection.y, cameraOrientedDirection.z };

            // Convert the origin to camera space by multiplying the origin by the rotation quaternion, then adding that to the
            // position of the camera.
            glm::vec3 offsetOrigin{ offsetRay.Origin.X, offsetRay.Origin.Y, offsetRay.Origin.Z };
            offsetOrigin = cameraOrientationQuaternion * offsetOrigin;

            // Pull out the origin composited from the offsetRay and camera position into a float array.
            float hitTestOrigin[3]
            {
                ActiveFrameViews[0].Space.Pose.Position.X + offsetOrigin.x,
                ActiveFrameViews[0].Space.Pose.Position.Y + offsetOrigin.y,
                ActiveFrameViews[0].Space.Pose.Position.Z + offsetOrigin.z
            };

            // Perform a hit test and process the results.
            ArFrame_hitTestRay(session, frame, hitTestOrigin, cameraOrientedDirectionArray, hitResultList);

            // Iterate over the results and pull out only those that match the desired TrackableType.  For now we are limiting results to
            // just hits against the Plane, and further scoping that to Poses that are contained in the polygon of the detected mesh.
            // This is equivalent to XRHitTestTrackableType.mesh (https://immersive-web.github.io/hit-test/#hit-test-trackable-type-enum).
            int32_t size{};
            ArHitResultList_getSize(session, hitResultList, &size);
            for (int i = 0; i < size; i++)
            {
                ArTrackableType trackableType{};
                ArTrackable* trackable;

                bool hitTestResultValid{false};
                ArHitResultList_getItem(session, hitResultList, i, hitResult);
                ArHitResult_acquireTrackable(session, hitResult, &trackable);
                ArTrackable_getType(session, trackable, &trackableType);
                if (trackableType == AR_TRACKABLE_PLANE)
                {
                    // If we are only hit testing against planes then mark the hit test as valid otherwise check
                    // if the hit result is inside the plane mesh.
                    if ((validHitTestTypes & xr::HitTestTrackableType::PLANE) != xr::HitTestTrackableType::NONE)
                    {
                        hitTestResultValid = true;
                    }
                    else if ((validHitTestTypes & xr::HitTestTrackableType::MESH) != xr::HitTestTrackableType::NONE)
                    {
                        int32_t isPoseInPolygon{};
                        ArHitResult_getHitPose(session, hitResult, tempPose);
                        ArPlane_isPoseInPolygon(session, reinterpret_cast<ArPlane*>(trackable), tempPose, &isPoseInPolygon);
                        hitTestResultValid = isPoseInPolygon != 0;
                    }
                }
                else if (trackableType == AR_TRACKABLE_POINT && (validHitTestTypes & xr::HitTestTrackableType::POINT) != xr::HitTestTrackableType::NONE)
                {
                    // Hit a feature point, which is valid for this hit test source.
                    hitTestResultValid = true;
                }

                if (hitTestResultValid)
                {
                    float rawPose[7]{};
                    ArHitResult_getHitPose(session, hitResult, tempPose);
                    ArPose_getPoseRaw(session, tempPose, rawPose);
                    HitResult hitResult{};
                    RawToPose(rawPose, hitResult.Pose);

                    hitResult.NativeTrackable = reinterpret_cast<NativeTrackablePtr>(trackable);
                    filteredResults.push_back(hitResult);
                    frameTrackables.push_back(trackable);
                }
            }
        }

        // Clean up all ArCore trackables owned by the current frame, this should be called once per frame.
        void CleanupFrameTrackables()
        {
            for (ArTrackable* trackable : frameTrackables)
            {
                ArTrackable_release(trackable);
            }

            frameTrackables.clear();
        }

        Anchor CreateAnchor(Pose pose, NativeTrackablePtr trackable)
        {
            // First translate the passed in pose to something usable by ArCore.
            ArPose* arPose{};
            float rawPose[7]{};
            PoseToRaw(rawPose, pose);
            ArPose_create(session, rawPose, &arPose);

            // Create the actual anchor. If a trackable was passed in (from a hit test result) create the
            // anchor against the tracakble. Otherwise create it against the session.
            ArAnchor* arAnchor{};
            auto trackableObj{ reinterpret_cast<ArTrackable*>(trackable) };
            if (trackableObj)
            {
                ArTrackable_acquireNewAnchor(session, trackableObj, arPose, &arAnchor);
            }
            else
            {
                ArSession_acquireNewAnchor(session, arPose, &arAnchor);
            }

            // Clean up the temp pose.
            ArPose_destroy(arPose);

            // Store the anchor the vector tracking currently allocated anchors, and pass back the result.
            arCoreAnchors.push_back(arAnchor);
            return {pose, reinterpret_cast<NativeAnchorPtr>(arAnchor)};
        }

        void UpdateAnchor(xr::Anchor& anchor)
        {
            // First check if the anchor still exists, if not then mark the anchor as no longer valid.
            auto arAnchor{ reinterpret_cast<ArAnchor*>(anchor.NativeAnchor) };
            if (arAnchor == nullptr)
            {
                anchor.IsValid = false;
                return;
            }

            ArTrackingState trackingState{};
            ArAnchor_getTrackingState(session, arAnchor, &trackingState);

            // If tracking then update the pose, if paused then skip the update, if stopped then
            // mark this anchor as no longer valid, as it will never again be tracked by ArCore.
            if (trackingState == AR_TRACKING_STATE_TRACKING)
            {
                ArAnchor_getPose(session, arAnchor, tempPose);
                float rawPose[7]{};
                ArPose_getPoseRaw(session, tempPose, rawPose);
                RawToPose(rawPose, anchor.Pose);
            }
            else if (trackingState == AR_TRACKING_STATE_STOPPED)
            {
                anchor.IsValid = false;
            }
        }

        void DeleteAnchor(xr::Anchor& anchor)
        {
            // If this anchor has not already been deleted, then detach it from the current AR session,
            // and clean up its state in memory.
            if (anchor.NativeAnchor != nullptr)
            {
                auto arAnchor{ reinterpret_cast<ArAnchor*>(anchor.NativeAnchor) };
                ArAnchor_detach(session, arAnchor);
                CleanupAnchor(arAnchor);
                anchor.NativeAnchor = nullptr;
            }
        }

        void CleanupAnchor(ArAnchor* arAnchor)
        {
            // Iterate over the list of anchors if arAnchor is null then clean up all anchors
            // otherwise clean up only the target anchor and return.
            auto anchorIter{ arCoreAnchors.begin() };
            while (anchorIter != arCoreAnchors.end())
            {
                if (arAnchor == nullptr || arAnchor == *anchorIter)
                {
                    ArAnchor_release(*anchorIter);
                    anchorIter = arCoreAnchors.erase(anchorIter);

                    if (arAnchor != nullptr)
                    {
                        return;
                    }
                }
                else
                {
                    anchorIter++;
                }
            }
        }

        void UpdatePlanes(std::vector<Frame::Plane::Identifier>& updatedPlanes, std::vector<Frame::Plane::Identifier>& deletedPlanes)
        {
            if (!PlaneDetectionEnabled)
            {
                return;
            }

            // First check if any existing planes have been subsumed by another plane, if so add them to the list of deleted planes
            CheckForSubsumedPlanes(deletedPlanes);

            // Next check for updated planes, and update their pose and polygon or create a new plane if it does not yet exist.
            ArFrame_getUpdatedTrackables(session, frame, AR_TRACKABLE_PLANE, trackableList);
            int32_t size{};
            ArTrackableList_getSize(session, trackableList, &size);
            for (int i = 0; i < size; i++)
            {
                // Get the plane.
                ArPlane* planeTrackable{};
                {
                    ArTrackable* trackable{};
                    ArTrackableList_acquireItem(session, trackableList, i, &trackable);
                    planeTrackable = reinterpret_cast<ArPlane*>(trackable);
                }

                // Check if this plane has been subsumed. If so skip it as we are about to delete this plane.
                ArPlane* subsumingPlane{};
                ArPlane_acquireSubsumedBy(session, planeTrackable, &subsumingPlane);
                if (subsumingPlane != nullptr)
                {
                    ArTrackable_release(reinterpret_cast<ArTrackable*>(planeTrackable));
                    ArTrackable_release(reinterpret_cast<ArTrackable*>(subsumingPlane));
                    continue;
                }

                // Get the center pose.
                float rawPose[7]{};
                ArPlane_getCenterPose(session, planeTrackable, tempPose);
                ArPose_getPoseRaw(session, tempPose, rawPose);

                // Dynamically allocate the polygon vector, and fill it in.
                int32_t polygonSize;
                ArPlane_getPolygonSize(session, planeTrackable, &polygonSize);
                planePolygonBuffer.clear();
                planePolygonBuffer.resize(polygonSize);
                ArPlane_getPolygon(session, planeTrackable, planePolygonBuffer.data());

                // Update the existing plane if it exists, otherwise create a new plane, and add it to our list of planes.
                auto planeIterator{ planeMap.find(planeTrackable) };
                if (planeIterator != planeMap.end())
                {
                    UpdatePlane(updatedPlanes, GetPlaneByID(planeIterator->second), rawPose, planePolygonBuffer, polygonSize);
                    ArTrackable_release(reinterpret_cast<ArTrackable*>(planeTrackable));
                }
                else
                {
                    // This is a new plane, create it and initialize its values.
                    Planes.emplace_back();
                    auto& plane{ Planes.back() };
                    planeMap.insert({planeTrackable, plane.ID});
                    UpdatePlane(updatedPlanes, plane, rawPose, planePolygonBuffer, polygonSize);
                }
            }
        }

        void UpdateFeaturePointCloud()
        {
            if (!FeaturePointCloudEnabled)
            {
                return;
            }

            // Get the feature point cloud from ArCore.
            ArPointCloud *pointCloud{};
            int32_t numberOfPoints{};
            const int32_t* pointCloudIDs{};
            const float *pointCloudData{};
            ArStatus status{ ArFrame_acquirePointCloud(session, frame, &pointCloud) };

            if (status != AR_SUCCESS)
            {
                FeaturePointCloud.clear();
                return;
            }

            try
            {
                ArPointCloud_getNumberOfPoints(session, pointCloud, &numberOfPoints);
                ArPointCloud_getData(session, pointCloud, &pointCloudData);
                ArPointCloud_getPointIds(session, pointCloud, &pointCloudIDs);

                FeaturePointCloud.resize(numberOfPoints);
                for (int32_t i = 0; i < numberOfPoints; i++)
                {
                    FeaturePointCloud.emplace_back();
                    auto& featurePoint{ FeaturePointCloud.back() };
                    int32_t dataIndex{ i * 4 };

                    // Grab the position and confidence value from the point cloud.
                    // Reflect the point across the Z axis, as we want to report this
                    // value in camera space.
                    featurePoint.X = pointCloudData[dataIndex];
                    featurePoint.Y = pointCloudData[dataIndex + 1];
                    featurePoint.Z = -1 * pointCloudData[dataIndex + 2];
                    featurePoint.ConfidenceValue = pointCloudData[dataIndex + 3];

                    // Check to see if this point ID exists in our point cloud mapping if not add it to the map.
                    const int32_t id{ pointCloudIDs[i] };
                    auto featurePointIterator = featurePointIDMap.find(id);
                    if (featurePointIterator != featurePointIDMap.end())
                    {
                        featurePoint.ID = featurePointIterator->second;
                    }
                    else
                    {
                        featurePoint.ID = nextFeaturePointID++;
                        featurePointIDMap.insert({id, featurePoint.ID});
                    }
                }
            }
            catch (std::exception)
            {
                // Release the point cloud to free its memory.
                ArPointCloud_release(pointCloud);
                throw;
            }

            // Release the point cloud to free its memory.
            ArPointCloud_release(pointCloud);
        }

        Frame::Plane& GetPlaneByID(Frame::Plane::Identifier planeID)
        {
            // Loop over the plane vector and find the correct plane.
            for (Frame::Plane& plane : Planes)
            {
                if (plane.ID == planeID)
                {
                    return plane;
                }
            }

            throw std::runtime_error{"Tried to get non-existent plane."};
        }

        /**
         * Checks whether the AR camera is currently tracking.
         **/
        bool IsTracking()
        {
            ArCamera* camera{};
            ArTrackingState trackingState{};
            ArFrame_acquireCamera(session, frame, &camera);
            ArCamera_getTrackingState(session, camera, &trackingState);
            return trackingState == ArTrackingState::AR_TRACKING_STATE_TRACKING;
        }

    private:
        bool isInitialized{false};
        bool sessionEnded{false};
        std::vector<ArTrackable*> frameTrackables{};
        std::vector<ArAnchor*> arCoreAnchors{};
        std::vector<float> planePolygonBuffer{};
        std::unordered_map<ArPlane*, Frame::Plane::Identifier> planeMap{};
        std::unordered_map<int32_t, FeaturePoint::Identifier> featurePointIDMap{};
        FeaturePoint::Identifier nextFeaturePointID{};

        ANativeWindow* window{};
        EGLDisplay display{};
        EGLConfig config{};
        EGLint format{};
        EGLContext parentContext{};
        EGLContext context{};
        EGLSurface surface{};

        GLuint cameraShaderProgramId{};
        GLuint babylonShaderProgramId{};
        GLuint cameraTextureId{};
        GLuint clearFrameBufferId{};

        ArSession* session{};
        ArFrame* frame{};
        ArPose* cameraPose{};
        ArPose* tempPose{};
        ArHitResultList* hitResultList{};
        ArHitResult* hitResult{};
        ArTrackableList* trackableList{};

        float CameraFrameUVs[VERTEX_COUNT * 2]{};

        AppStateChangedCallbackTicket pauseTicket;
        AppStateChangedCallbackTicket resumeTicket;

        void PauseSession()
        {
            if (session)
            {
                ArSession_pause(session);
            }
        }

        void ResumeSession()
        {
            if (session)
            {
                ArSession_resume(session);
            }
        }

        void DestroyDisplayResources(std::function<void(void* texturePointer)> deletedTextureCallback = [](void*){})
        {
            if (ActiveFrameViews[0].ColorTexturePointer)
            {
                auto colorTextureId{ static_cast<GLuint>(reinterpret_cast<uintptr_t>(ActiveFrameViews[0].ColorTexturePointer)) };
                glDeleteTextures(1, &colorTextureId);
                deletedTextureCallback(ActiveFrameViews[0].ColorTexturePointer);
            }

            if (ActiveFrameViews[0].DepthTexturePointer)
            {
                auto depthTextureId{ static_cast<GLuint>(reinterpret_cast<uintptr_t>(ActiveFrameViews[0].DepthTexturePointer)) };
                glDeleteTextures(1, &depthTextureId);
            }

            ActiveFrameViews[0] = {};
        }

        void PoseToRaw(float rawPose[], const Pose& pose)
        {
            rawPose[0] = pose.Orientation.X;
            rawPose[1] = pose.Orientation.Y;
            rawPose[2] = pose.Orientation.Z;
            rawPose[3] = pose.Orientation.W;
            rawPose[4] = pose.Position.X;
            rawPose[5] = pose.Position.Y;
            rawPose[6] = pose.Position.Z;
        }

        void RawToPose(const float rawPose[], Pose& pose)
        {
            pose.Orientation.X = rawPose[0];
            pose.Orientation.Y = rawPose[1];
            pose.Orientation.Z = rawPose[2];
            pose.Orientation.W = rawPose[3];
            pose.Position.X = rawPose[4];
            pose.Position.Y = rawPose[5];
            pose.Position.Z = rawPose[6];
        }

        /**
         * Checks whether this plane has been subsumed (i.e. no longer needed), and adds it to the vector if so.
         **/
        void CheckForSubsumedPlanes(std::vector<Frame::Plane::Identifier>& subsumedPlanes)
        {
            auto planeMapIterator{ planeMap.begin() };
            while (planeMapIterator != planeMap.end())
            {
                auto [arPlane, planeID]{ *planeMapIterator };

                // Check if the plane has been subsumed, and if we should stop tracking it.
                ArPlane* subsumingPlane{};
                ArPlane_acquireSubsumedBy(session, arPlane, &subsumingPlane);

                // Plane has been subsumed, stop tracking it explicitly.
                if (subsumingPlane != nullptr)
                {
                    subsumedPlanes.push_back(planeID);

                    auto& plane{ GetPlaneByID(planeID) };
                    plane.Polygon.clear();
                    plane.PolygonSize = 0;

                    planeMapIterator = planeMap.erase(planeMapIterator);
                    ArTrackable_release(reinterpret_cast<ArTrackable*>(arPlane));
                    ArTrackable_release(reinterpret_cast<ArTrackable*>(subsumingPlane));
                }
                else
                {
                    planeMapIterator++;
                }
            }
        }

        void UpdatePlane(std::vector<Frame::Plane::Identifier>& updatedPlanes, Frame::Plane& plane, const float rawPose[], std::vector<float>& newPolygon, size_t polygonSize)
        {
            // Grab the new center
            Pose newCenter{};
            RawToPose(rawPose, newCenter);

            // Plane was not actually updated return.
            if (!CheckIfPlaneWasUpdated(plane, newPolygon, newCenter))
            {
                return;
            }

            // Update the center pose.
            plane.Center = newCenter;

            // Swap the old polygon with the new one.
            plane.Polygon.swap(newPolygon);

            // Set the polygon size, and format.
            plane.PolygonSize = polygonSize / 2;
            plane.PolygonFormat = PolygonFormat::XZ;
            updatedPlanes.push_back(plane.ID);
        }
    };

    struct System::Session::Frame::Impl
    {
        Impl(Session::Impl& sessionImpl)
            : sessionImpl{sessionImpl}
        {
        }

        Session::Impl& sessionImpl;
    };

    System::Session::Frame::Frame(Session::Impl& sessionImpl)
        : Views{ sessionImpl.ActiveFrameViews }
        , InputSources{ sessionImpl.InputSources }
        , Planes{ sessionImpl.Planes }
        , Meshes { sessionImpl.Meshes }
        , FeaturePointCloud{ sessionImpl.FeaturePointCloud }
        , UpdatedSceneObjects{}
        , RemovedSceneObjects{}
        , UpdatedPlanes{}
        , RemovedPlanes{}
        , UpdatedMeshes{}
        , RemovedMeshes{}
        , IsTracking{sessionImpl.IsTracking()}
        , m_impl{ std::make_unique<Session::Frame::Impl>(sessionImpl) }
    {
        if (IsTracking)
        {
            m_impl->sessionImpl.UpdatePlanes(UpdatedPlanes, RemovedPlanes);
            m_impl->sessionImpl.UpdateFeaturePointCloud();
        }
    }

    void System::Session::Frame::GetHitTestResults(std::vector<HitResult>& filteredResults, xr::Ray offsetRay, xr::HitTestTrackableType trackableTypes) const
    {
        m_impl->sessionImpl.GetHitTestResults(filteredResults, offsetRay, trackableTypes);
    }

    Anchor System::Session::Frame::CreateAnchor(Pose pose, NativeTrackablePtr trackable) const
    {
        return m_impl->sessionImpl.CreateAnchor(pose, trackable);
    }

    void System::Session::Frame::UpdateAnchor(xr::Anchor& anchor) const
    {
        m_impl->sessionImpl.UpdateAnchor(anchor);
    }

    void System::Session::Frame::DeleteAnchor(xr::Anchor& anchor) const
    {
        m_impl->sessionImpl.DeleteAnchor(anchor);
    }

    System::Session::Frame::SceneObject& System::Session::Frame::GetSceneObjectByID(System::Session::Frame::SceneObject::Identifier /*sceneObjectID*/) const
    {
        throw std::runtime_error{"Scene object detection is not supported on current platform."};
    }

    System::Session::Frame::Plane& System::Session::Frame::GetPlaneByID(System::Session::Frame::Plane::Identifier planeID) const
    {
        return m_impl->sessionImpl.GetPlaneByID(planeID);
    }

    System::Session::Frame::Mesh& System::Session::Frame::GetMeshByID(System::Session::Frame::Mesh::Identifier /*meshID*/) const
    {
        throw std::runtime_error{"Mesh detection is not supported on current platform."};
    }

    System::Session::Frame::~Frame()
    {
        m_impl->sessionImpl.CleanupFrameTrackables();
        m_impl->sessionImpl.DrawFrame();
    }

    System::System(const char* appName)
        : m_impl{ std::make_unique<System::Impl>(appName) }
    {}

    System::~System() {}

    bool System::IsInitialized() const
    {
        return m_impl->IsInitialized();
    }

    bool System::TryInitialize()
    {
        return m_impl->TryInitialize();
    }

    arcana::task<bool, std::exception_ptr> System::IsSessionSupportedAsync(SessionType sessionType)
    {
        // Currently only AR is supported on Android
        if (sessionType == SessionType::IMMERSIVE_AR)
        {
            // Spin up a background thread to own the polling check.
            arcana::task_completion_source<bool, std::exception_ptr> tcs;
            std::thread([tcs]() mutable
            {
                // Query ARCore to check if AR sessions are supported.
                // If not yet installed then poll supported status up to 100 times over 20 seconds.
                for (int i = 0; i < 100; i++)
                {
                    ArAvailability arAvailability{};
                    ArCoreApk_checkAvailability(GetEnvForCurrentThread(), GetAppContext(), &arAvailability);
                    switch (arAvailability)
                    {
                        case AR_AVAILABILITY_SUPPORTED_APK_TOO_OLD:
                        case AR_AVAILABILITY_SUPPORTED_INSTALLED:
                        case AR_AVAILABILITY_SUPPORTED_NOT_INSTALLED:
                            tcs.complete(true);
                            break;
                        case AR_AVAILABILITY_UNKNOWN_CHECKING:
                            std::this_thread::sleep_for(std::chrono::milliseconds(200));
                            break;
                        default:
                            tcs.complete(false);
                            break;
                    }

                    if (tcs.completed())
                    {
                        break;
                    }
                }

                if (!tcs.completed())
                {
                    tcs.complete(false);
                }
            }).detach();

            return tcs.as_task();
        }

        // VR and inline sessions are not supported at this time.
        return arcana::task_from_result<std::exception_ptr>(false);
    }

    arcana::task<std::shared_ptr<System::Session>, std::exception_ptr> System::Session::CreateAsync(System& system, void* graphicsDevice, std::function<void*()> windowProvider)
    {
        // First perform the ARCore installation check, request install if not yet installed.
        return CheckAndInstallARCoreAsync().then(arcana::inline_scheduler, arcana::cancellation::none(), []()
        {
            // Next check for camera permissions, and request if not already granted.
            return CheckCameraPermissionAsync();
        }).then(arcana::inline_scheduler, arcana::cancellation::none(), [&system, graphicsDevice, windowProvider{ std::move(windowProvider) }]()
        {
            // Finally if the previous two tasks succeed, start the AR session.
            return std::make_shared<System::Session>(system, graphicsDevice, windowProvider);
        });
    }

    System::Session::Session(System& system, void* graphicsDevice, std::function<void*()>)
        : m_impl{ std::make_unique<System::Session::Impl>(*system.m_impl, graphicsDevice) }
    {}

    System::Session::~Session()
    {
    }

    std::unique_ptr<System::Session::Frame> System::Session::GetNextFrame(bool& shouldEndSession, bool& shouldRestartSession, std::function<void(void* texturePointer)> deletedTextureCallback)
    {
        return m_impl->GetNextFrame(shouldEndSession, shouldRestartSession, deletedTextureCallback);
    }

    void System::Session::RequestEndSession()
    {
        m_impl->RequestEndSession();
    }

    Size System::Session::GetWidthAndHeightForViewIndex(size_t viewIndex) const
    {
        return m_impl->GetWidthAndHeightForViewIndex(viewIndex);
    }

    void System::Session::SetDepthsNearFar(float depthNear, float depthFar)
    {
        m_impl->DepthNearZ = depthNear;
        m_impl->DepthFarZ = depthFar;
    }

    void System::Session::SetPlaneDetectionEnabled(bool enabled) const
    {
        m_impl->PlaneDetectionEnabled = enabled;
    }

    bool System::Session::TrySetFeaturePointCloudEnabled(bool enabled) const
    {
        // Point cloud system not yet supported.
        m_impl->FeaturePointCloudEnabled = enabled;
        return enabled;
    }

    bool System::Session::TrySetPreferredPlaneDetectorOptions(const xr::GeometryDetectorOptions& /*options*/)
    {
        // TODO
        return false;
    }

    bool System::Session::TrySetMeshDetectorEnabled(const bool /*enabled*/)
    {
        // TODO
        return false;
    }

    bool System::Session::TrySetPreferredMeshDetectorOptions(const xr::GeometryDetectorOptions& /*options*/)
    {
        // TODO
        return false;
    }
}
