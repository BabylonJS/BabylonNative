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

// Copied from bgfx renderer_gl.h

#define GL_ATC_RGB_AMD 0x8C92
#define GL_ATC_RGBA_EXPLICIT_ALPHA_AMD 0x8C93
#define GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD 0x87EE

#define GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG 0x8C01
#define GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG 0x8C03
#define GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG 0x8C00
#define GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG 0x8C02
#define GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG 0x9137
#define GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG 0x9138
#define GL_COMPRESSED_SRGB_PVRTC_2BPPV1_EXT 0x8A54
#define GL_COMPRESSED_SRGB_PVRTC_4BPPV1_EXT 0x8A55
#define GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV1_EXT 0x8A56
#define GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV1_EXT 0x8A57

#define GL_ETC1_RGB8_OES 0x8D64

#define GL_COMPRESSED_SRGB8_ASTC_4x4_KHR 0x93D0
#define GL_COMPRESSED_SRGB8_ASTC_5x4_KHR 0x93D1
#define GL_COMPRESSED_SRGB8_ASTC_5x5_KHR 0x93D2
#define GL_COMPRESSED_SRGB8_ASTC_6x5_KHR 0x93D3
#define GL_COMPRESSED_SRGB8_ASTC_6x6_KHR 0x93D4
#define GL_COMPRESSED_SRGB8_ASTC_8x5_KHR 0x93D5
#define GL_COMPRESSED_SRGB8_ASTC_8x6_KHR 0x93D6
#define GL_COMPRESSED_SRGB8_ASTC_8x8_KHR 0x93D7
#define GL_COMPRESSED_SRGB8_ASTC_10x5_KHR 0x93D8
#define GL_COMPRESSED_SRGB8_ASTC_10x6_KHR 0x93D9
#define GL_COMPRESSED_SRGB8_ASTC_10x8_KHR 0x93DA
#define GL_COMPRESSED_SRGB8_ASTC_10x10_KHR 0x93DB
#define GL_COMPRESSED_SRGB8_ASTC_12x10_KHR 0x93DC
#define GL_COMPRESSED_SRGB8_ASTC_12x12_KHR 0x93DD

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
            {GL_COMPRESSED_LUMINANCE_LATC1_EXT, GL_ZERO},                                                // BC4
            {GL_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT, GL_ZERO},                                          // BC5
            {GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB, GL_ZERO},                                          // BC6H
            {GL_COMPRESSED_RGBA_BPTC_UNORM_ARB, GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB},                // BC7
            {GL_ETC1_RGB8_OES, GL_ZERO},                                                                 // ETC1
            {GL_COMPRESSED_RGB8_ETC2, GL_ZERO},                                                          // ETC2
            {GL_COMPRESSED_RGBA8_ETC2_EAC, GL_COMPRESSED_SRGB8_ETC2},                                    // ETC2A
            {GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2, GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2}, // ETC2A1
            {GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG, GL_COMPRESSED_SRGB_PVRTC_2BPPV1_EXT},                   // PTC12
            {GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG, GL_COMPRESSED_SRGB_PVRTC_4BPPV1_EXT},                   // PTC14
            {GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG, GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV1_EXT},            // PTC12A
            {GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG, GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV1_EXT},            // PTC14A
            {GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG, GL_ZERO},                                              // PTC22
            {GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG, GL_ZERO},                                              // PTC24
            {GL_ATC_RGB_AMD, GL_ZERO},                                                                   // ATC
            {GL_ATC_RGBA_EXPLICIT_ALPHA_AMD, GL_ZERO},                                                   // ATCE
            {GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD, GL_ZERO},                                               // ATCI
            {GL_COMPRESSED_RGBA_ASTC_4x4_KHR, GL_COMPRESSED_SRGB8_ASTC_4x4_KHR},                         // ASTC4x4
            {GL_COMPRESSED_RGBA_ASTC_5x4_KHR, GL_COMPRESSED_SRGB8_ASTC_5x4_KHR},                         // ASTC5x4
            {GL_COMPRESSED_RGBA_ASTC_5x5_KHR, GL_COMPRESSED_SRGB8_ASTC_5x5_KHR},                         // ASTC5x5
            {GL_COMPRESSED_RGBA_ASTC_6x5_KHR, GL_COMPRESSED_SRGB8_ASTC_6x5_KHR},                         // ASTC6x5
            {GL_COMPRESSED_RGBA_ASTC_6x6_KHR, GL_COMPRESSED_SRGB8_ASTC_6x6_KHR},                         // ASTC6x6
            {GL_COMPRESSED_RGBA_ASTC_8x5_KHR, GL_COMPRESSED_SRGB8_ASTC_8x5_KHR},                         // ASTC8x5
            {GL_COMPRESSED_RGBA_ASTC_8x6_KHR, GL_COMPRESSED_SRGB8_ASTC_8x6_KHR},                         // ASTC8x6
            {GL_COMPRESSED_RGBA_ASTC_8x8_KHR, GL_COMPRESSED_SRGB8_ASTC_8x8_KHR},                         // ASTC8x8
            {GL_COMPRESSED_RGBA_ASTC_10x5_KHR, GL_COMPRESSED_SRGB8_ASTC_10x5_KHR},                       // ASTC10x5
            {GL_COMPRESSED_RGBA_ASTC_10x6_KHR, GL_COMPRESSED_SRGB8_ASTC_10x6_KHR},                       // ASTC10x6
            {GL_COMPRESSED_RGBA_ASTC_10x8_KHR, GL_COMPRESSED_SRGB8_ASTC_10x8_KHR},                       // ASTC10x8
            {GL_COMPRESSED_RGBA_ASTC_10x10_KHR, GL_COMPRESSED_SRGB8_ASTC_10x10_KHR},                     // ASTC10x10
            {GL_COMPRESSED_RGBA_ASTC_12x10_KHR, GL_COMPRESSED_SRGB8_ASTC_12x10_KHR},                     // ASTC12x10
            {GL_COMPRESSED_RGBA_ASTC_12x12_KHR, GL_COMPRESSED_SRGB8_ASTC_12x12_KHR},                     // ASTC12x12
            {GL_ZERO, GL_ZERO},                                                                          // Unknown
            {GL_ZERO, GL_ZERO},                                                                          // R1
            {GL_ALPHA, GL_ZERO},                                                                         // A8
            {GL_R8, GL_ZERO},                                                                            // R8
            {GL_R8I, GL_ZERO},                                                                           // R8I
            {GL_R8UI, GL_ZERO},                                                                          // R8U
            {GL_R8_SNORM, GL_ZERO},                                                                      // R8S
            {GL_R16, GL_ZERO},                                                                           // R16
            {GL_R16I, GL_ZERO},                                                                          // R16I
            {GL_R16UI, GL_ZERO},                                                                         // R16U
            {GL_R16F, GL_ZERO},                                                                          // R16F
            {GL_R16_SNORM, GL_ZERO},                                                                     // R16S
            {GL_R32I, GL_ZERO},                                                                          // R32I
            {GL_R32UI, GL_ZERO},                                                                         // R32U
            {GL_R32F, GL_ZERO},                                                                          // R32F
            {GL_RG8, GL_ZERO},                                                                           // RG8
            {GL_RG8I, GL_ZERO},                                                                          // RG8I
            {GL_RG8UI, GL_ZERO},                                                                         // RG8U
            {GL_RG8_SNORM, GL_ZERO},                                                                     // RG8S
            {GL_RG16, GL_ZERO},                                                                          // RG16
            {GL_RG16I, GL_ZERO},                                                                         // RG16I
            {GL_RG16UI, GL_ZERO},                                                                        // RG16U
            {GL_RG16F, GL_ZERO},                                                                         // RG16F
            {GL_RG16_SNORM, GL_ZERO},                                                                    // RG16S
            {GL_RG32I, GL_ZERO},                                                                         // RG32I
            {GL_RG32UI, GL_ZERO},                                                                        // RG32U
            {GL_RG32F, GL_ZERO},                                                                         // RG32F
            {GL_RGB8, GL_SRGB8},                                                                         // RGB8
            {GL_RGB8I, GL_ZERO},                                                                         // RGB8I
            {GL_RGB8UI, GL_ZERO},                                                                        // RGB8U
            {GL_RGB8_SNORM, GL_ZERO},                                                                    // RGB8S
            {GL_RGB9_E5, GL_ZERO},                                                                       // RGB9E5F
            {GL_RGBA8, GL_SRGB8_ALPHA8},                                                                 // BGRA8
            {GL_RGBA8, GL_SRGB8_ALPHA8},                                                                 // RGBA8
            {GL_RGBA8I, GL_ZERO},                                                                        // RGBA8I
            {GL_RGBA8UI, GL_ZERO},                                                                       // RGBA8U
            {GL_RGBA8_SNORM, GL_ZERO},                                                                   // RGBA8S
            {GL_RGBA16, GL_ZERO},                                                                        // RGBA16
            {GL_RGBA16I, GL_ZERO},                                                                       // RGBA16I
            {GL_RGBA16UI, GL_ZERO},                                                                      // RGBA16U
            {GL_RGBA16F, GL_ZERO},                                                                       // RGBA16F
            {GL_RGBA16_SNORM, GL_ZERO},                                                                  // RGBA16S
            {GL_RGBA32I, GL_ZERO},                                                                       // RGBA32I
            {GL_RGBA32UI, GL_ZERO},                                                                      // RGBA32U
            {GL_RGBA32F, GL_ZERO},                                                                       // RGBA32F
            {GL_RGB565, GL_ZERO},                                                                        // B5G6R5
            {GL_RGB565, GL_ZERO},                                                                        // R5G6B5
            {GL_RGBA4, GL_ZERO},                                                                         // BGRA4
            {GL_RGBA4, GL_ZERO},                                                                         // RGBA4
            {GL_RGB5_A1, GL_ZERO},                                                                       // BGR5A1
            {GL_RGB5_A1, GL_ZERO},                                                                       // RGB5A1
            {GL_RGB10_A2, GL_ZERO},                                                                      // RGB10A2
            {GL_R11F_G11F_B10F, GL_ZERO},                                                                // RG11B10F
            {GL_ZERO, GL_ZERO},                                                                          // UnknownDepth
            {GL_DEPTH_COMPONENT16, GL_ZERO},                                                             // D16
            {GL_DEPTH_COMPONENT24, GL_ZERO},                                                             // D24
            {GL_DEPTH24_STENCIL8, GL_ZERO},                                                              // D24S8
            {GL_DEPTH_COMPONENT32, GL_ZERO},                                                             // D32
            {GL_DEPTH_COMPONENT32F, GL_ZERO},                                                            // D16F
            {GL_DEPTH_COMPONENT32F, GL_ZERO},                                                            // D24F
            {GL_DEPTH_COMPONENT32F, GL_ZERO},                                                            // D32F
            {GL_STENCIL_INDEX8, GL_ZERO},                                                                // D0S8
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
        GLint countMipMapLevelsExcludingBase()
        {
            int width, height;
            GLint mipCount = 0;

            // Query the width and height of each mipmap level
            while (true)
            {
                // Get the texture width and height at the current mipmap level
                glGetTexLevelParameteriv(GL_TEXTURE_2D, mipCount + 1, GL_TEXTURE_WIDTH, &width);
                glGetTexLevelParameteriv(GL_TEXTURE_2D, mipCount + 1, GL_TEXTURE_HEIGHT, &height);

                // If the width or height is 0, the mipmap level does not exist
                if (width == 0 || height == 0)
                {
                    break; // No more mipmap levels
                }

                // Increment the mipmap level and count
                mipCount++;
            }

            // Return the number of mipmap levels excluding level 0
            return mipCount;
        }

        void GetInfo(Graphics::TextureT textureID, Info& info)
        {
            GLint previousTexture = 0;
            glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousTexture);

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
            mipLevels = countMipMapLevelsExcludingBase();

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

            glBindTexture(GL_TEXTURE_2D, previousTexture);
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
