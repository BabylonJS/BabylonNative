#include <Babylon/Plugins/ExternalTexture.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Graphics/DeviceContext.h>
#include <Babylon/Graphics/RendererType.h>
#include <Babylon/Graphics/Texture.h>
#include <napi/pointer.h>
#include <bx/bx.h>
#include <bgfx/bgfx.h>
#include <GL/gl.h>

#include "ExternalTexture_Base.h"

namespace
{
    struct TextureFormatInfo
    {
        GLint m_fmt;
        GLint m_fmtSrgb;
    };

    static TextureFormatInfo s_textureFormat[] =
        {
            {GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT},                  // BC1
            {GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT},                  // BC2
            {GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT},                  // BC3
            {GL_COMPRESSED_RED_RGTC1, GL_INVALID_ENUM},                                                  // BC4
            {GL_COMPRESSED_RG_RGTC2, GL_INVALID_ENUM},                                                   // BC5
            {GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT, GL_INVALID_ENUM},                                    // BC6H
            {GL_COMPRESSED_RGBA_BPTC_UNORM, GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM},                        // BC7
            {GL_INVALID_ENUM, GL_INVALID_ENUM},                                                          // ETC1
            {GL_COMPRESSED_RGB8_ETC2, GL_COMPRESSED_SRGB8_ETC2},                                         // ETC2
            {GL_COMPRESSED_RGBA8_ETC2_EAC, GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC},                         // ETC2A
            {GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2, GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2}, // ETC2A1
            {GL_INVALID_ENUM, GL_INVALID_ENUM},                                                          // PTC12
            {GL_INVALID_ENUM, GL_INVALID_ENUM},                                                          // PTC14
            {GL_INVALID_ENUM, GL_INVALID_ENUM},                                                          // PTC12A
            {GL_INVALID_ENUM, GL_INVALID_ENUM},                                                          // PTC14A
            {GL_INVALID_ENUM, GL_INVALID_ENUM},                                                          // PTC22
            {GL_INVALID_ENUM, GL_INVALID_ENUM},                                                          // PTC24
            {GL_INVALID_ENUM, GL_INVALID_ENUM},                                                          // ATC
            {GL_INVALID_ENUM, GL_INVALID_ENUM},                                                          // ATCE
            {GL_INVALID_ENUM, GL_INVALID_ENUM},                                                          // ATCI
            {GL_COMPRESSED_RGBA_ASTC_4x4_KHR, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR},                  // ASTC4x4
            {GL_COMPRESSED_RGBA_ASTC_5x4_KHR, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR},                  // ASTC5x4
            {GL_COMPRESSED_RGBA_ASTC_5x5_KHR, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR},                  // ASTC5x5
            {GL_COMPRESSED_RGBA_ASTC_6x5_KHR, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR},                  // ASTC6x5
            {GL_COMPRESSED_RGBA_ASTC_6x6_KHR, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR},                  // ASTC6x6
            {GL_COMPRESSED_RGBA_ASTC_8x5_KHR, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR},                  // ASTC8x5
            {GL_COMPRESSED_RGBA_ASTC_8x6_KHR, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR},                  // ASTC8x6
            {GL_COMPRESSED_RGBA_ASTC_8x8_KHR, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR},                  // ASTC8x8
            {GL_COMPRESSED_RGBA_ASTC_10x5_KHR, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR},                // ASTC10x5
            {GL_COMPRESSED_RGBA_ASTC_10x6_KHR, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR},                // ASTC10x6
            {GL_COMPRESSED_RGBA_ASTC_10x8_KHR, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR},                // ASTC10x8
            {GL_COMPRESSED_RGBA_ASTC_10x10_KHR, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR},              // ASTC10x10
            {GL_COMPRESSED_RGBA_ASTC_12x10_KHR, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR},              // ASTC12x10
            {GL_COMPRESSED_RGBA_ASTC_12x12_KHR, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR},              // ASTC12x12
            {GL_INVALID_ENUM, GL_INVALID_ENUM},                                                          // Unknown
            {GL_INVALID_ENUM, GL_INVALID_ENUM},                                                          // R1
            {GL_INVALID_ENUM, GL_INVALID_ENUM},                                                          // A8
            {GL_R8, GL_INVALID_ENUM},                                                                    // R8
            {GL_R8I, GL_INVALID_ENUM},                                                                   // R8I
            {GL_R8UI, GL_INVALID_ENUM},                                                                  // R8U
            {GL_R8_SNORM, GL_INVALID_ENUM},                                                              // R8S
            {GL_R16, GL_INVALID_ENUM},                                                                   // R16
            {GL_R16I, GL_INVALID_ENUM},                                                                  // R16I
            {GL_R16UI, GL_INVALID_ENUM},                                                                 // R16U
            {GL_R16F, GL_INVALID_ENUM},                                                                  // R16F
            {GL_R16_SNORM, GL_INVALID_ENUM},                                                             // R16S
            {GL_R32I, GL_INVALID_ENUM},                                                                  // R32I
            {GL_R32UI, GL_INVALID_ENUM},                                                                 // R32U
            {GL_R32F, GL_INVALID_ENUM},                                                                  // R32F
            {GL_RG8, GL_INVALID_ENUM},                                                                   // RG8
            {GL_RG8I, GL_INVALID_ENUM},                                                                  // RG8I
            {GL_RG8UI, GL_INVALID_ENUM},                                                                 // RG8U
            {GL_RG8_SNORM, GL_INVALID_ENUM},                                                             // RG8S
            {GL_RG16, GL_INVALID_ENUM},                                                                  // RG16
            {GL_RG16I, GL_INVALID_ENUM},                                                                 // RG16I
            {GL_RG16UI, GL_INVALID_ENUM},                                                                // RG16U
            {GL_RG16F, GL_INVALID_ENUM},                                                                 // RG16F
            {GL_RG16_SNORM, GL_INVALID_ENUM},                                                            // RG16S
            {GL_RG32I, GL_INVALID_ENUM},                                                                 // RG32I
            {GL_RG32UI, GL_INVALID_ENUM},                                                                // RG32U
            {GL_RG32F, GL_INVALID_ENUM},                                                                 // RG32F
            {GL_RGB8, GL_INVALID_ENUM},                                                                  // RGB8
            {GL_RGB8I, GL_INVALID_ENUM},                                                                 // RGB8I
            {GL_RGB8UI, GL_INVALID_ENUM},                                                                // RGB8U
            {GL_RGB8_SNORM, GL_INVALID_ENUM},                                                            // RGB8S
            {GL_RGB9_E5, GL_INVALID_ENUM},                                                               // RGB9E5F
            {GL_BGRA8_EXT, GL_INVALID_ENUM},                                                             // BGRA8
            {GL_RGBA8, GL_SRGB8_ALPHA8},                                                                 // RGBA8
            {GL_RGBA8I, GL_INVALID_ENUM},                                                                // RGBA8I
            {GL_RGBA8UI, GL_INVALID_ENUM},                                                               // RGBA8U
            {GL_RGBA8_SNORM, GL_INVALID_ENUM},                                                           // RGBA8S
            {GL_RGBA16, GL_INVALID_ENUM},                                                                // RGBA16
            {GL_RGBA16I, GL_INVALID_ENUM},                                                               // RGBA16I
            {GL_RGBA16UI, GL_INVALID_ENUM},                                                              // RGBA16U
            {GL_RGBA16F, GL_INVALID_ENUM},                                                               // RGBA16F
            {GL_RGBA16_SNORM, GL_INVALID_ENUM},                                                          // RGBA16S
            {GL_RGBA32I, GL_INVALID_ENUM},                                                               // RGBA32I
            {GL_RGBA32UI, GL_INVALID_ENUM},                                                              // RGBA32U
            {GL_RGBA32F, GL_INVALID_ENUM},                                                               // RGBA32F
            {GL_INVALID_ENUM, GL_INVALID_ENUM},                                                          // B5G6R5
            {GL_RGB565, GL_INVALID_ENUM},                                                                // R5G6B5
            {GL_INVALID_ENUM, GL_INVALID_ENUM},                                                          // BGRA4
            {GL_RGBA4, GL_INVALID_ENUM},                                                                 // RGBA4
            {GL_INVALID_ENUM, GL_INVALID_ENUM},                                                          // BGR5A1
            {GL_RGB5_A1, GL_INVALID_ENUM},                                                               // RGB5A1
            {GL_RGB10_A2, GL_INVALID_ENUM},                                                              // RGB10A2
            {GL_R11F_G11F_B10F, GL_INVALID_ENUM},                                                        // RG11B10F
            {GL_INVALID_ENUM, GL_INVALID_ENUM},                                                          // UnknownDepth
            {GL_DEPTH_COMPONENT16, GL_INVALID_ENUM},                                                     // D16
            {GL_DEPTH_COMPONENT24, GL_INVALID_ENUM},                                                     // D24
            {GL_DEPTH24_STENCIL8, GL_INVALID_ENUM},                                                      // D24S8
            {GL_DEPTH_COMPONENT32, GL_INVALID_ENUM},                                                     // D32
            {GL_INVALID_ENUM, GL_INVALID_ENUM},                                                          // D16F
            {GL_INVALID_ENUM, GL_INVALID_ENUM},                                                          // D24F
            {GL_DEPTH_COMPONENT32F, GL_INVALID_ENUM},                                                    // D32F
            {GL_STENCIL_INDEX8, GL_INVALID_ENUM},                                                        // D0S8
    };
    BX_STATIC_ASSERT(bgfx::TextureFormat::Count == BX_COUNTOF(s_textureFormat));

}

