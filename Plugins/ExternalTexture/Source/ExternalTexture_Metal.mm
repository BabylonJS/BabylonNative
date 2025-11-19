#include <Babylon/Plugins/ExternalTexture.h>
#include <Babylon/Graphics/DeviceContext.h>
#include <Babylon/Graphics/Texture.h>
#include <napi/pointer.h>
#include <bx/bx.h>

#include "ExternalTexture_Base.h"

// clang-format off

// Copied from renderer_mtl.h with unused pixel formats removed.
namespace
{
	// Metal API has obnoxious "availability" annotations on enums causing build errors when
	// referencing format, and requiring use of ifdefery to reference format. To reduce ifdefery
	// bgfx uses redefined formats, and on initialization it sets up format caps and provides
	// availability.
	constexpr MTLPixelFormat kMtlPixelFormatInvalid               = MTLPixelFormat(0);
	constexpr MTLPixelFormat kMtlPixelFormatA8Unorm               = MTLPixelFormat(1);
	constexpr MTLPixelFormat kMtlPixelFormatR8Unorm               = MTLPixelFormat(10);
	constexpr MTLPixelFormat kMtlPixelFormatR8Unorm_sRGB          = MTLPixelFormat(11);
	constexpr MTLPixelFormat kMtlPixelFormatR8Snorm               = MTLPixelFormat(12);
	constexpr MTLPixelFormat kMtlPixelFormatR8Uint                = MTLPixelFormat(13);
	constexpr MTLPixelFormat kMtlPixelFormatR8Sint                = MTLPixelFormat(14);
	constexpr MTLPixelFormat kMtlPixelFormatR16Unorm              = MTLPixelFormat(20);
	constexpr MTLPixelFormat kMtlPixelFormatR16Snorm              = MTLPixelFormat(22);
	constexpr MTLPixelFormat kMtlPixelFormatR16Uint               = MTLPixelFormat(23);
	constexpr MTLPixelFormat kMtlPixelFormatR16Sint               = MTLPixelFormat(24);
	constexpr MTLPixelFormat kMtlPixelFormatR16Float              = MTLPixelFormat(25);
	constexpr MTLPixelFormat kMtlPixelFormatRG8Unorm              = MTLPixelFormat(30);
	constexpr MTLPixelFormat kMtlPixelFormatRG8Unorm_sRGB         = MTLPixelFormat(31);
	constexpr MTLPixelFormat kMtlPixelFormatRG8Snorm              = MTLPixelFormat(32);
	constexpr MTLPixelFormat kMtlPixelFormatRG8Uint               = MTLPixelFormat(33);
	constexpr MTLPixelFormat kMtlPixelFormatRG8Sint               = MTLPixelFormat(34);
	constexpr MTLPixelFormat kMtlPixelFormatB5G6R5Unorm           = MTLPixelFormat(40);
	constexpr MTLPixelFormat kMtlPixelFormatABGR4Unorm            = MTLPixelFormat(42);
	constexpr MTLPixelFormat kMtlPixelFormatBGR5A1Unorm           = MTLPixelFormat(43);
	constexpr MTLPixelFormat kMtlPixelFormatR32Uint               = MTLPixelFormat(53);
	constexpr MTLPixelFormat kMtlPixelFormatR32Sint               = MTLPixelFormat(54);
	constexpr MTLPixelFormat kMtlPixelFormatR32Float              = MTLPixelFormat(55);
	constexpr MTLPixelFormat kMtlPixelFormatRG16Unorm             = MTLPixelFormat(60);
	constexpr MTLPixelFormat kMtlPixelFormatRG16Snorm             = MTLPixelFormat(62);
	constexpr MTLPixelFormat kMtlPixelFormatRG16Uint              = MTLPixelFormat(63);
	constexpr MTLPixelFormat kMtlPixelFormatRG16Sint              = MTLPixelFormat(64);
	constexpr MTLPixelFormat kMtlPixelFormatRG16Float             = MTLPixelFormat(65);
	constexpr MTLPixelFormat kMtlPixelFormatRGBA8Unorm            = MTLPixelFormat(70);
	constexpr MTLPixelFormat kMtlPixelFormatRGBA8Unorm_sRGB       = MTLPixelFormat(71);
	constexpr MTLPixelFormat kMtlPixelFormatRGBA8Snorm            = MTLPixelFormat(72);
	constexpr MTLPixelFormat kMtlPixelFormatRGBA8Uint             = MTLPixelFormat(73);
	constexpr MTLPixelFormat kMtlPixelFormatRGBA8Sint             = MTLPixelFormat(74);
	constexpr MTLPixelFormat kMtlPixelFormatBGRA8Unorm            = MTLPixelFormat(80);
	constexpr MTLPixelFormat kMtlPixelFormatBGRA8Unorm_sRGB       = MTLPixelFormat(81);
	constexpr MTLPixelFormat kMtlPixelFormatRGB10A2Unorm          = MTLPixelFormat(90);
	constexpr MTLPixelFormat kMtlPixelFormatRG11B10Float          = MTLPixelFormat(92);
	constexpr MTLPixelFormat kMtlPixelFormatRGB9E5Float           = MTLPixelFormat(93);
	constexpr MTLPixelFormat kMtlPixelFormatRG32Uint              = MTLPixelFormat(103);
	constexpr MTLPixelFormat kMtlPixelFormatRG32Sint              = MTLPixelFormat(104);
	constexpr MTLPixelFormat kMtlPixelFormatRG32Float             = MTLPixelFormat(105);
	constexpr MTLPixelFormat kMtlPixelFormatRGBA16Unorm           = MTLPixelFormat(110);
	constexpr MTLPixelFormat kMtlPixelFormatRGBA16Snorm           = MTLPixelFormat(112);
	constexpr MTLPixelFormat kMtlPixelFormatRGBA16Uint            = MTLPixelFormat(113);
	constexpr MTLPixelFormat kMtlPixelFormatRGBA16Sint            = MTLPixelFormat(114);
	constexpr MTLPixelFormat kMtlPixelFormatRGBA16Float           = MTLPixelFormat(115);
	constexpr MTLPixelFormat kMtlPixelFormatRGBA32Uint            = MTLPixelFormat(123);
	constexpr MTLPixelFormat kMtlPixelFormatRGBA32Sint            = MTLPixelFormat(124);
	constexpr MTLPixelFormat kMtlPixelFormatRGBA32Float           = MTLPixelFormat(125);
	constexpr MTLPixelFormat kMtlPixelFormatBC1_RGBA              = MTLPixelFormat(130);
	constexpr MTLPixelFormat kMtlPixelFormatBC1_RGBA_sRGB         = MTLPixelFormat(131);
	constexpr MTLPixelFormat kMtlPixelFormatBC2_RGBA              = MTLPixelFormat(132);
	constexpr MTLPixelFormat kMtlPixelFormatBC2_RGBA_sRGB         = MTLPixelFormat(133);
	constexpr MTLPixelFormat kMtlPixelFormatBC3_RGBA              = MTLPixelFormat(134);
	constexpr MTLPixelFormat kMtlPixelFormatBC3_RGBA_sRGB         = MTLPixelFormat(135);
	constexpr MTLPixelFormat kMtlPixelFormatBC4_RUnorm            = MTLPixelFormat(140);
	constexpr MTLPixelFormat kMtlPixelFormatBC5_RGUnorm           = MTLPixelFormat(142);
	constexpr MTLPixelFormat kMtlPixelFormatBC6H_RGBFloat         = MTLPixelFormat(150);
	constexpr MTLPixelFormat kMtlPixelFormatBC7_RGBAUnorm         = MTLPixelFormat(152);
	constexpr MTLPixelFormat kMtlPixelFormatBC7_RGBAUnorm_sRGB    = MTLPixelFormat(153);
	constexpr MTLPixelFormat kMtlPixelFormatPVRTC_RGB_2BPP        = MTLPixelFormat(160);
	constexpr MTLPixelFormat kMtlPixelFormatPVRTC_RGB_2BPP_sRGB   = MTLPixelFormat(161);
	constexpr MTLPixelFormat kMtlPixelFormatPVRTC_RGB_4BPP        = MTLPixelFormat(162);
	constexpr MTLPixelFormat kMtlPixelFormatPVRTC_RGB_4BPP_sRGB   = MTLPixelFormat(163);
	constexpr MTLPixelFormat kMtlPixelFormatPVRTC_RGBA_2BPP       = MTLPixelFormat(164);
	constexpr MTLPixelFormat kMtlPixelFormatPVRTC_RGBA_2BPP_sRGB  = MTLPixelFormat(165);
	constexpr MTLPixelFormat kMtlPixelFormatPVRTC_RGBA_4BPP       = MTLPixelFormat(166);
	constexpr MTLPixelFormat kMtlPixelFormatPVRTC_RGBA_4BPP_sRGB  = MTLPixelFormat(167);
	constexpr MTLPixelFormat kMtlPixelFormatEAC_RGBA8             = MTLPixelFormat(178);
	constexpr MTLPixelFormat kMtlPixelFormatEAC_RGBA8_sRGB        = MTLPixelFormat(179);
	constexpr MTLPixelFormat kMtlPixelFormatETC2_RGB8             = MTLPixelFormat(180);
	constexpr MTLPixelFormat kMtlPixelFormatETC2_RGB8_sRGB        = MTLPixelFormat(181);
	constexpr MTLPixelFormat kMtlPixelFormatETC2_RGB8A1           = MTLPixelFormat(182);
	constexpr MTLPixelFormat kMtlPixelFormatETC2_RGB8A1_sRGB      = MTLPixelFormat(183);
	constexpr MTLPixelFormat kMtlPixelFormatASTC_4x4_sRGB         = MTLPixelFormat(186);
	constexpr MTLPixelFormat kMtlPixelFormatASTC_5x4_sRGB         = MTLPixelFormat(187);
	constexpr MTLPixelFormat kMtlPixelFormatASTC_5x5_sRGB         = MTLPixelFormat(188);
	constexpr MTLPixelFormat kMtlPixelFormatASTC_6x5_sRGB         = MTLPixelFormat(189);
	constexpr MTLPixelFormat kMtlPixelFormatASTC_6x6_sRGB         = MTLPixelFormat(190);
	constexpr MTLPixelFormat kMtlPixelFormatASTC_8x5_sRGB         = MTLPixelFormat(192);
	constexpr MTLPixelFormat kMtlPixelFormatASTC_8x6_sRGB         = MTLPixelFormat(193);
	constexpr MTLPixelFormat kMtlPixelFormatASTC_8x8_sRGB         = MTLPixelFormat(194);
	constexpr MTLPixelFormat kMtlPixelFormatASTC_10x5_sRGB        = MTLPixelFormat(195);
	constexpr MTLPixelFormat kMtlPixelFormatASTC_10x6_sRGB        = MTLPixelFormat(196);
	constexpr MTLPixelFormat kMtlPixelFormatASTC_10x8_sRGB        = MTLPixelFormat(197);
	constexpr MTLPixelFormat kMtlPixelFormatASTC_10x10_sRGB       = MTLPixelFormat(198);
	constexpr MTLPixelFormat kMtlPixelFormatASTC_12x10_sRGB       = MTLPixelFormat(199);
	constexpr MTLPixelFormat kMtlPixelFormatASTC_12x12_sRGB       = MTLPixelFormat(200);
	constexpr MTLPixelFormat kMtlPixelFormatASTC_4x4_LDR          = MTLPixelFormat(204);
	constexpr MTLPixelFormat kMtlPixelFormatASTC_5x4_LDR          = MTLPixelFormat(205);
	constexpr MTLPixelFormat kMtlPixelFormatASTC_5x5_LDR          = MTLPixelFormat(206);
	constexpr MTLPixelFormat kMtlPixelFormatASTC_6x5_LDR          = MTLPixelFormat(207);
	constexpr MTLPixelFormat kMtlPixelFormatASTC_6x6_LDR          = MTLPixelFormat(208);
	constexpr MTLPixelFormat kMtlPixelFormatASTC_8x5_LDR          = MTLPixelFormat(210);
	constexpr MTLPixelFormat kMtlPixelFormatASTC_8x6_LDR          = MTLPixelFormat(211);
	constexpr MTLPixelFormat kMtlPixelFormatASTC_8x8_LDR          = MTLPixelFormat(212);
	constexpr MTLPixelFormat kMtlPixelFormatASTC_10x5_LDR         = MTLPixelFormat(213);
	constexpr MTLPixelFormat kMtlPixelFormatASTC_10x6_LDR         = MTLPixelFormat(214);
	constexpr MTLPixelFormat kMtlPixelFormatASTC_10x8_LDR         = MTLPixelFormat(215);
	constexpr MTLPixelFormat kMtlPixelFormatASTC_10x10_LDR        = MTLPixelFormat(216);
	constexpr MTLPixelFormat kMtlPixelFormatASTC_12x10_LDR        = MTLPixelFormat(217);
	constexpr MTLPixelFormat kMtlPixelFormatASTC_12x12_LDR        = MTLPixelFormat(218);
	constexpr MTLPixelFormat kMtlPixelFormatDepth16Unorm          = MTLPixelFormat(250);
	constexpr MTLPixelFormat kMtlPixelFormatDepth32Float          = MTLPixelFormat(252);
	constexpr MTLPixelFormat kMtlPixelFormatStencil8              = MTLPixelFormat(253);
	constexpr MTLPixelFormat kMtlPixelFormatDepth24Unorm_Stencil8 = MTLPixelFormat(255);
}

