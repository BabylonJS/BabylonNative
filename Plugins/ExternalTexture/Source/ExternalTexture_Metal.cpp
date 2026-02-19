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
	constexpr MTL::PixelFormat kMtlPixelFormatInvalid               = MTL::PixelFormat(0);
	constexpr MTL::PixelFormat kMtlPixelFormatA8Unorm               = MTL::PixelFormat(1);
	constexpr MTL::PixelFormat kMtlPixelFormatR8Unorm               = MTL::PixelFormat(10);
	constexpr MTL::PixelFormat kMtlPixelFormatR8Unorm_sRGB          = MTL::PixelFormat(11);
	constexpr MTL::PixelFormat kMtlPixelFormatR8Snorm               = MTL::PixelFormat(12);
	constexpr MTL::PixelFormat kMtlPixelFormatR8Uint                = MTL::PixelFormat(13);
	constexpr MTL::PixelFormat kMtlPixelFormatR8Sint                = MTL::PixelFormat(14);
	constexpr MTL::PixelFormat kMtlPixelFormatR16Unorm              = MTL::PixelFormat(20);
	constexpr MTL::PixelFormat kMtlPixelFormatR16Snorm              = MTL::PixelFormat(22);
	constexpr MTL::PixelFormat kMtlPixelFormatR16Uint               = MTL::PixelFormat(23);
	constexpr MTL::PixelFormat kMtlPixelFormatR16Sint               = MTL::PixelFormat(24);
	constexpr MTL::PixelFormat kMtlPixelFormatR16Float              = MTL::PixelFormat(25);
	constexpr MTL::PixelFormat kMtlPixelFormatRG8Unorm              = MTL::PixelFormat(30);
	constexpr MTL::PixelFormat kMtlPixelFormatRG8Unorm_sRGB         = MTL::PixelFormat(31);
	constexpr MTL::PixelFormat kMtlPixelFormatRG8Snorm              = MTL::PixelFormat(32);
	constexpr MTL::PixelFormat kMtlPixelFormatRG8Uint               = MTL::PixelFormat(33);
	constexpr MTL::PixelFormat kMtlPixelFormatRG8Sint               = MTL::PixelFormat(34);
	constexpr MTL::PixelFormat kMtlPixelFormatB5G6R5Unorm           = MTL::PixelFormat(40);
	constexpr MTL::PixelFormat kMtlPixelFormatABGR4Unorm            = MTL::PixelFormat(42);
	constexpr MTL::PixelFormat kMtlPixelFormatBGR5A1Unorm           = MTL::PixelFormat(43);
	constexpr MTL::PixelFormat kMtlPixelFormatR32Uint               = MTL::PixelFormat(53);
	constexpr MTL::PixelFormat kMtlPixelFormatR32Sint               = MTL::PixelFormat(54);
	constexpr MTL::PixelFormat kMtlPixelFormatR32Float              = MTL::PixelFormat(55);
	constexpr MTL::PixelFormat kMtlPixelFormatRG16Unorm             = MTL::PixelFormat(60);
	constexpr MTL::PixelFormat kMtlPixelFormatRG16Snorm             = MTL::PixelFormat(62);
	constexpr MTL::PixelFormat kMtlPixelFormatRG16Uint              = MTL::PixelFormat(63);
	constexpr MTL::PixelFormat kMtlPixelFormatRG16Sint              = MTL::PixelFormat(64);
	constexpr MTL::PixelFormat kMtlPixelFormatRG16Float             = MTL::PixelFormat(65);
	constexpr MTL::PixelFormat kMtlPixelFormatRGBA8Unorm            = MTL::PixelFormat(70);
	constexpr MTL::PixelFormat kMtlPixelFormatRGBA8Unorm_sRGB       = MTL::PixelFormat(71);
	constexpr MTL::PixelFormat kMtlPixelFormatRGBA8Snorm            = MTL::PixelFormat(72);
	constexpr MTL::PixelFormat kMtlPixelFormatRGBA8Uint             = MTL::PixelFormat(73);
	constexpr MTL::PixelFormat kMtlPixelFormatRGBA8Sint             = MTL::PixelFormat(74);
	constexpr MTL::PixelFormat kMtlPixelFormatBGRA8Unorm            = MTL::PixelFormat(80);
	constexpr MTL::PixelFormat kMtlPixelFormatBGRA8Unorm_sRGB       = MTL::PixelFormat(81);
	constexpr MTL::PixelFormat kMtlPixelFormatRGB10A2Unorm          = MTL::PixelFormat(90);
	constexpr MTL::PixelFormat kMtlPixelFormatRG11B10Float          = MTL::PixelFormat(92);
	constexpr MTL::PixelFormat kMtlPixelFormatRGB9E5Float           = MTL::PixelFormat(93);
	constexpr MTL::PixelFormat kMtlPixelFormatRG32Uint              = MTL::PixelFormat(103);
	constexpr MTL::PixelFormat kMtlPixelFormatRG32Sint              = MTL::PixelFormat(104);
	constexpr MTL::PixelFormat kMtlPixelFormatRG32Float             = MTL::PixelFormat(105);
	constexpr MTL::PixelFormat kMtlPixelFormatRGBA16Unorm           = MTL::PixelFormat(110);
	constexpr MTL::PixelFormat kMtlPixelFormatRGBA16Snorm           = MTL::PixelFormat(112);
	constexpr MTL::PixelFormat kMtlPixelFormatRGBA16Uint            = MTL::PixelFormat(113);
	constexpr MTL::PixelFormat kMtlPixelFormatRGBA16Sint            = MTL::PixelFormat(114);
	constexpr MTL::PixelFormat kMtlPixelFormatRGBA16Float           = MTL::PixelFormat(115);
	constexpr MTL::PixelFormat kMtlPixelFormatRGBA32Uint            = MTL::PixelFormat(123);
	constexpr MTL::PixelFormat kMtlPixelFormatRGBA32Sint            = MTL::PixelFormat(124);
	constexpr MTL::PixelFormat kMtlPixelFormatRGBA32Float           = MTL::PixelFormat(125);
	constexpr MTL::PixelFormat kMtlPixelFormatBC1_RGBA              = MTL::PixelFormat(130);
	constexpr MTL::PixelFormat kMtlPixelFormatBC1_RGBA_sRGB         = MTL::PixelFormat(131);
	constexpr MTL::PixelFormat kMtlPixelFormatBC2_RGBA              = MTL::PixelFormat(132);
	constexpr MTL::PixelFormat kMtlPixelFormatBC2_RGBA_sRGB         = MTL::PixelFormat(133);
	constexpr MTL::PixelFormat kMtlPixelFormatBC3_RGBA              = MTL::PixelFormat(134);
	constexpr MTL::PixelFormat kMtlPixelFormatBC3_RGBA_sRGB         = MTL::PixelFormat(135);
	constexpr MTL::PixelFormat kMtlPixelFormatBC4_RUnorm            = MTL::PixelFormat(140);
	constexpr MTL::PixelFormat kMtlPixelFormatBC5_RGUnorm           = MTL::PixelFormat(142);
	constexpr MTL::PixelFormat kMtlPixelFormatBC6H_RGBFloat         = MTL::PixelFormat(150);
	constexpr MTL::PixelFormat kMtlPixelFormatBC7_RGBAUnorm         = MTL::PixelFormat(152);
	constexpr MTL::PixelFormat kMtlPixelFormatBC7_RGBAUnorm_sRGB    = MTL::PixelFormat(153);
	constexpr MTL::PixelFormat kMtlPixelFormatPVRTC_RGB_2BPP        = MTL::PixelFormat(160);
	constexpr MTL::PixelFormat kMtlPixelFormatPVRTC_RGB_2BPP_sRGB   = MTL::PixelFormat(161);
	constexpr MTL::PixelFormat kMtlPixelFormatPVRTC_RGB_4BPP        = MTL::PixelFormat(162);
	constexpr MTL::PixelFormat kMtlPixelFormatPVRTC_RGB_4BPP_sRGB   = MTL::PixelFormat(163);
	constexpr MTL::PixelFormat kMtlPixelFormatPVRTC_RGBA_2BPP       = MTL::PixelFormat(164);
	constexpr MTL::PixelFormat kMtlPixelFormatPVRTC_RGBA_2BPP_sRGB  = MTL::PixelFormat(165);
	constexpr MTL::PixelFormat kMtlPixelFormatPVRTC_RGBA_4BPP       = MTL::PixelFormat(166);
	constexpr MTL::PixelFormat kMtlPixelFormatPVRTC_RGBA_4BPP_sRGB  = MTL::PixelFormat(167);
	constexpr MTL::PixelFormat kMtlPixelFormatEAC_RGBA8             = MTL::PixelFormat(178);
	constexpr MTL::PixelFormat kMtlPixelFormatEAC_RGBA8_sRGB        = MTL::PixelFormat(179);
	constexpr MTL::PixelFormat kMtlPixelFormatETC2_RGB8             = MTL::PixelFormat(180);
	constexpr MTL::PixelFormat kMtlPixelFormatETC2_RGB8_sRGB        = MTL::PixelFormat(181);
	constexpr MTL::PixelFormat kMtlPixelFormatETC2_RGB8A1           = MTL::PixelFormat(182);
	constexpr MTL::PixelFormat kMtlPixelFormatETC2_RGB8A1_sRGB      = MTL::PixelFormat(183);
	constexpr MTL::PixelFormat kMtlPixelFormatASTC_4x4_sRGB         = MTL::PixelFormat(186);
	constexpr MTL::PixelFormat kMtlPixelFormatASTC_5x4_sRGB         = MTL::PixelFormat(187);
	constexpr MTL::PixelFormat kMtlPixelFormatASTC_5x5_sRGB         = MTL::PixelFormat(188);
	constexpr MTL::PixelFormat kMtlPixelFormatASTC_6x5_sRGB         = MTL::PixelFormat(189);
	constexpr MTL::PixelFormat kMtlPixelFormatASTC_6x6_sRGB         = MTL::PixelFormat(190);
	constexpr MTL::PixelFormat kMtlPixelFormatASTC_8x5_sRGB         = MTL::PixelFormat(192);
	constexpr MTL::PixelFormat kMtlPixelFormatASTC_8x6_sRGB         = MTL::PixelFormat(193);
	constexpr MTL::PixelFormat kMtlPixelFormatASTC_8x8_sRGB         = MTL::PixelFormat(194);
	constexpr MTL::PixelFormat kMtlPixelFormatASTC_10x5_sRGB        = MTL::PixelFormat(195);
	constexpr MTL::PixelFormat kMtlPixelFormatASTC_10x6_sRGB        = MTL::PixelFormat(196);
	constexpr MTL::PixelFormat kMtlPixelFormatASTC_10x8_sRGB        = MTL::PixelFormat(197);
	constexpr MTL::PixelFormat kMtlPixelFormatASTC_10x10_sRGB       = MTL::PixelFormat(198);
	constexpr MTL::PixelFormat kMtlPixelFormatASTC_12x10_sRGB       = MTL::PixelFormat(199);
	constexpr MTL::PixelFormat kMtlPixelFormatASTC_12x12_sRGB       = MTL::PixelFormat(200);
	constexpr MTL::PixelFormat kMtlPixelFormatASTC_4x4_LDR          = MTL::PixelFormat(204);
	constexpr MTL::PixelFormat kMtlPixelFormatASTC_5x4_LDR          = MTL::PixelFormat(205);
	constexpr MTL::PixelFormat kMtlPixelFormatASTC_5x5_LDR          = MTL::PixelFormat(206);
	constexpr MTL::PixelFormat kMtlPixelFormatASTC_6x5_LDR          = MTL::PixelFormat(207);
	constexpr MTL::PixelFormat kMtlPixelFormatASTC_6x6_LDR          = MTL::PixelFormat(208);
	constexpr MTL::PixelFormat kMtlPixelFormatASTC_8x5_LDR          = MTL::PixelFormat(210);
	constexpr MTL::PixelFormat kMtlPixelFormatASTC_8x6_LDR          = MTL::PixelFormat(211);
	constexpr MTL::PixelFormat kMtlPixelFormatASTC_8x8_LDR          = MTL::PixelFormat(212);
	constexpr MTL::PixelFormat kMtlPixelFormatASTC_10x5_LDR         = MTL::PixelFormat(213);
	constexpr MTL::PixelFormat kMtlPixelFormatASTC_10x6_LDR         = MTL::PixelFormat(214);
	constexpr MTL::PixelFormat kMtlPixelFormatASTC_10x8_LDR         = MTL::PixelFormat(215);
	constexpr MTL::PixelFormat kMtlPixelFormatASTC_10x10_LDR        = MTL::PixelFormat(216);
	constexpr MTL::PixelFormat kMtlPixelFormatASTC_12x10_LDR        = MTL::PixelFormat(217);
	constexpr MTL::PixelFormat kMtlPixelFormatASTC_12x12_LDR        = MTL::PixelFormat(218);
	constexpr MTL::PixelFormat kMtlPixelFormatDepth16Unorm          = MTL::PixelFormat(250);
	constexpr MTL::PixelFormat kMtlPixelFormatDepth32Float          = MTL::PixelFormat(252);
	constexpr MTL::PixelFormat kMtlPixelFormatStencil8              = MTL::PixelFormat(253);
	constexpr MTL::PixelFormat kMtlPixelFormatDepth24Unorm_Stencil8 = MTL::PixelFormat(255);
}

