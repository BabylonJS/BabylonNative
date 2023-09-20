#include <Babylon/Plugins/ExternalTexture.h>
#include <Babylon/Graphics/DeviceContext.h>
#include <Babylon/Graphics/Texture.h>
#include <napi/napi_pointer.h>
#include <bx/bx.h>

#include "ExternalTexture_Base.h"

// clang-format off

// Copied fom renderer_mtl.cpp
namespace
{
    struct TextureFormatInfo
    {
        MTLPixelFormat m_fmt;
        MTLPixelFormat m_fmtSrgb;
    };

    static TextureFormatInfo s_textureFormat[] =
    {
        { MTLPixelFormat(130/*BC1_RGBA*/),              MTLPixelFormat(131/*BC1_RGBA_sRGB*/)        }, // BC1
        { MTLPixelFormat(132/*BC2_RGBA*/),              MTLPixelFormat(133/*BC2_RGBA_sRGB*/)        }, // BC2
        { MTLPixelFormat(134/*BC3_RGBA*/),              MTLPixelFormat(135/*BC3_RGBA_sRGB*/)        }, // BC3
        { MTLPixelFormat(140/*BC4_RUnorm*/),            MTLPixelFormatInvalid                       }, // BC4
        { MTLPixelFormat(142/*BC5_RGUnorm*/),           MTLPixelFormatInvalid                       }, // BC5
        { MTLPixelFormat(150/*BC6H_RGBFloat*/),         MTLPixelFormatInvalid                       }, // BC6H
        { MTLPixelFormat(152/*BC7_RGBAUnorm*/),         MTLPixelFormat(153/*BC7_RGBAUnorm_sRGB*/)   }, // BC7
        { MTLPixelFormatInvalid,                        MTLPixelFormatInvalid                       }, // ETC1
        { MTLPixelFormat(180/*ETC2_RGB8*/),             MTLPixelFormat(181/*ETC2_RGB8_sRGB*/)       }, // ETC2
        { MTLPixelFormat(178/*EAC_RGBA8*/),             MTLPixelFormat(179/*EAC_RGBA8_sRGB*/)       }, // ETC2A
        { MTLPixelFormat(182/*ETC2_RGB8A1*/),           MTLPixelFormat(183/*ETC2_RGB8A1_sRGB*/)     }, // ETC2A1
        { MTLPixelFormat(160/*PVRTC_RGB_2BPP*/),        MTLPixelFormat(161/*PVRTC_RGB_2BPP_sRGB*/)  }, // PTC12
        { MTLPixelFormat(162/*PVRTC_RGB_4BPP*/),        MTLPixelFormat(163/*PVRTC_RGB_4BPP_sRGB*/)  }, // PTC14
        { MTLPixelFormat(164/*PVRTC_RGBA_2BPP*/),       MTLPixelFormat(165/*PVRTC_RGBA_2BPP_sRGB*/) }, // PTC12A
        { MTLPixelFormat(166/*PVRTC_RGBA_4BPP*/),       MTLPixelFormat(167/*PVRTC_RGBA_4BPP_sRGB*/) }, // PTC14A
        { MTLPixelFormatInvalid,                        MTLPixelFormatInvalid                       }, // PTC22
        { MTLPixelFormatInvalid,                        MTLPixelFormatInvalid                       }, // PTC24
        { MTLPixelFormatInvalid,                        MTLPixelFormatInvalid                       }, // ATC
        { MTLPixelFormatInvalid,                        MTLPixelFormatInvalid                       }, // ATCE
        { MTLPixelFormatInvalid,                        MTLPixelFormatInvalid                       }, // ATCI
#if BX_PLATFORM_IOS && !TARGET_OS_MACCATALYST
        { MTLPixelFormatASTC_4x4_LDR,                   MTLPixelFormatASTC_4x4_sRGB                 }, // ASTC4x4
        { MTLPixelFormatASTC_5x4_LDR,                   MTLPixelFormatASTC_5x4_sRGB                 }, // ASTC5x4
        { MTLPixelFormatASTC_5x5_LDR,                   MTLPixelFormatASTC_5x5_sRGB                 }, // ASTC5x5
        { MTLPixelFormatASTC_6x5_LDR,                   MTLPixelFormatASTC_6x5_sRGB                 }, // ASTC6x5
        { MTLPixelFormatASTC_6x6_LDR,                   MTLPixelFormatASTC_6x6_sRGB                 }, // ASTC6x6
        { MTLPixelFormatASTC_8x5_LDR,                   MTLPixelFormatASTC_8x5_sRGB                 }, // ASTC8x5
        { MTLPixelFormatASTC_8x6_LDR,                   MTLPixelFormatASTC_8x6_sRGB                 }, // ASTC8x6
        { MTLPixelFormatASTC_8x8_LDR,                   MTLPixelFormatASTC_8x8_sRGB                 }, // ASTC8x8
        { MTLPixelFormatASTC_10x5_LDR,                  MTLPixelFormatASTC_10x5_sRGB                }, // ASTC10x5
        { MTLPixelFormatASTC_10x6_LDR,                  MTLPixelFormatASTC_10x6_sRGB                }, // ASTC10x6
        { MTLPixelFormatASTC_10x8_LDR,                  MTLPixelFormatASTC_10x8_sRGB                }, // ASTC10x8
        { MTLPixelFormatASTC_10x10_LDR,                 MTLPixelFormatASTC_10x10_sRGB               }, // ASTC10x10
        { MTLPixelFormatASTC_12x10_LDR,                 MTLPixelFormatASTC_12x10_sRGB               }, // ASTC12x10
        { MTLPixelFormatASTC_12x12_LDR,                 MTLPixelFormatASTC_12x12_sRGB               }, // ASTC12x12
#else
        { MTLPixelFormatInvalid,                        MTLPixelFormatInvalid                       }, // ASTC4x4
        { MTLPixelFormatInvalid,                        MTLPixelFormatInvalid                       }, // ASTC5x4
        { MTLPixelFormatInvalid,                        MTLPixelFormatInvalid                       }, // ASTC5x5
        { MTLPixelFormatInvalid,                        MTLPixelFormatInvalid                       }, // ASTC6x5
        { MTLPixelFormatInvalid,                        MTLPixelFormatInvalid                       }, // ASTC6x6
        { MTLPixelFormatInvalid,                        MTLPixelFormatInvalid                       }, // ASTC8x5
        { MTLPixelFormatInvalid,                        MTLPixelFormatInvalid                       }, // ASTC8x6
        { MTLPixelFormatInvalid,                        MTLPixelFormatInvalid                       }, // ASTC8x8
        { MTLPixelFormatInvalid,                        MTLPixelFormatInvalid                       }, // ASTC10x5
        { MTLPixelFormatInvalid,                        MTLPixelFormatInvalid                       }, // ASTC10x6
        { MTLPixelFormatInvalid,                        MTLPixelFormatInvalid                       }, // ASTC10x8
        { MTLPixelFormatInvalid,                        MTLPixelFormatInvalid                       }, // ASTC10x10
        { MTLPixelFormatInvalid,                        MTLPixelFormatInvalid                       }, // ASTC12x10
        { MTLPixelFormatInvalid,                        MTLPixelFormatInvalid                       }, // ASTC12x12
#endif // BX_PLATFORM_IOS && !TARGET_OS_MACCATALYST
        { MTLPixelFormatInvalid,                        MTLPixelFormatInvalid                       }, // Unknown
        { MTLPixelFormatInvalid,                        MTLPixelFormatInvalid                       }, // R1
        { MTLPixelFormatA8Unorm,                        MTLPixelFormatInvalid                       }, // A8
        { MTLPixelFormatR8Unorm,                        MTLPixelFormat(11/*R8Unorm_sRGB*/)          }, // R8
        { MTLPixelFormatR8Sint,                         MTLPixelFormatInvalid                       }, // R8I
        { MTLPixelFormatR8Uint,                         MTLPixelFormatInvalid                       }, // R8U
        { MTLPixelFormatR8Snorm,                        MTLPixelFormatInvalid                       }, // R8S
        { MTLPixelFormatR16Unorm,                       MTLPixelFormatInvalid                       }, // R16
        { MTLPixelFormatR16Sint,                        MTLPixelFormatInvalid                       }, // R16I
        { MTLPixelFormatR16Uint,                        MTLPixelFormatInvalid                       }, // R16U
        { MTLPixelFormatR16Float,                       MTLPixelFormatInvalid                       }, // R16F
        { MTLPixelFormatR16Snorm,                       MTLPixelFormatInvalid                       }, // R16S
        { MTLPixelFormatR32Sint,                        MTLPixelFormatInvalid                       }, // R32I
        { MTLPixelFormatR32Uint,                        MTLPixelFormatInvalid                       }, // R32U
        { MTLPixelFormatR32Float,                       MTLPixelFormatInvalid                       }, // R32F
        { MTLPixelFormatRG8Unorm,                       MTLPixelFormat(31/*RG8Unorm_sRGB*/)         }, // RG8
        { MTLPixelFormatRG8Sint,                        MTLPixelFormatInvalid                       }, // RG8I
        { MTLPixelFormatRG8Uint,                        MTLPixelFormatInvalid                       }, // RG8U
        { MTLPixelFormatRG8Snorm,                       MTLPixelFormatInvalid                       }, // RG8S
        { MTLPixelFormatRG16Unorm,                      MTLPixelFormatInvalid                       }, // RG16
        { MTLPixelFormatRG16Sint,                       MTLPixelFormatInvalid                       }, // RG16I
        { MTLPixelFormatRG16Uint,                       MTLPixelFormatInvalid                       }, // RG16U
        { MTLPixelFormatRG16Float,                      MTLPixelFormatInvalid                       }, // RG16F
        { MTLPixelFormatRG16Snorm,                      MTLPixelFormatInvalid                       }, // RG16S
        { MTLPixelFormatRG32Sint,                       MTLPixelFormatInvalid                       }, // RG32I
        { MTLPixelFormatRG32Uint,                       MTLPixelFormatInvalid                       }, // RG32U
        { MTLPixelFormatRG32Float,                      MTLPixelFormatInvalid                       }, // RG32F
        { MTLPixelFormatInvalid,                        MTLPixelFormatInvalid                       }, // RGB8
        { MTLPixelFormatInvalid,                        MTLPixelFormatInvalid                       }, // RGB8I
        { MTLPixelFormatInvalid,                        MTLPixelFormatInvalid                       }, // RGB8U
        { MTLPixelFormatInvalid,                        MTLPixelFormatInvalid                       }, // RGB8S
        { MTLPixelFormatRGB9E5Float,                    MTLPixelFormatInvalid                       }, // RGB9E5F
        { MTLPixelFormatBGRA8Unorm,                     MTLPixelFormatBGRA8Unorm_sRGB               }, // BGRA8
        { MTLPixelFormatRGBA8Unorm,                     MTLPixelFormatRGBA8Unorm_sRGB               }, // RGBA8
        { MTLPixelFormatRGBA8Sint,                      MTLPixelFormatInvalid                       }, // RGBA8I
        { MTLPixelFormatRGBA8Uint,                      MTLPixelFormatInvalid                       }, // RGBA8U
        { MTLPixelFormatRGBA8Snorm,                     MTLPixelFormatInvalid                       }, // RGBA8S
        { MTLPixelFormatRGBA16Unorm,                    MTLPixelFormatInvalid                       }, // RGBA16
        { MTLPixelFormatRGBA16Sint,                     MTLPixelFormatInvalid                       }, // RGBA16I
        { MTLPixelFormatRGBA16Uint,                     MTLPixelFormatInvalid                       }, // RGBA16U
        { MTLPixelFormatRGBA16Float,                    MTLPixelFormatInvalid                       }, // RGBA16F
        { MTLPixelFormatRGBA16Snorm,                    MTLPixelFormatInvalid                       }, // RGBA16S
        { MTLPixelFormatRGBA32Sint,                     MTLPixelFormatInvalid                       }, // RGBA32I
        { MTLPixelFormatRGBA32Uint,                     MTLPixelFormatInvalid                       }, // RGBA32U
        { MTLPixelFormatRGBA32Float,                    MTLPixelFormatInvalid                       }, // RGBA32F
        { MTLPixelFormatB5G6R5Unorm,                    MTLPixelFormatInvalid                       }, // B5G6R5
        { MTLPixelFormatB5G6R5Unorm,                    MTLPixelFormatInvalid                       }, // R5G6B5
        { MTLPixelFormatABGR4Unorm,                     MTLPixelFormatInvalid                       }, // BGRA4
        { MTLPixelFormatABGR4Unorm,                     MTLPixelFormatInvalid                       }, // RGBA4
        { MTLPixelFormatBGR5A1Unorm,                    MTLPixelFormatInvalid                       }, // BGR5A1
        { MTLPixelFormatBGR5A1Unorm,                    MTLPixelFormatInvalid                       }, // RGB5A1
        { MTLPixelFormatRGB10A2Unorm,                   MTLPixelFormatInvalid                       }, // RGB10A2
        { MTLPixelFormatRG11B10Float,                   MTLPixelFormatInvalid                       }, // RG11B10F
        { MTLPixelFormatInvalid,                        MTLPixelFormatInvalid                       }, // UnknownDepth
        { MTLPixelFormatDepth32Float,                   MTLPixelFormatInvalid                       }, // D16
        { MTLPixelFormatDepth32Float,                   MTLPixelFormatInvalid                       }, // D24
        { MTLPixelFormat(255/*Depth24Unorm_Stencil8*/), MTLPixelFormatInvalid                       }, // D24S8
        { MTLPixelFormatDepth32Float,                   MTLPixelFormatInvalid                       }, // D32
        { MTLPixelFormatDepth32Float,                   MTLPixelFormatInvalid                       }, // D16F
        { MTLPixelFormatDepth32Float,                   MTLPixelFormatInvalid                       }, // D24F
        { MTLPixelFormatDepth32Float,                   MTLPixelFormatInvalid                       }, // D32F
        { MTLPixelFormatStencil8,                       MTLPixelFormatInvalid                       }, // D0S8
    };
    BX_STATIC_ASSERT(bgfx::TextureFormat::Count == BX_COUNTOF(s_textureFormat));
}