namespace Babylon::Plugins
{
    class ExternalTexture::Impl final : public ImplBase
    {
    public:
        // Implemented in ExternalTexture_Shared.h
        Impl(Graphics::TextureT);
        void Update(Graphics::TextureT);

        ~Impl()
        {
            if (m_textureID)
            {
                glDeleteTextures(1, &m_textureID);
            }
        }

        uintptr_t Ptr() const
        {
            return m_textureID;
        }

    private:
        int countMipMapLevelsExcludingBase(GLuint textureID)
        {
            // Bind the texture
            glBindTexture(GL_TEXTURE_2D, textureID);

            int width, height;
            int mipLevel = 1; // Start from level 1 (excluding level 0)
            int mipCount = 0;

            // Query the width and height of each mipmap level
            while (true)
            {
                // Get the texture width and height at the current mipmap level
                glGetTexLevelParameteriv(GL_TEXTURE_2D, mipLevel, GL_TEXTURE_WIDTH, &width);
                glGetTexLevelParameteriv(GL_TEXTURE_2D, mipLevel, GL_TEXTURE_HEIGHT, &height);

                // If the width or height is 0, the mipmap level does not exist
                if (width == 0 && height == 0)
                {
                    break; // No more mipmap levels
                }

                // Increment the mipmap level and count
                mipLevel++;
                mipCount++;
            }

            // Return the number of mipmap levels excluding level 0
            return mipCount;
        }

