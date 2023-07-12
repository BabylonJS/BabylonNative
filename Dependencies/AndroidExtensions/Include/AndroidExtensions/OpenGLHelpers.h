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
        inline auto MakeCurrent(EGLDisplay display, EGLSurface drawSurface, EGLSurface readSurface, EGLContext context)
        {
            EGLDisplay previousDisplay{ eglGetDisplay(EGL_DEFAULT_DISPLAY) };
            EGLSurface previousDrawSurface{ eglGetCurrentSurface(EGL_DRAW) };
            EGLSurface previousReadSurface{ eglGetCurrentSurface(EGL_READ) };
            EGLContext previousContext{ eglGetCurrentContext() };
            eglMakeCurrent(display, drawSurface, readSurface, context);
            return gsl::finally([previousDisplay, previousDrawSurface, previousReadSurface, previousContext]() { eglMakeCurrent(previousDisplay, previousDrawSurface, previousReadSurface, previousContext); });
        }

        inline auto SetStencil(uint8_t mask)
        {
            GLint previousStencilMask{};
            glGetIntegerv(GL_STENCIL_WRITEMASK, &previousStencilMask);
            glStencilMask(mask);
            return gsl::finally([previousStencilMask]() { glStencilMask(previousStencilMask); });
        }
    }
}