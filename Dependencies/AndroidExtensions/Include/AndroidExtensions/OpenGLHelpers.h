#pragma once

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <GLES3/gl3platform.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <gsl/gsl>

namespace android::OpenGLHelpers
{
    constexpr GLint GetTextureUnit(GLenum texture)
    {
        return texture - GL_TEXTURE0;
    }

    GLuint CreateShaderProgram(const char* vertShaderSource, const char* fragShaderSource);

    namespace GLTransactions
    {
        inline auto SetCapability(GLenum capability, bool isEnabled)
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

        inline auto BindFrameBuffer(GLuint frameBufferId)
        {
            GLint previousFrameBufferId;
            glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFrameBufferId);
            glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
            return gsl::finally([previousFrameBufferId]() { glBindFramebuffer(GL_FRAMEBUFFER, static_cast<GLuint>(previousFrameBufferId)); });
        }

        inline auto DepthMask(GLboolean depthMask)
        {
            GLboolean previousDepthMask;
            glGetBooleanv(GL_DEPTH_WRITEMASK, &previousDepthMask);
            glDepthMask(depthMask);
            return gsl::finally([previousDepthMask]() { glDepthMask(previousDepthMask); });
        }

        inline auto BindSampler(GLenum unit, GLuint id)
        {
            glActiveTexture(unit);
            GLint previousId;
            glGetIntegerv(GL_SAMPLER_BINDING, &previousId);
            glBindSampler(unit - GL_TEXTURE0, id);
            return gsl::finally([unit, id{ previousId }]() { glActiveTexture(unit); glBindSampler(unit - GL_TEXTURE0, id); });
        }

        inline auto MakeCurrent(EGLDisplay display, EGLSurface drawSurface, EGLSurface readSurface, EGLContext context)
        {
            EGLDisplay previousDisplay{ eglGetDisplay(EGL_DEFAULT_DISPLAY) };
            EGLSurface previousDrawSurface{ eglGetCurrentSurface(EGL_DRAW) };
            EGLSurface previousReadSurface{ eglGetCurrentSurface(EGL_READ) };
            EGLContext previousContext{ eglGetCurrentContext() };
            eglMakeCurrent(display, drawSurface, readSurface, context);
            return gsl::finally([previousDisplay, previousDrawSurface, previousReadSurface, previousContext]() { eglMakeCurrent(previousDisplay, previousDrawSurface, previousReadSurface, previousContext); });
        }
    }
}