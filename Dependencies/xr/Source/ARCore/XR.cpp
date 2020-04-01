#include <XR.h>

#include <assert.h>
#include <optional>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl3.h>
#include <EGL/egl.h>

#include <android/native_window.h>
#include <android/log.h>
#include <arcore_c_api.h>

#define GLM_FORCE_RADIANS 1
#define GLM_ENABLE_EXPERIMENTAL
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/quaternion.hpp>

namespace xr
{
    class System::Impl
    {
    public:
        Impl(const std::string& applicationName)
        {}

        bool IsInitialized() const
        {
            return (m_env != nullptr && m_appContext != nullptr);
        }

        bool TryInitialize(JNIEnv* env, jobject appContext)
        {
            m_env = env;
            m_appContext = appContext;

            // Perhaps call eglGetCurrentSurface to get the render surface *before* XR render loop starts and changes to rendering to an FBO?
            return true;
        }

        JNIEnv* Env() const
        {
            return m_env;
        }

        const jobject& AppContext() const
        {
            return m_appContext;
        }

    private:
        JNIEnv* m_env{};
        jobject m_appContext{};
    };

    namespace
    {
        const GLfloat vertexPositions[] = {-1.0f, -1.0f, +1.0f, -1.0f, -1.0f, +1.0f, +1.0f, +1.0f, };

        constexpr char QUAD_VERT_SHADER[] = R"(#version 300 es
            precision highp float;
            uniform vec2 vertexPositions[4];
            uniform vec2 cameraFrameUVs[4];
            out vec2 cameraFrameUV;
            out vec2 babylonUV;
            void main() {
                gl_Position = vec4(vertexPositions[gl_VertexID], 0.0, 1.0);
                cameraFrameUV = cameraFrameUVs[gl_VertexID];
                babylonUV = vec2(gl_Position.x + 1.0, gl_Position.y + 1.0) * 0.5;
            }
        )";

