#include <gtest/gtest.h>
#include "Helpers.h"

#include <GLES3/gl3.h>

#include <stdexcept>
#include <thread>

namespace Helpers
{
    // The OpenGL ExternalTexture backend imports single-sample, non-array
    // GL_TEXTURE_2D handles. glGenTextures/glTexImage2D require a current GL
    // context, which the single-threaded Graphics::Device makes current on the
    // calling thread between StartRenderingCurrentFrame/FinishRenderingCurrentFrame
    // (the same thread the tests create textures on).
    Babylon::Graphics::TextureT CreateTexture(Babylon::Graphics::DeviceT, uint32_t width, uint32_t height, uint32_t arraySize, bool renderTarget, uint32_t samples)
    {
        // OpenGL needs no special allocation for a texture that will be attached to a
        // framebuffer, so renderTarget only has to be recorded as usage below. Array and
        // multisample images would need a different texture target; the tests that ask for
        // those are skipped on this backend (see CMakeLists.txt).
        if (arraySize != 1 || samples != 1)
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

        // Hand back a reference-counted image, mirroring what the D3D and Metal helpers return.
        // The release callback owns the GL name, so the texture dies with the last reference and
        // DestroyTexture just drops the caller's own.
        //
        // glDeleteTextures needs a current GL context, which this backend only has on the thread
        // that created the texture. ExternalTexture is copyable, so the last reference can be
        // dropped anywhere; the contract is that embedders marshal, and this assertion turns a
        // violation into a test failure instead of a silently leaked texture.
        const auto creatingThread = std::this_thread::get_id();
        return Babylon::Graphics::GL::Texture::Create(texture, width, height, GL_RGBA8,
            renderTarget ? Babylon::Graphics::GL::Texture::Usage::RenderTarget
                         : Babylon::Graphics::GL::Texture::Usage::Sampled,
            [creatingThread](unsigned int name) {
                EXPECT_EQ(creatingThread, std::this_thread::get_id());
                glDeleteTextures(1, &name);
            });
    }

    void DestroyTexture(Babylon::Graphics::TextureT texture)
    {
        if (texture != nullptr)
        {
            texture->RemoveReference();
        }
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
