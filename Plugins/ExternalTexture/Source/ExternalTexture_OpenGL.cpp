#include <Babylon/Plugins/ExternalTexture.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Graphics/DeviceContext.h>
#include <Babylon/Graphics/RendererType.h>
#include <Babylon/Graphics/Texture.h>
#include <napi/pointer.h>
#include <bx/bx.h>
#include <bgfx/bgfx.h>

#include <algorithm>

// This backend runs on BabylonNative's OpenGL ES 3.0 context, so it uses only
// ES 3.0 entry points. Neither the dimensions nor the internal format can be
// queried from a bare handle on ES 3.0 (glGetTexLevelParameteriv is ES 3.1),
// so GL::Texture carries both.
#include <GLES3/gl3.h>

#include "ExternalTexture_Base.h"

// The bgfx OpenGLES backend uses a number of compressed/extension texture
// formats whose enum values are not present in the standard GLES headers.
// Define them here (values per the OpenGL registry) so the format table
// below mirrors bgfx/src/renderer_gl.cpp's s_textureFormat[]. These are
// used purely for reverse-mapping a texture's internalFormat to a
// bgfx::TextureFormat::Enum and never passed to a GL call here, so it is
// safe to define them unconditionally.
#ifndef GL_COMPRESSED_RGBA_S3TC_DXT1_EXT
#  define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT 0x83F1
#endif
#ifndef GL_COMPRESSED_RGBA_S3TC_DXT3_EXT
#  define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT 0x83F2
#endif
#ifndef GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
#  define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT 0x83F3
#endif
#ifndef GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT
#  define GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT 0x8C4D
#endif
#ifndef GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT
#  define GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT 0x8C4E
#endif
#ifndef GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT
#  define GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT 0x8C4F
#endif
#ifndef GL_COMPRESSED_LUMINANCE_LATC1_EXT
#  define GL_COMPRESSED_LUMINANCE_LATC1_EXT 0x8C70
#endif
#ifndef GL_COMPRESSED_SIGNED_LUMINANCE_LATC1_EXT
#  define GL_COMPRESSED_SIGNED_LUMINANCE_LATC1_EXT 0x8C71
#endif
#ifndef GL_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT
#  define GL_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT 0x8C72
#endif
#ifndef GL_COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2_EXT
#  define GL_COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2_EXT 0x8C73
#endif
#ifndef GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB
#  define GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB 0x8E8E
#endif
#ifndef GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB
#  define GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB 0x8E8F
#endif
#ifndef GL_COMPRESSED_RGBA_BPTC_UNORM_ARB
#  define GL_COMPRESSED_RGBA_BPTC_UNORM_ARB 0x8E8C
#endif
#ifndef GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB
#  define GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB 0x8E8D
#endif
#ifndef GL_ETC1_RGB8_OES
#  define GL_ETC1_RGB8_OES 0x8D64
#endif
#ifndef GL_COMPRESSED_SRGB8_ETC2
#  define GL_COMPRESSED_SRGB8_ETC2 0x9275
#endif
#ifndef GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC
#  define GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC 0x9279
#endif
#ifndef GL_COMPRESSED_R11_EAC
#  define GL_COMPRESSED_R11_EAC 0x9270
#endif
#ifndef GL_COMPRESSED_SIGNED_R11_EAC
#  define GL_COMPRESSED_SIGNED_R11_EAC 0x9271
#endif
#ifndef GL_COMPRESSED_RG11_EAC
#  define GL_COMPRESSED_RG11_EAC 0x9272
#endif
#ifndef GL_COMPRESSED_SIGNED_RG11_EAC
#  define GL_COMPRESSED_SIGNED_RG11_EAC 0x9273
#endif
#ifndef GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG
#  define GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG 0x8C01
#endif
#ifndef GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG
#  define GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG 0x8C00
#endif
#ifndef GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG
#  define GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG 0x8C03
#endif
#ifndef GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG
#  define GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG 0x8C02
#endif
#ifndef GL_COMPRESSED_SRGB_PVRTC_2BPPV1_EXT
#  define GL_COMPRESSED_SRGB_PVRTC_2BPPV1_EXT 0x8A54
#endif
#ifndef GL_COMPRESSED_SRGB_PVRTC_4BPPV1_EXT
#  define GL_COMPRESSED_SRGB_PVRTC_4BPPV1_EXT 0x8A55
#endif
#ifndef GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV1_EXT
#  define GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV1_EXT 0x8A56
#endif
#ifndef GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV1_EXT
#  define GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV1_EXT 0x8A57
#endif
#ifndef GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG
#  define GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG 0x9137
#endif
#ifndef GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG
#  define GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG 0x9138
#endif
#ifndef GL_ATC_RGB_AMD
#  define GL_ATC_RGB_AMD 0x8C92
#endif
#ifndef GL_ATC_RGBA_EXPLICIT_ALPHA_AMD
#  define GL_ATC_RGBA_EXPLICIT_ALPHA_AMD 0x8C93
#endif
#ifndef GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD
#  define GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD 0x87EE
#endif
#ifndef GL_R16
#  define GL_R16 0x822A
#endif
#ifndef GL_R16_SNORM
#  define GL_R16_SNORM 0x8F98
#endif
#ifndef GL_RG16
#  define GL_RG16 0x822C
#endif
#ifndef GL_RG16_SNORM
#  define GL_RG16_SNORM 0x8F99
#endif
#ifndef GL_RGBA16
#  define GL_RGBA16 0x805B
#endif
#ifndef GL_RGBA16_SNORM
#  define GL_RGBA16_SNORM 0x8F9B
#endif
#ifndef GL_BGRA
#  define GL_BGRA 0x80E1
#endif
#ifndef GL_DEPTH_COMPONENT32
#  define GL_DEPTH_COMPONENT32 0x81A7
#endif
#ifndef GL_TEXTURE_INTERNAL_FORMAT
#  define GL_TEXTURE_INTERNAL_FORMAT 0x1003
#endif
// ASTC (KHR_texture_compression_astc_ldr). Standard GLES 3.1 headers do not
// always declare these; define them here so the table compiles.
#ifndef GL_COMPRESSED_RGBA_ASTC_4x4_KHR
#  define GL_COMPRESSED_RGBA_ASTC_4x4_KHR 0x93B0
#endif
#ifndef GL_COMPRESSED_RGBA_ASTC_5x4_KHR
#  define GL_COMPRESSED_RGBA_ASTC_5x4_KHR 0x93B1
#endif
#ifndef GL_COMPRESSED_RGBA_ASTC_5x5_KHR
#  define GL_COMPRESSED_RGBA_ASTC_5x5_KHR 0x93B2
#endif
#ifndef GL_COMPRESSED_RGBA_ASTC_6x5_KHR
#  define GL_COMPRESSED_RGBA_ASTC_6x5_KHR 0x93B3
#endif
#ifndef GL_COMPRESSED_RGBA_ASTC_6x6_KHR
#  define GL_COMPRESSED_RGBA_ASTC_6x6_KHR 0x93B4
#endif
#ifndef GL_COMPRESSED_RGBA_ASTC_8x5_KHR
#  define GL_COMPRESSED_RGBA_ASTC_8x5_KHR 0x93B5
#endif
#ifndef GL_COMPRESSED_RGBA_ASTC_8x6_KHR
#  define GL_COMPRESSED_RGBA_ASTC_8x6_KHR 0x93B6
#endif
#ifndef GL_COMPRESSED_RGBA_ASTC_8x8_KHR
#  define GL_COMPRESSED_RGBA_ASTC_8x8_KHR 0x93B7
#endif
#ifndef GL_COMPRESSED_RGBA_ASTC_10x5_KHR
#  define GL_COMPRESSED_RGBA_ASTC_10x5_KHR 0x93B8
#endif
#ifndef GL_COMPRESSED_RGBA_ASTC_10x6_KHR
#  define GL_COMPRESSED_RGBA_ASTC_10x6_KHR 0x93B9
#endif
#ifndef GL_COMPRESSED_RGBA_ASTC_10x8_KHR
#  define GL_COMPRESSED_RGBA_ASTC_10x8_KHR 0x93BA
#endif
#ifndef GL_COMPRESSED_RGBA_ASTC_10x10_KHR
#  define GL_COMPRESSED_RGBA_ASTC_10x10_KHR 0x93BB
#endif
#ifndef GL_COMPRESSED_RGBA_ASTC_12x10_KHR
#  define GL_COMPRESSED_RGBA_ASTC_12x10_KHR 0x93BC
#endif
#ifndef GL_COMPRESSED_RGBA_ASTC_12x12_KHR
#  define GL_COMPRESSED_RGBA_ASTC_12x12_KHR 0x93BD
#endif
#ifndef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR
#  define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR 0x93D0
#endif
#ifndef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR
#  define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR 0x93D1
#endif
#ifndef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR
#  define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR 0x93D2
#endif
#ifndef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR
#  define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR 0x93D3
#endif
#ifndef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR
#  define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR 0x93D4
#endif
#ifndef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR
#  define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR 0x93D5
#endif
#ifndef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR
#  define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR 0x93D6
#endif
#ifndef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR
#  define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR 0x93D7
#endif
#ifndef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR
#  define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR 0x93D8
#endif
#ifndef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR
#  define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR 0x93D9
#endif
#ifndef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR
#  define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR 0x93DA
#endif
#ifndef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR
#  define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR 0x93DB
#endif
#ifndef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR
#  define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR 0x93DC
#endif
#ifndef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR
#  define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR 0x93DD
#endif