// Copied from renderer_mtl.cpp
namespace
{
    struct TextureFormatInfo
    {
        MTLPixelFormat m_fmt;
        MTLPixelFormat m_fmtSrgb;
    };

	static TextureFormatInfo s_textureFormat[] =
	{
		{ kMtlPixelFormatBC1_RGBA,              kMtlPixelFormatBC1_RGBA_sRGB,        }, // BC1
		{ kMtlPixelFormatBC2_RGBA,              kMtlPixelFormatBC2_RGBA_sRGB,        }, // BC2
		{ kMtlPixelFormatBC3_RGBA,              kMtlPixelFormatBC3_RGBA_sRGB,        }, // BC3
		{ kMtlPixelFormatBC4_RUnorm,            kMtlPixelFormatInvalid,              }, // BC4
		{ kMtlPixelFormatBC5_RGUnorm,           kMtlPixelFormatInvalid,              }, // BC5
		{ kMtlPixelFormatBC6H_RGBFloat,         kMtlPixelFormatInvalid,              }, // BC6H
		{ kMtlPixelFormatBC7_RGBAUnorm,         kMtlPixelFormatBC7_RGBAUnorm_sRGB,   }, // BC7
		{ kMtlPixelFormatInvalid,               kMtlPixelFormatInvalid,              }, // ETC1
		{ kMtlPixelFormatETC2_RGB8,             kMtlPixelFormatETC2_RGB8_sRGB,       }, // ETC2
		{ kMtlPixelFormatEAC_RGBA8,             kMtlPixelFormatEAC_RGBA8_sRGB,       }, // ETC2A
		{ kMtlPixelFormatETC2_RGB8A1,           kMtlPixelFormatETC2_RGB8A1_sRGB,     }, // ETC2A1
		{ kMtlPixelFormatPVRTC_RGB_2BPP,        kMtlPixelFormatPVRTC_RGB_2BPP_sRGB,  }, // PTC12
		{ kMtlPixelFormatPVRTC_RGB_4BPP,        kMtlPixelFormatPVRTC_RGB_4BPP_sRGB,  }, // PTC14
		{ kMtlPixelFormatPVRTC_RGBA_2BPP,       kMtlPixelFormatPVRTC_RGBA_2BPP_sRGB, }, // PTC12A
		{ kMtlPixelFormatPVRTC_RGBA_4BPP,       kMtlPixelFormatPVRTC_RGBA_4BPP_sRGB, }, // PTC14A
		{ kMtlPixelFormatInvalid,               kMtlPixelFormatInvalid,              }, // PTC22
		{ kMtlPixelFormatInvalid,               kMtlPixelFormatInvalid,              }, // PTC24
		{ kMtlPixelFormatInvalid,               kMtlPixelFormatInvalid,              }, // ATC
		{ kMtlPixelFormatInvalid,               kMtlPixelFormatInvalid,              }, // ATCE
		{ kMtlPixelFormatInvalid,               kMtlPixelFormatInvalid,              }, // ATCI
		{ kMtlPixelFormatASTC_4x4_LDR,          kMtlPixelFormatASTC_4x4_sRGB,        }, // ASTC4x4
		{ kMtlPixelFormatASTC_5x4_LDR,          kMtlPixelFormatASTC_5x4_sRGB,        }, // ASTC5x4
		{ kMtlPixelFormatASTC_5x5_LDR,          kMtlPixelFormatASTC_5x5_sRGB,        }, // ASTC5x5
		{ kMtlPixelFormatASTC_6x5_LDR,          kMtlPixelFormatASTC_6x5_sRGB,        }, // ASTC6x5
		{ kMtlPixelFormatASTC_6x6_LDR,          kMtlPixelFormatASTC_6x6_sRGB,        }, // ASTC6x6
		{ kMtlPixelFormatASTC_8x5_LDR,          kMtlPixelFormatASTC_8x5_sRGB,        }, // ASTC8x5
		{ kMtlPixelFormatASTC_8x6_LDR,          kMtlPixelFormatASTC_8x6_sRGB,        }, // ASTC8x6
		{ kMtlPixelFormatASTC_8x8_LDR,          kMtlPixelFormatASTC_8x8_sRGB,        }, // ASTC8x8
		{ kMtlPixelFormatASTC_10x5_LDR,         kMtlPixelFormatASTC_10x5_sRGB,       }, // ASTC10x5
		{ kMtlPixelFormatASTC_10x6_LDR,         kMtlPixelFormatASTC_10x6_sRGB,       }, // ASTC10x6
		{ kMtlPixelFormatASTC_10x8_LDR,         kMtlPixelFormatASTC_10x8_sRGB,       }, // ASTC10x8
		{ kMtlPixelFormatASTC_10x10_LDR,        kMtlPixelFormatASTC_10x10_sRGB,      }, // ASTC10x10
		{ kMtlPixelFormatASTC_12x10_LDR,        kMtlPixelFormatASTC_12x10_sRGB,      }, // ASTC12x10
		{ kMtlPixelFormatASTC_12x12_LDR,        kMtlPixelFormatASTC_12x12_sRGB,      }, // ASTC12x12
		{ kMtlPixelFormatInvalid,               kMtlPixelFormatInvalid,              }, // Unknown
		{ kMtlPixelFormatInvalid,               kMtlPixelFormatInvalid,              }, // R1
		{ kMtlPixelFormatA8Unorm,               kMtlPixelFormatInvalid,              }, // A8
		{ kMtlPixelFormatR8Unorm,               kMtlPixelFormatR8Unorm_sRGB,         }, // R8
		{ kMtlPixelFormatR8Sint,                kMtlPixelFormatInvalid,              }, // R8I
		{ kMtlPixelFormatR8Uint,                kMtlPixelFormatInvalid,              }, // R8U
		{ kMtlPixelFormatR8Snorm,               kMtlPixelFormatInvalid,              }, // R8S
		{ kMtlPixelFormatR16Unorm,              kMtlPixelFormatInvalid,              }, // R16
		{ kMtlPixelFormatR16Sint,               kMtlPixelFormatInvalid,              }, // R16I
		{ kMtlPixelFormatR16Uint,               kMtlPixelFormatInvalid,              }, // R16U
		{ kMtlPixelFormatR16Float,              kMtlPixelFormatInvalid,              }, // R16F
		{ kMtlPixelFormatR16Snorm,              kMtlPixelFormatInvalid,              }, // R16S
		{ kMtlPixelFormatR32Sint,               kMtlPixelFormatInvalid,              }, // R32I
		{ kMtlPixelFormatR32Uint,               kMtlPixelFormatInvalid,              }, // R32U
		{ kMtlPixelFormatR32Float,              kMtlPixelFormatInvalid,              }, // R32F
		{ kMtlPixelFormatRG8Unorm,              kMtlPixelFormatRG8Unorm_sRGB,        }, // RG8
		{ kMtlPixelFormatRG8Sint,               kMtlPixelFormatInvalid,              }, // RG8I
		{ kMtlPixelFormatRG8Uint,               kMtlPixelFormatInvalid,              }, // RG8U
		{ kMtlPixelFormatRG8Snorm,              kMtlPixelFormatInvalid,              }, // RG8S
		{ kMtlPixelFormatRG16Unorm,             kMtlPixelFormatInvalid,              }, // RG16
		{ kMtlPixelFormatRG16Sint,              kMtlPixelFormatInvalid,              }, // RG16I
		{ kMtlPixelFormatRG16Uint,              kMtlPixelFormatInvalid,              }, // RG16U
		{ kMtlPixelFormatRG16Float,             kMtlPixelFormatInvalid,              }, // RG16F
		{ kMtlPixelFormatRG16Snorm,             kMtlPixelFormatInvalid,              }, // RG16S
		{ kMtlPixelFormatRG32Sint,              kMtlPixelFormatInvalid,              }, // RG32I
		{ kMtlPixelFormatRG32Uint,              kMtlPixelFormatInvalid,              }, // RG32U
		{ kMtlPixelFormatRG32Float,             kMtlPixelFormatInvalid,              }, // RG32F
		{ kMtlPixelFormatInvalid,               kMtlPixelFormatInvalid,              }, // RGB8
		{ kMtlPixelFormatInvalid,               kMtlPixelFormatInvalid,              }, // RGB8I
		{ kMtlPixelFormatInvalid,               kMtlPixelFormatInvalid,              }, // RGB8U
		{ kMtlPixelFormatInvalid,               kMtlPixelFormatInvalid,              }, // RGB8S
		{ kMtlPixelFormatRGB9E5Float,           kMtlPixelFormatInvalid,              }, // RGB9E5F
		{ kMtlPixelFormatBGRA8Unorm,            kMtlPixelFormatBGRA8Unorm_sRGB,      }, // BGRA8
		{ kMtlPixelFormatRGBA8Unorm,            kMtlPixelFormatRGBA8Unorm_sRGB,      }, // RGBA8
		{ kMtlPixelFormatRGBA8Sint,             kMtlPixelFormatInvalid,              }, // RGBA8I
		{ kMtlPixelFormatRGBA8Uint,             kMtlPixelFormatInvalid,              }, // RGBA8U
		{ kMtlPixelFormatRGBA8Snorm,            kMtlPixelFormatInvalid,              }, // RGBA8S
		{ kMtlPixelFormatRGBA16Unorm,           kMtlPixelFormatInvalid,              }, // RGBA16
		{ kMtlPixelFormatRGBA16Sint,            kMtlPixelFormatInvalid,              }, // RGBA16I
		{ kMtlPixelFormatRGBA16Uint,            kMtlPixelFormatInvalid,              }, // RGBA16U
		{ kMtlPixelFormatRGBA16Float,           kMtlPixelFormatInvalid,              }, // RGBA16F
		{ kMtlPixelFormatRGBA16Snorm,           kMtlPixelFormatInvalid,              }, // RGBA16S
		{ kMtlPixelFormatRGBA32Sint,            kMtlPixelFormatInvalid,              }, // RGBA32I
		{ kMtlPixelFormatRGBA32Uint,            kMtlPixelFormatInvalid,              }, // RGBA32U
		{ kMtlPixelFormatRGBA32Float,           kMtlPixelFormatInvalid,              }, // RGBA32F
		{ kMtlPixelFormatB5G6R5Unorm,           kMtlPixelFormatInvalid,              }, // B5G6R5
		{ kMtlPixelFormatB5G6R5Unorm,           kMtlPixelFormatInvalid,              }, // R5G6B5
		{ kMtlPixelFormatABGR4Unorm,            kMtlPixelFormatInvalid,              }, // BGRA4
		{ kMtlPixelFormatABGR4Unorm,            kMtlPixelFormatInvalid,              }, // RGBA4
		{ kMtlPixelFormatBGR5A1Unorm,           kMtlPixelFormatInvalid,              }, // BGR5A1
		{ kMtlPixelFormatBGR5A1Unorm,           kMtlPixelFormatInvalid,              }, // RGB5A1
		{ kMtlPixelFormatRGB10A2Unorm,          kMtlPixelFormatInvalid,              }, // RGB10A2
		{ kMtlPixelFormatRG11B10Float,          kMtlPixelFormatInvalid,              }, // RG11B10F
		{ kMtlPixelFormatInvalid,               kMtlPixelFormatInvalid,              }, // UnknownDepth
		{ kMtlPixelFormatDepth16Unorm,          kMtlPixelFormatInvalid,              }, // D16
		{ kMtlPixelFormatDepth32Float,          kMtlPixelFormatInvalid,              }, // D24
		{ kMtlPixelFormatDepth24Unorm_Stencil8, kMtlPixelFormatInvalid,              }, // D24S8
		{ kMtlPixelFormatDepth32Float,          kMtlPixelFormatInvalid,              }, // D32
		{ kMtlPixelFormatDepth32Float,          kMtlPixelFormatInvalid,              }, // D16F
		{ kMtlPixelFormatDepth32Float,          kMtlPixelFormatInvalid,              }, // D24F
		{ kMtlPixelFormatDepth32Float,          kMtlPixelFormatInvalid,              }, // D32F
		{ kMtlPixelFormatStencil8,              kMtlPixelFormatInvalid,              }, // D0S8
	};
	static_assert(bgfx::TextureFormat::Count == BX_COUNTOF(s_textureFormat) );
}

