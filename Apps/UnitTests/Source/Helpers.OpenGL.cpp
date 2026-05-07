#include <gtest/gtest.h>
#include "Helpers.h"

#include <GLES3/gl3.h>

#include <stdexcept>

namespace Helpers
{
    // The OpenGL ExternalTexture backend imports single-sample, non-array
    // GL_TEXTURE_2D handles. glGenTextures/glTexImage2D require a current GL
    // context, which the single-threaded Graphics::Device makes current on the
    // calling thread between StartRenderingCurrentFrame/FinishRenderingCurrentFrame
    // (the same thread the tests create textures on).
    Babylon::Graphics::TextureT CreateTexture(Babylon::Graphics::DeviceT, uint32_t width, uint32_t height, uint32_t arraySize, bool renderTarget, uint32_t samples)
    {
        // The array / render-target / MSAA variants are only exercised by the
        // render-path tests, which are skipped on this backend (see CMakeLists.txt).
        if (arraySize != 1 || renderTarget || samples != 1)
        {
            throw std::runtime_error{"Helpers::CreateTexture(OpenGL): only single-sample, non-array GL_TEXTURE_2D textures are supported"};
        }

        GLint previousBinding = 0;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousBinding);

        GLuint texture = 0;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        // Allocate a single, texture-complete mip level. The pixel contents are
        // never read back by the enabled tests; only the queried dimensions and
        // internal format matter.
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(previousBinding));

        EXPECT_EQ(glGetError(), static_cast<GLenum>(GL_NO_ERROR));

        return texture;
    }

    void DestroyTexture(Babylon::Graphics::TextureT texture)
    {
        GLuint handle = texture;
        glDeleteTextures(1, &handle);
    }

    Babylon::Graphics::TextureT CreateTextureArrayWithData(Babylon::Graphics::DeviceT, uint32_t, uint32_t, const Color*, uint32_t)
    {
        throw std::runtime_error{"not implemented"};
    }

    std::vector<uint8_t> ReadPixels(const Babylon::Graphics::PlatformInfo&, Babylon::Graphics::TextureT, uint32_t, uint32_t)
    {
        throw std::runtime_error{"not implemented"};
    }
}