// clang-format on

namespace Babylon::Plugins
{
    class ExternalTexture::Impl final : public ImplBase
    {
    public:
        // Implemented in ExternalTexture_Shared.h
        Impl(Graphics::TextureT);
        void Update(Graphics::TextureT);

        uintptr_t Ptr() const
        {
            return reinterpret_cast<uintptr_t>(m_ptr);
        }

    private:
        void GetInfo(Graphics::TextureT ptr, Info& info)
        {
            if (ptr.textureType != MTLTextureType2D && ptr.textureType != MTLTextureType2DMultisample)
            {
                throw std::runtime_error{"Unsupported texture type"};
            }

            info.Width = static_cast<uint16_t>(ptr.width);
            info.Height = static_cast<uint16_t>(ptr.height);
            info.MipLevels = static_cast<uint16_t>(ptr.mipmapLevelCount);

            if ((ptr.usage & MTLTextureUsageRenderTarget) != 0)
            {
                info.Flags |= BGFX_TEXTURE_RT;

                if (ptr.sampleCount > 1)
                {
                    info.Flags |= BGFX_TEXTURE_MSAA_SAMPLE | RenderTargetSamplesToBgfxMsaaFlag(ptr.sampleCount);
                }
            }

            const auto pixelFormat = m_ptr.pixelFormat;
            for (size_t i = 0; i < BX_COUNTOF(s_textureFormat); ++i)
            {
                const auto& format = s_textureFormat[i];
                if (format.m_fmt == pixelFormat || format.m_fmtSrgb == pixelFormat)
                {
                    info.Format = static_cast<bgfx::TextureFormat::Enum>(i);
                    if (format.m_fmtSrgb == pixelFormat)
                    {
                        info.Flags |= BGFX_TEXTURE_SRGB;
                    }
                    break;
                }
            }
        }

        void Assign(Graphics::TextureT ptr)
        {
            m_ptr = ptr;
        }

        id<MTLTexture> m_ptr;
    };
}

#include "ExternalTexture_Shared.h"