namespace Babylon::Plugins
{
    uintptr_t NativeTextureHandle(Graphics::TextureT ptr)
    {
        return ptr == nullptr ? 0 : static_cast<uintptr_t>(ptr->Handle());
    }

    namespace
    {
        // Two-column slice of the bgfx OpenGLES s_textureFormat[] table
        // (see bgfx/src/renderer_gl.cpp). Indexed by bgfx::TextureFormat::Enum.
        // Used by GetInfo to reverse-map a texture's GL internalFormat back to
        // the corresponding bgfx::TextureFormat::Enum (with sRGB detection).
        struct TextureFormatInfo
        {
            GLenum m_fmt;
            GLenum m_fmtSrgb;
        };

        constexpr TextureFormatInfo s_textureFormat[] =
        {
            { GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,            GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT       }, // BC1
            { GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,            GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT       }, // BC2
            { GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,            GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT       }, // BC3
            { GL_COMPRESSED_LUMINANCE_LATC1_EXT,           GL_ZERO                                      }, // BC4
            { GL_COMPRESSED_SIGNED_LUMINANCE_LATC1_EXT,    GL_ZERO                                      }, // BC4S
            { GL_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT,     GL_ZERO                                      }, // BC5
            { GL_COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2_EXT, GL_ZERO                                   }, // BC5S
            { GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB,     GL_ZERO                                      }, // BC6H
            { GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB,   GL_ZERO                                      }, // BC6HU
            { GL_COMPRESSED_RGBA_BPTC_UNORM_ARB,           GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB      }, // BC7
            { GL_ETC1_RGB8_OES,                            GL_ZERO                                      }, // ETC1
            { GL_COMPRESSED_RGB8_ETC2,                     GL_COMPRESSED_SRGB8_ETC2                     }, // ETC2
            { GL_COMPRESSED_RGBA8_ETC2_EAC,                GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC          }, // ETC2A
            { GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2, GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2 }, // ETC2A1
            { GL_COMPRESSED_R11_EAC,                       GL_ZERO                                      }, // EACR11
            { GL_COMPRESSED_SIGNED_R11_EAC,                GL_ZERO                                      }, // EACR11S
            { GL_COMPRESSED_RG11_EAC,                      GL_ZERO                                      }, // EACRG11
            { GL_COMPRESSED_SIGNED_RG11_EAC,               GL_ZERO                                      }, // EACRG11S
            { GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG,          GL_COMPRESSED_SRGB_PVRTC_2BPPV1_EXT          }, // PTC12
            { GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG,          GL_COMPRESSED_SRGB_PVRTC_4BPPV1_EXT          }, // PTC14
            { GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG,         GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV1_EXT    }, // PTC12A
            { GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG,         GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV1_EXT    }, // PTC14A
            { GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG,         GL_ZERO                                      }, // PTC22
            { GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG,         GL_ZERO                                      }, // PTC24
            { GL_ATC_RGB_AMD,                              GL_ZERO                                      }, // ATC
            { GL_ATC_RGBA_EXPLICIT_ALPHA_AMD,              GL_ZERO                                      }, // ATCE
            { GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD,          GL_ZERO                                      }, // ATCI
            { GL_COMPRESSED_RGBA_ASTC_4x4_KHR,             GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR      }, // ASTC4x4
            { GL_COMPRESSED_RGBA_ASTC_5x4_KHR,             GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR      }, // ASTC5x4
            { GL_COMPRESSED_RGBA_ASTC_5x5_KHR,             GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR      }, // ASTC5x5
            { GL_COMPRESSED_RGBA_ASTC_6x5_KHR,             GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR      }, // ASTC6x5
            { GL_COMPRESSED_RGBA_ASTC_6x6_KHR,             GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR      }, // ASTC6x6
            { GL_COMPRESSED_RGBA_ASTC_8x5_KHR,             GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR      }, // ASTC8x5
            { GL_COMPRESSED_RGBA_ASTC_8x6_KHR,             GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR      }, // ASTC8x6
            { GL_COMPRESSED_RGBA_ASTC_8x8_KHR,             GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR      }, // ASTC8x8
            { GL_COMPRESSED_RGBA_ASTC_10x5_KHR,            GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR     }, // ASTC10x5
            { GL_COMPRESSED_RGBA_ASTC_10x6_KHR,            GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR     }, // ASTC10x6
            { GL_COMPRESSED_RGBA_ASTC_10x8_KHR,            GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR     }, // ASTC10x8
            { GL_COMPRESSED_RGBA_ASTC_10x10_KHR,           GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR    }, // ASTC10x10
            { GL_COMPRESSED_RGBA_ASTC_12x10_KHR,           GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR    }, // ASTC12x10
            { GL_COMPRESSED_RGBA_ASTC_12x12_KHR,           GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR    }, // ASTC12x12
            { GL_ZERO,                                     GL_ZERO                                      }, // Unknown
            { GL_ZERO,                                     GL_ZERO                                      }, // R1
            { GL_ALPHA,                                    GL_ZERO                                      }, // A8
            { GL_R8,                                       GL_ZERO                                      }, // R8
            { GL_R8I,                                      GL_ZERO                                      }, // R8I
            { GL_R8UI,                                     GL_ZERO                                      }, // R8U
            { GL_R8_SNORM,                                 GL_ZERO                                      }, // R8S
            { GL_R16,                                      GL_ZERO                                      }, // R16
            { GL_R16I,                                     GL_ZERO                                      }, // R16I
            { GL_R16UI,                                    GL_ZERO                                      }, // R16U
            { GL_R16F,                                     GL_ZERO                                      }, // R16F
            { GL_R16_SNORM,                                GL_ZERO                                      }, // R16S
            { GL_R32I,                                     GL_ZERO                                      }, // R32I
            { GL_R32UI,                                    GL_ZERO                                      }, // R32U
            { GL_R32F,                                     GL_ZERO                                      }, // R32F
            { GL_RG8,                                      GL_ZERO                                      }, // RG8
            { GL_RG8I,                                     GL_ZERO                                      }, // RG8I
            { GL_RG8UI,                                    GL_ZERO                                      }, // RG8U
            { GL_RG8_SNORM,                                GL_ZERO                                      }, // RG8S
            { GL_RG16,                                     GL_ZERO                                      }, // RG16
            { GL_RG16I,                                    GL_ZERO                                      }, // RG16I
            { GL_RG16UI,                                   GL_ZERO                                      }, // RG16U
            { GL_RG16F,                                    GL_ZERO                                      }, // RG16F
            { GL_RG16_SNORM,                               GL_ZERO                                      }, // RG16S
            { GL_RG32I,                                    GL_ZERO                                      }, // RG32I
            { GL_RG32UI,                                   GL_ZERO                                      }, // RG32U
            { GL_RG32F,                                    GL_ZERO                                      }, // RG32F
            { GL_RGB8,                                     GL_SRGB8                                     }, // RGB8
            { GL_RGB8I,                                    GL_ZERO                                      }, // RGB8I
            { GL_RGB8UI,                                   GL_ZERO                                      }, // RGB8U
            { GL_RGB8_SNORM,                               GL_ZERO                                      }, // RGB8S
            { GL_RGB9_E5,                                  GL_ZERO                                      }, // RGB9E5F
            { GL_RGBA8,                                    GL_SRGB8_ALPHA8                              }, // BGRA8 (bgfx maps both to GL_RGBA8)
            { GL_RGBA8,                                    GL_SRGB8_ALPHA8                              }, // RGBA8
            { GL_RGBA8I,                                   GL_ZERO                                      }, // RGBA8I
            { GL_RGBA8UI,                                  GL_ZERO                                      }, // RGBA8U
            { GL_RGBA8_SNORM,                              GL_ZERO                                      }, // RGBA8S
            { GL_RGBA16,                                   GL_ZERO                                      }, // RGBA16
            { GL_RGBA16I,                                  GL_ZERO                                      }, // RGBA16I
            { GL_RGBA16UI,                                 GL_ZERO                                      }, // RGBA16U
            { GL_RGBA16F,                                  GL_ZERO                                      }, // RGBA16F
            { GL_RGBA16_SNORM,                             GL_ZERO                                      }, // RGBA16S
            { GL_RGBA32I,                                  GL_ZERO                                      }, // RGBA32I
            { GL_RGBA32UI,                                 GL_ZERO                                      }, // RGBA32U
            { GL_RGBA32F,                                  GL_ZERO                                      }, // RGBA32F
            { GL_RGB565,                                   GL_ZERO                                      }, // B5G6R5
            { GL_RGB565,                                   GL_ZERO                                      }, // R5G6B5
            { GL_RGBA4,                                    GL_ZERO                                      }, // BGRA4
            { GL_RGBA4,                                    GL_ZERO                                      }, // RGBA4
            { GL_RGB5_A1,                                  GL_ZERO                                      }, // BGR5A1
            { GL_RGB5_A1,                                  GL_ZERO                                      }, // RGB5A1
            { GL_RGB10_A2,                                 GL_ZERO                                      }, // RGB10A2
            { GL_RGB10_A2UI,                               GL_ZERO                                      }, // RGB10A2U
            { GL_R11F_G11F_B10F,                           GL_ZERO                                      }, // RG11B10F
            { GL_ZERO,                                     GL_ZERO                                      }, // UnknownDepth
            { GL_DEPTH_COMPONENT16,                        GL_ZERO                                      }, // D16
            { GL_DEPTH_COMPONENT24,                        GL_ZERO                                      }, // D24
            { GL_DEPTH24_STENCIL8,                         GL_ZERO                                      }, // D24S8
            { GL_DEPTH_COMPONENT32,                        GL_ZERO                                      }, // D32
            { GL_DEPTH_COMPONENT32F,                       GL_ZERO                                      }, // D16F
            { GL_DEPTH_COMPONENT32F,                       GL_ZERO                                      }, // D24F
            { GL_DEPTH_COMPONENT32F,                       GL_ZERO                                      }, // D32F
            { GL_DEPTH32F_STENCIL8,                        GL_ZERO                                      }, // D32FS8
            { GL_STENCIL_INDEX8,                           GL_ZERO                                      }, // D0S8
        };
        static_assert(bgfx::TextureFormat::Count == BX_COUNTOF(s_textureFormat));
    }

