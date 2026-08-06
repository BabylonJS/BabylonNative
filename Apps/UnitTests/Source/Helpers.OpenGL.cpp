#include <gtest/gtest.h>
#include "Helpers.h"

#include <GLES3/gl3.h>

#include <algorithm>
#include <stdexcept>
#include <thread>

namespace Helpers
{
    namespace
    {
        // Allocates one texture-complete mip level on the target implied by layers, and hands back
        // a reference-counted image whose release callback owns the GL name.
        //
        // glGenTextures/glTexImage* require a current GL context, which the single-threaded
        // Graphics::Device makes current on the calling thread between
        // StartRenderingCurrentFrame/FinishRenderingCurrentFrame (the same thread the tests create
        // textures on).
        //
        // pixels may be null, leaving the contents undefined: the render tests clear and draw into
        // the image before reading it back.
        Babylon::Graphics::TextureT CreateGLTexture(uint32_t width, uint32_t height, uint32_t layers, bool renderTarget, const void* pixels)
        {
            const bool isArray = layers > 1;
            const GLenum target = isArray ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D;

            GLint previousBinding = 0;
            glGetIntegerv(isArray ? GL_TEXTURE_BINDING_2D_ARRAY : GL_TEXTURE_BINDING_2D, &previousBinding);

            GLuint texture = 0;
            glGenTextures(1, &texture);
            glBindTexture(target, texture);

            if (isArray)
            {
                glTexImage3D(target, 0, GL_RGBA8, static_cast<GLsizei>(width), static_cast<GLsizei>(height),
                    static_cast<GLsizei>(layers), 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
            }
            else
            {
                glTexImage2D(target, 0, GL_RGBA8, static_cast<GLsizei>(width), static_cast<GLsizei>(height),
                    0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
            }

            glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            glBindTexture(target, static_cast<GLuint>(previousBinding));

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
            return Babylon::Graphics::GL::Texture::Create(
                {
                    .Handle = texture,
                    .Width = width,
                    .Height = height,
                    .Layers = layers,
                    .Format = GL_RGBA8,
                    .Usage = renderTarget ? Babylon::Graphics::GL::Texture::Usage::RenderTarget
                                          : Babylon::Graphics::GL::Texture::Usage::Sampled,
                },
                [creatingThread](unsigned int name) {
                    EXPECT_EQ(creatingThread, std::this_thread::get_id());
                    glDeleteTextures(1, &name);
                });
        }
    }

    Babylon::Graphics::TextureT CreateTexture(Babylon::Graphics::DeviceT, uint32_t width, uint32_t height, uint32_t arraySize, bool renderTarget, uint32_t samples)
    {
        // Multisample textures (GL_TEXTURE_2D_MULTISAMPLE) are ES 3.1, and Babylon Native pins
        // bgfx to ES 3.0 (BGFX_CONFIG_RENDERER_OPENGLES=30). The multisample tests are skipped
        // on this backend (see CMakeLists.txt).
        if (samples != 1)
        {
            throw std::runtime_error{"Helpers::CreateTexture(OpenGL): multisample textures require OpenGL ES 3.1"};
        }

        // OpenGL needs no special allocation for a texture that will be attached to a framebuffer,
        // so renderTarget only has to be recorded as usage.
        return CreateGLTexture(width, height, arraySize, renderTarget, nullptr);
    }

    void DestroyTexture(Babylon::Graphics::TextureT texture)
    {
        if (texture != nullptr)
        {
            texture->RemoveReference();
        }
    }

    Babylon::Graphics::TextureT CreateTextureArrayWithData(Babylon::Graphics::DeviceT, uint32_t width, uint32_t height, const Color* sliceColors, uint32_t sliceCount)
    {
        // glTexImage3D takes the whole array as one tightly packed buffer, slice-major. Color is
        // laid out exactly as GL_RGBA/GL_UNSIGNED_BYTE expects, so the slices upload as they are.
        const size_t texelsPerSlice = static_cast<size_t>(width) * height;
        std::vector<Color> pixels(texelsPerSlice * sliceCount);
        for (uint32_t slice = 0; slice < sliceCount; ++slice)
        {
            std::fill_n(pixels.data() + slice * texelsPerSlice, texelsPerSlice, sliceColors[slice]);
        }

        return CreateGLTexture(width, height, sliceCount, false, pixels.data());
    }

    std::vector<uint8_t> ReadPixels(const Babylon::Graphics::PlatformInfo&, Babylon::Graphics::TextureT texture, uint32_t width, uint32_t height)
    {
        // ES 3.0 has no glGetTexImage, so the only way to get texels back off a texture is to
        // attach it to a framebuffer and read that. The caller classifies pixels by count rather
        // than position, so GL's bottom-left origin (vs D3D's top-left) needs no correction.
        GLint previousFramebuffer = 0;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFramebuffer);

        GLuint framebuffer = 0;
        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->Handle(), 0);
        EXPECT_EQ(glCheckFramebufferStatus(GL_FRAMEBUFFER), static_cast<GLenum>(GL_FRAMEBUFFER_COMPLETE));

        std::vector<uint8_t> pixels(static_cast<size_t>(width) * height * 4);
        glReadPixels(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height), GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
        EXPECT_EQ(glGetError(), static_cast<GLenum>(GL_NO_ERROR));

        glBindFramebuffer(GL_FRAMEBUFFER, static_cast<GLuint>(previousFramebuffer));
        glDeleteFramebuffers(1, &framebuffer);

        return pixels;
    }
}