        void GetInfo(Graphics::TextureT textureID, Info& info)
        {
            glBindTexture(GL_TEXTURE_2D, textureID); // Bind the texture

            // Retrieve width and height of the base mip level (level 0)
            GLint width, height, internalFormat, mipLevels;
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalFormat);

            if (width == 0 || height == 0)
            {
                throw std::runtime_error{"Unsupported or invalid texture"};
            }

            // Retrieve the number of mip levels
            mipLevels = countMipMapLevelsExcludingBase(textureID);

            info.Width = static_cast<uint16_t>(width);
            info.Height = static_cast<uint16_t>(height);
            info.MipLevels = static_cast<uint16_t>(mipLevels);

            info.Flags |= BGFX_TEXTURE_RT;
            // Handle MSAA (Multisample Anti-Aliasing) in OpenGL (e.g., use `GL_TEXTURE_SAMPLES`)
            GLint samples;
            glGetTexLevelParameteriv(GL_TEXTURE_2D_MULTISAMPLE, 0, GL_TEXTURE_SAMPLES, &samples);
            if (samples > 1)
            {
                info.Flags |= BGFX_TEXTURE_MSAA_SAMPLE | RenderTargetSamplesToBgfxMsaaFlag(samples);
            }

            // Match the texture format with `s_textureFormat`
            for (long unsigned int i = 0; i < BX_COUNTOF(s_textureFormat); ++i)
            {
                const auto& format = s_textureFormat[i];
                if (format.m_fmt == internalFormat || format.m_fmtSrgb == internalFormat)
                {
                    info.Format = static_cast<bgfx::TextureFormat::Enum>(i);
                    if (format.m_fmtSrgb == internalFormat)
                    {
                        info.Flags |= BGFX_TEXTURE_SRGB;
                    }

                    break;
                }
            }

            glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture when done
        }

        void Assign(Graphics::TextureT textureID)
        {
            m_textureID = textureID;
        }

    private:
        GLuint m_textureID;
    };
}

#include "ExternalTexture_Shared.h"