    class ExternalTexture::Impl final : public ImplBase
    {
    public:
        // Implemented in ExternalTexture_Shared.h
        Impl(Graphics::TextureT, std::optional<Graphics::TextureFormatT>);
        void Update(Graphics::TextureT, std::optional<Graphics::TextureFormatT>, std::optional<uint16_t>);

        Graphics::TextureT Get() const
        {
            return m_ptr.Get();
        }

    private:
        static void GetInfo(Graphics::TextureT ptr, std::optional<Graphics::TextureFormatT> overrideFormat, Info& info)
        {
            // NOTE: the GL state queries below require the bgfx OpenGL context to be current on
            // this thread. ExternalTexture is constructed/updated on the same scheduler that
            // AddToContextAsync uses for bgfx work, so context current-ness is the caller's
            // responsibility.

            if (ptr == nullptr)
            {
                throw std::runtime_error{"ExternalTexture: null OpenGL texture"};
            }

            const GLuint texture = ptr->Handle();

            // Reject obviously invalid handles up front. glIsTexture only returns true for
            // handles that have already been used (bound at least once); any real FBO color
            // attachment will satisfy this.
            if (texture == 0 || glIsTexture(texture) == GL_FALSE)
            {
                throw std::runtime_error{"ExternalTexture: invalid OpenGL texture handle"};
            }

            // bgfx::createTexture2D uses NumLayers directly, so it must be >= 1.
            if (ptr->Layers() == 0)
            {
                throw std::runtime_error{"ExternalTexture: OpenGL texture has no layers"};
            }

            // Dimensions travel with the texture. OpenGL ES 3.0 cannot report them from a bare
            // handle (glGetTexLevelParameteriv is ES 3.1), which is why GL::Texture carries them.
            if (ptr->Width() == 0 || ptr->Height() == 0)
            {
                throw std::runtime_error{"ExternalTexture: OpenGL texture has degenerate dimensions"};
            }

            info.Width = static_cast<uint16_t>(ptr->Width());
            info.Height = static_cast<uint16_t>(ptr->Height());

            // ES 3.0 cannot introspect the mip chain from a handle, so external textures bridged
            // into bgfx are single-level. The layer count travels with the texture for the same
            // reason the dimensions do.
            info.MipLevels = 1;
            info.NumLayers = static_cast<uint16_t>(ptr->Layers());

            // OpenGL cannot be asked whether a texture is usable as a framebuffer attachment, so
            // GL::Texture carries what its creator declared. bgfx rejects an attachment whose
            // texture has no BGFX_TEXTURE_RT* flag, and the flag also narrows format validation
            // to framebuffer-renderable formats, so it must not be set for sampled-only images.
            if (ptr->IsRenderTarget())
            {
                info.Flags |= BGFX_TEXTURE_RT;
            }

            // The GL internal format travels with the texture too. An explicit override still
            // wins, so callers can reinterpret (e.g. select the sRGB twin), exactly as on D3D
            // and Metal.
            const GLenum targetFormat = static_cast<GLenum>(overrideFormat.value_or(ptr->Format()));

            bool formatFound = false;
            for (size_t i = 0; i < BX_COUNTOF(s_textureFormat); ++i)
            {
                const auto bgfxFormat = static_cast<bgfx::TextureFormat::Enum>(i);

                // GL has no distinct BGRA-ordered internal formats: bgfx maps each BGRA*/BGR*
                // variant to the SAME GL internal format as its RGBA twin (BGRA8/RGBA8 ->
                // GL_RGBA8, B5G6R5/R5G6B5 -> GL_RGB565, etc.). Because this reverse lookup stops
                // at the first match and the BGRA rows precede their RGBA twins, skip the
                // BGRA-ordered rows so the RGBA (correct channel order) twin wins.
                if (bgfxFormat == bgfx::TextureFormat::BGRA8 ||
                    bgfxFormat == bgfx::TextureFormat::BGRA4 ||
                    bgfxFormat == bgfx::TextureFormat::BGR5A1 ||
                    bgfxFormat == bgfx::TextureFormat::B5G6R5)
                {
                    continue;
                }

                const auto& format = s_textureFormat[i];
                if (format.m_fmt == GL_ZERO && format.m_fmtSrgb == GL_ZERO)
                {
                    continue;
                }

                if (format.m_fmt == targetFormat || format.m_fmtSrgb == targetFormat)
                {
                    info.Format = bgfxFormat;
                    if (format.m_fmtSrgb == targetFormat && format.m_fmtSrgb != GL_ZERO)
                    {
                        info.Flags |= BGFX_TEXTURE_SRGB;
                    }
                    formatFound = true;
                    break;
                }
            }

            if (!formatFound)
            {
                throw std::runtime_error{"ExternalTexture: unsupported OpenGL texture internal format"};
            }
        }

        // Takes a reference of our own, so the caller may remove its own reference whenever it
        // likes, exactly as the D3D and Metal backends do with their platform holders.
        void Set(Graphics::TextureT ptr)
        {
            m_ptr.CopyFrom(ptr);
        }

        Graphics::GL::SharedPtr<Graphics::GL::Texture> m_ptr{};
    };
}

#include "ExternalTexture_Shared.h"