        constexpr char QUAD_FRAG_SHADER[] = R"(#version 300 es
            #extension GL_OES_EGL_image_external_essl3 : require
            precision mediump float;
            in vec2 cameraFrameUV;
            in vec2 babylonUV;
            uniform samplerExternalOES cameraTexture;
            uniform sampler2D babylonTexture;
            out vec4 oFragColor;
            void main() {
                vec4 cameraColor = texture(cameraTexture, cameraFrameUV);
                vec4 babylonColor = texture(babylonTexture, babylonUV);
                oFragColor = mix(cameraColor, babylonColor, babylonColor.a);
            }
        )";

        GLuint LoadShader(GLenum shader_type, const char* shader_source)
        {
            GLuint shader = glCreateShader(shader_type);
            if (!shader)
            {
                throw std::runtime_error{ "Failed to create shader" };
            }

            glShaderSource(shader, 1, &shader_source, nullptr);
            glCompileShader(shader);
            GLint compileStatus = GL_FALSE;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);

            if (compileStatus != GL_TRUE)
            {
                GLint infoLogLength = 0;

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

        GLuint CreateShaderProgram()
        {
            GLuint vertShader = LoadShader(GL_VERTEX_SHADER, QUAD_VERT_SHADER);
            GLuint fragShader = LoadShader(GL_FRAGMENT_SHADER, QUAD_FRAG_SHADER);

            GLuint program = glCreateProgram();
            if (!program)
            {
                throw std::runtime_error{ "Failed to create shader program" };
            }

            glAttachShader(program, vertShader);
            glAttachShader(program, fragShader);

            glLinkProgram(program);
            GLint linkStatus = GL_FALSE;
            glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

            if (linkStatus != GL_TRUE)
            {
                GLint infoLogLength = 0;
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
    }

    class System::Session::Impl
    {
    public:
        const System::Impl& HmdImpl;
        std::vector<Frame::View> ActiveFrameViews{ {} };
        std::vector<Frame::InputSource> InputSources;
        float DepthNearZ{ DEFAULT_DEPTH_NEAR_Z };
        float DepthFarZ{ DEFAULT_DEPTH_FAR_Z };
        bool SessionEnded{ false };

        GLuint shader_program_;
        GLint attribute_vertices_;
        GLint attribute_uvs_;
        GLint uniform_texture_;
        GLuint vertexArray;
        GLuint vertexBuffer;

        GLuint cameraTextureId{};

        ArSession* session{};
        ArFrame* frame{};
        ArPose* pose{};

        float transformed_uvs[8];
        bool uvs_initialized{false};

        Impl(System::Impl& hmdImpl, void* graphicsContext)
            : HmdImpl{ hmdImpl }
        {
            // Note: graphicsContext is an EGLContext

            // Get the width and height of the current surface
            size_t width, height;
            {
                EGLDisplay display = eglGetCurrentDisplay();
                EGLSurface surface = eglGetCurrentSurface(EGL_DRAW);
                EGLint _width, _height;
                eglQuerySurface(eglGetDisplay(EGL_DEFAULT_DISPLAY), surface, EGL_WIDTH, &_width);
                eglQuerySurface(eglGetDisplay(EGL_DEFAULT_DISPLAY), surface, EGL_HEIGHT, &_height);
                width = static_cast<size_t>(_width);
                height = static_cast<size_t >(_height);
            }

            // Allocate and store the render texture
            {
                GLuint colorTextureId;
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
                GLuint depthTextureId;
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

            // Generate a texture id for the camera texture (ARCore will allocate the texture itself)
            {
                glGenTextures(1, &cameraTextureId);
                glBindTexture(GL_TEXTURE_EXTERNAL_OES, cameraTextureId);
                glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glBindTexture(GL_TEXTURE_EXTERNAL_OES, 0);
            }

            // Create the shader program used for drawing the full screen quad that is the camera frame + Babylon render texture
            shader_program_ = CreateShaderProgram();

            // Create the ARCore ArSession
            {
                ArStatus status = ArSession_create(hmdImpl.Env(), hmdImpl.AppContext(), &session);
                if (status != ArStatus::AR_SUCCESS)
                {
                    throw std::runtime_error{ "Failed to create ArSession with status " + status };
                }
            }

            // Create the ARCore ArFrame (this gets reused each time we query for the latest frame)
            ArFrame_create(session, &frame);

            // Create the ARCore ArPose (this gets reused for each frame as well)
            ArPose_create(session, nullptr, &pose);

            // Initialize the width and height of the display with ARCore (this is used to adjust the UVs for the camera texture so we can draw a portion of the camera frame that matches the size of the UI element displaying it)
            ArSession_setDisplayGeometry(session, 0, static_cast<int32_t>(width), static_cast<int32_t>(height));

            // Start the ArSession
            {
                ArStatus status = ArSession_resume(session);
                if (status != ArStatus::AR_SUCCESS)
                {
                    throw std::runtime_error{ "Failed to start ArSession with status: " + status };
                }
            }
        }

        ~Impl()
        {
            ArPose_destroy(pose);
            ArFrame_destroy(frame);
            ArSession_destroy(session);
        }

        std::unique_ptr<System::Session::Frame> GetNextFrame(bool& shouldEndSession, bool& shouldRestartSession)
        {
            shouldEndSession = SessionEnded;
            shouldRestartSession = false;

            // Call ArSession_setCameraTextureName and ArSession_update
            ArSession_setCameraTextureName(session, static_cast<uint32_t>(cameraTextureId));
            ArSession_update(session, frame);

            return std::make_unique<Frame>(*this);
        }

        void RequestEndSession()
        {
            // Call ArSession_destroy and ArFrame_destroy, or maybe do this in the destructor
            SessionEnded = true;
        }

        Size GetWidthAndHeightForViewIndex(size_t viewIndex) const
        {
            // Just return the window/surface width/height
            return {1,1};
        }
    };

    System::Session::Frame::Frame(Session::Impl& sessionImpl)
        : Views{ sessionImpl.ActiveFrameViews }
        , m_sessionImpl{ sessionImpl }
        , InputSources{ sessionImpl.InputSources}
    {
        Views[0].DepthNearZ = sessionImpl.DepthNearZ;
        Views[0].DepthFarZ = sessionImpl.DepthFarZ;

        /*Views[0].Space.Position = {0, 0, -10};
        // https://quaternions.online/
        Views[0].Space.Orientation = {0.707f, 0, -.707f, 0};
        Views[0].FieldOfView.AngleLeft = 0.4;
        Views[0].FieldOfView.AngleRight = 0.4;
        Views[0].FieldOfView.AngleUp = 0.4;
        Views[0].FieldOfView.AngleDown = 0.4;*/

        // Call ArFrame_acquireCamera 
        // Call ArCamera_getPose and ArCamera_getProjectionMatrix and mash state into the single View (from Views)
        // Call ArCamera_release

        ArCamera* camera;
        ArFrame_acquireCamera(sessionImpl.session, sessionImpl.frame, &camera);

        {
            float rawPose[7];
            //ArCamera_getPose(sessionImpl.session, camera, sessionImpl.pose);
            ArCamera_getDisplayOrientedPose(sessionImpl.session, camera, sessionImpl.pose);
            ArPose_getPoseRaw(sessionImpl.session, sessionImpl.pose, rawPose);

            Views[0].Space.Orientation = {rawPose[0], rawPose[1], rawPose[2], rawPose[3]};
            Views[0].Space.Position = {rawPose[4], rawPose[5], rawPose[6]};
        }

        {
            glm::mat4 projection_mat;
            ArCamera_getProjectionMatrix(sessionImpl.session, camera, Views[0].DepthNearZ, Views[0].DepthFarZ, glm::value_ptr(projection_mat));

            float a = projection_mat[0][0];
            float b = projection_mat[1][1];

            float aspect_ratio = b / a;

            float RAD2DEG = 180.0f / 3.14159265358979323846f;
            //float fov = RAD2DEG * (2.0f * std::atan(1.0f / b));
            float fov = std::atan(1.0f / b);

            Views[0].FieldOfView.AngleDown = -(Views[0].FieldOfView.AngleUp = fov);
            Views[0].FieldOfView.AngleLeft = -(Views[0].FieldOfView.AngleRight = fov * aspect_ratio);

            // TODO: Seems like projection matrix is not being reconstructed correctly from this in NativeXR.cpp#56
        }

        ArTrackingState camera_tracking_state;
        ArCamera_getTrackingState(sessionImpl.session, camera, &camera_tracking_state);

        if (camera_tracking_state == ArTrackingState::AR_TRACKING_STATE_TRACKING)
        {
            __android_log_write(ANDROID_LOG_DEBUG, "BabylonNative", "Tracking");

            int32_t geometry_changed = 0;
            ArFrame_getDisplayGeometryChanged(sessionImpl.session, sessionImpl.frame, &geometry_changed);
            if (geometry_changed != 0 || !sessionImpl.uvs_initialized)
            {
                ArFrame_transformCoordinates2d(
                        sessionImpl.session, sessionImpl.frame, AR_COORDINATES_2D_OPENGL_NORMALIZED_DEVICE_COORDINATES,
                        4, vertexPositions, AR_COORDINATES_2D_TEXTURE_NORMALIZED,
                        sessionImpl.transformed_uvs);
                sessionImpl.uvs_initialized = true;
            }
        }

        ArCamera_release(camera);
    }

    System::Session::Frame::~Frame()
    {
        // Suppress rendering if the camera did not produce the first frame yet.
        // This is to avoid drawing possible leftover data from previous sessions if
        // the texture is reused.
        int64_t frame_timestamp;
        ArFrame_getTimestamp(m_sessionImpl.session, m_sessionImpl.frame, &frame_timestamp);
        if (frame_timestamp != 0)
        {
/*
            TODO cg: bind gl context used for rendering
            auto success = eglMakeCurrent(m_sessionImpl.Display, m_sessionImpl.Surface, m_sessionImpl.Surface, m_sessionImpl.RenderContext);
*/
            GLint drawFboId;
            glGetIntegerv(GL_FRAMEBUFFER_BINDING, &drawFboId);
            GLfloat old_clearColor[4];
            glGetFloatv(GL_COLOR_CLEAR_VALUE, old_clearColor);
            GLboolean old_glCullFace = glIsEnabled(GL_CULL_FACE);
            GLboolean old_glDepthTest = glIsEnabled(GL_DEPTH_TEST);
            GLboolean old_glBlend = glIsEnabled(GL_BLEND);
            GLboolean old_glDepthMask;
            glGetBooleanv(GL_DEPTH_WRITEMASK, &old_glDepthMask);
            GLint old_glBlendFunc;
            glGetIntegerv(GL_BLEND_SRC_ALPHA, &old_glBlendFunc);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, Views[0].ColorTextureSize.Width, Views[0].ColorTextureSize.Height);

            glClearColor(0, 1, 0, 1);
            glClear(GL_COLOR_BUFFER_BIT);

            glDisable(GL_CULL_FACE);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glUseProgram(m_sessionImpl.shader_program_);
            glDepthMask(GL_FALSE);

            auto uniform_texture_ = glGetUniformLocation(m_sessionImpl.shader_program_, "cameraTexture");
            glUniform1i(uniform_texture_, 0);
            glActiveTexture(GL_TEXTURE0);
            GLint old_texture0Binding;
            glGetIntegerv(GL_TEXTURE_BINDING_EXTERNAL_OES, &old_texture0Binding);
            glBindTexture(GL_TEXTURE_EXTERNAL_OES, m_sessionImpl.cameraTextureId);

            auto uniform_positions = glGetUniformLocation(m_sessionImpl.shader_program_, "vertexPositions");
            glUniform2fv(uniform_positions, 4, vertexPositions);

            auto uniform_uvs = glGetUniformLocation(m_sessionImpl.shader_program_, "cameraFrameUVs");
            glUniform2fv(uniform_uvs, 4, m_sessionImpl.transformed_uvs);

            auto uniform_texture_babylon = glGetUniformLocation(m_sessionImpl.shader_program_, "babylonTexture");
            if (uniform_texture_babylon >= 0)
            {
                glUniform1i(uniform_texture_babylon, 1);
                glActiveTexture(GL_TEXTURE1);
                auto texId = (GLuint) (size_t) Views[0].ColorTexturePointer;
                GLint old_texture1Binding;
                glGetIntegerv(GL_TEXTURE_BINDING_2D, &old_texture1Binding);
                glBindTexture(GL_TEXTURE_2D, texId);
            }

            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        //eglSwapBuffers(m_sessionImpl.Display, m_sessionImpl.Surface);

            glUseProgram(0);
            //glDepthMask(GL_TRUE);
            glClearColor(old_clearColor[0], old_clearColor[1], old_clearColor[2], old_clearColor[3]);
            old_glCullFace ?
            glEnable(GL_CULL_FACE) :
            glDisable(GL_CULL_FACE);
            old_glDepthTest ?
            glEnable(GL_DEPTH_TEST) :
            glDisable(GL_DEPTH_TEST);
            old_glBlend ?
            glEnable(GL_BLEND) :
            glDisable(GL_BLEND);
            old_glDepthMask ?
            glDepthMask(GL_TRUE) :
            glDepthMask(GL_FALSE);
            glBlendFunc(GL_SRC_ALPHA, old_glBlendFunc);
            glBindFramebuffer(GL_FRAMEBUFFER, drawFboId);


            /* TODO CG: set back original gl context with eglMakeCurrent*/
        }
    }

    System::System(const char* appName)
        : m_impl{ std::make_unique<System::Impl>(appName) }
    {}

    System::~System() {}

    bool System::IsInitialized() const
    {
        return m_impl->IsInitialized();
    }

    bool System::TryInitialize(JNIEnv* env, jobject appContext)
    {
        return m_impl->TryInitialize(env, appContext);
    }

    System::Session::Session(System& system, void* graphicsDevice)
        : m_impl{ std::make_unique<System::Session::Impl>(*system.m_impl, graphicsDevice) }
    {}

    System::Session::~Session()
    {
        // Free textures
    }

    std::unique_ptr<System::Session::Frame> System::Session::GetNextFrame(bool& shouldEndSession, bool& shouldRestartSession)
    {
        return m_impl->GetNextFrame(shouldEndSession, shouldRestartSession);
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
}