// clang-format on

namespace Babylon::Plugins
{
    class ExternalTexture::Impl final : public ImplBase
    {
    public:
        // Implemented in ExternalTexture_Shared.h
        Impl(Graphics::TextureT, std::optional<Graphics::TextureFormatT>);
        void Update(Graphics::TextureT, std::optional<Graphics::TextureFormatT>, std::optional<uint16_t>);

        Graphics::TextureT Get() const
        {
            return m_ptr;
        }

    private:
        static void GetInfo(Graphics::TextureT ptr, std::optional<Graphics::TextureFormatT> overrideFormat, Info& info)
        {
            if (ptr.textureType != MTLTextureType2D && ptr.textureType != MTLTextureType2DMultisample)
            {
                throw std::runtime_error{"Unsupported texture type"};
            }

            info.Width = static_cast<uint16_t>(ptr.width);
            info.Height = static_cast<uint16_t>(ptr.height);
            info.MipLevels = static_cast<uint16_t>(ptr.mipmapLevelCount);
            info.NumLayers = static_cast<uint16_t>(ptr.arrayLength);

            if ((ptr.usage & MTLTextureUsageRenderTarget) != 0)
            {
                info.Flags |= BGFX_TEXTURE_RT;

                if (ptr.sampleCount > 1)
                {
                    info.Flags |= BGFX_TEXTURE_MSAA_SAMPLE | RenderTargetSamplesToBgfxMsaaFlag(ptr.sampleCount);
                }
            }

            const auto targetFormat = overrideFormat.has_value() ? overrideFormat.value() : ptr.pixelFormat;
            for (size_t i = 0; i < BX_COUNTOF(s_textureFormat); ++i)
            {
                const auto& format = s_textureFormat[i];
                if (format.m_fmt == targetFormat || format.m_fmtSrgb == targetFormat)
                {
                    info.Format = static_cast<bgfx::TextureFormat::Enum>(i);
                    if (format.m_fmtSrgb == targetFormat)
                    {
                        info.Flags |= BGFX_TEXTURE_SRGB;
                    }
                    break;
                }
            }
        }

        void Set(Graphics::TextureT ptr)
        {
            m_ptr = ptr;
        }

        id<MTLTexture> m_ptr;
    };
}

#include "ExternalTexture_Shared.h"