// Copied from renderer_mtl.cpp
namespace
{
    struct TextureFormatInfo
    {
        MTL::PixelFormat m_fmt;
        MTL::PixelFormat m_fmtSrgb;
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
            return m_ptr.get();
        }

    private:
        static void GetInfo(Graphics::TextureT ptr, std::optional<Graphics::TextureFormatT> overrideFormat, Info& info)
        {
            if (ptr->textureType() != MTL::TextureType2D && ptr->textureType() != MTL::TextureType2DMultisample)
            {
                throw std::runtime_error{"Unsupported texture type"};
            }

            info.Width = static_cast<uint16_t>(ptr->width());
            info.Height = static_cast<uint16_t>(ptr->height());
            info.MipLevels = static_cast<uint16_t>(ptr->mipmapLevelCount());
            info.NumLayers = static_cast<uint16_t>(ptr->arrayLength());

            if ((ptr->usage() & MTL::TextureUsageRenderTarget) != 0)
            {
                info.Flags |= BGFX_TEXTURE_RT;

                if (ptr->sampleCount() > 1)
                {
                    info.Flags |= BGFX_TEXTURE_MSAA_SAMPLE | RenderTargetSamplesToBgfxMsaaFlag(ptr->sampleCount());
                }
            }

            const auto targetFormat = overrideFormat.has_value() ? overrideFormat.value() : ptr->pixelFormat();
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
            m_ptr = RetainPtr(ptr);
        }

        NS::SharedPtr<MTL::Texture> m_ptr;
    };
}

#include "ExternalTexture_Shared.h"
