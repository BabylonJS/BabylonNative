#pragma once

#include <bgfx/bgfx.h>
#include <bx/bx.h>
#include <d3d11.h>

#include <optional>

// clang-format off

namespace Babylon::Graphics::D3D11TextureFormats
{
    // Copied from bgfx's renderer_d3d.h. These values are defined by newer Windows SDKs, but
    // Babylon Native also supports SDKs where the corresponding DXGI_FORMAT names are absent.
    inline constexpr DXGI_FORMAT DXGI_FORMAT_ASTC_4X4_UNORM        = DXGI_FORMAT(134);
    inline constexpr DXGI_FORMAT DXGI_FORMAT_ASTC_4X4_UNORM_SRGB   = DXGI_FORMAT(135);
    inline constexpr DXGI_FORMAT DXGI_FORMAT_ASTC_5X4_UNORM        = DXGI_FORMAT(138);
    inline constexpr DXGI_FORMAT DXGI_FORMAT_ASTC_5X4_UNORM_SRGB   = DXGI_FORMAT(139);
    inline constexpr DXGI_FORMAT DXGI_FORMAT_ASTC_5X5_UNORM        = DXGI_FORMAT(142);
    inline constexpr DXGI_FORMAT DXGI_FORMAT_ASTC_5X5_UNORM_SRGB   = DXGI_FORMAT(143);
    inline constexpr DXGI_FORMAT DXGI_FORMAT_ASTC_6X5_UNORM        = DXGI_FORMAT(146);
    inline constexpr DXGI_FORMAT DXGI_FORMAT_ASTC_6X5_UNORM_SRGB   = DXGI_FORMAT(147);
    inline constexpr DXGI_FORMAT DXGI_FORMAT_ASTC_6X6_UNORM        = DXGI_FORMAT(150);
    inline constexpr DXGI_FORMAT DXGI_FORMAT_ASTC_6X6_UNORM_SRGB   = DXGI_FORMAT(151);
    inline constexpr DXGI_FORMAT DXGI_FORMAT_ASTC_8X5_UNORM        = DXGI_FORMAT(154);
    inline constexpr DXGI_FORMAT DXGI_FORMAT_ASTC_8X5_UNORM_SRGB   = DXGI_FORMAT(155);
    inline constexpr DXGI_FORMAT DXGI_FORMAT_ASTC_8X6_UNORM        = DXGI_FORMAT(158);
    inline constexpr DXGI_FORMAT DXGI_FORMAT_ASTC_8X6_UNORM_SRGB   = DXGI_FORMAT(159);
    inline constexpr DXGI_FORMAT DXGI_FORMAT_ASTC_8X8_UNORM        = DXGI_FORMAT(162);
    inline constexpr DXGI_FORMAT DXGI_FORMAT_ASTC_8X8_UNORM_SRGB   = DXGI_FORMAT(163);
    inline constexpr DXGI_FORMAT DXGI_FORMAT_ASTC_10X5_UNORM       = DXGI_FORMAT(166);
    inline constexpr DXGI_FORMAT DXGI_FORMAT_ASTC_10X5_UNORM_SRGB  = DXGI_FORMAT(167);
    inline constexpr DXGI_FORMAT DXGI_FORMAT_ASTC_10X6_UNORM       = DXGI_FORMAT(170);
    inline constexpr DXGI_FORMAT DXGI_FORMAT_ASTC_10X6_UNORM_SRGB  = DXGI_FORMAT(171);
    inline constexpr DXGI_FORMAT DXGI_FORMAT_ASTC_10X8_UNORM       = DXGI_FORMAT(174);
    inline constexpr DXGI_FORMAT DXGI_FORMAT_ASTC_10X8_UNORM_SRGB  = DXGI_FORMAT(175);
    inline constexpr DXGI_FORMAT DXGI_FORMAT_ASTC_10X10_UNORM      = DXGI_FORMAT(178);
    inline constexpr DXGI_FORMAT DXGI_FORMAT_ASTC_10X10_UNORM_SRGB = DXGI_FORMAT(179);
    inline constexpr DXGI_FORMAT DXGI_FORMAT_ASTC_12X10_UNORM      = DXGI_FORMAT(182);
    inline constexpr DXGI_FORMAT DXGI_FORMAT_ASTC_12X10_UNORM_SRGB = DXGI_FORMAT(183);
    inline constexpr DXGI_FORMAT DXGI_FORMAT_ASTC_12X12_UNORM      = DXGI_FORMAT(186);
    inline constexpr DXGI_FORMAT DXGI_FORMAT_ASTC_12X12_UNORM_SRGB = DXGI_FORMAT(187);

    // Copied from renderer_d3d11.cpp
    struct TextureFormatInfo
    {
        DXGI_FORMAT m_fmt;
        DXGI_FORMAT m_fmtSrgb;
    };

    inline const TextureFormatInfo s_textureFormat[] =
    {
        { DXGI_FORMAT_BC1_UNORM,          DXGI_FORMAT_BC1_UNORM_SRGB       }, // BC1
        { DXGI_FORMAT_BC2_UNORM,          DXGI_FORMAT_BC2_UNORM_SRGB       }, // BC2
        { DXGI_FORMAT_BC3_UNORM,          DXGI_FORMAT_BC3_UNORM_SRGB       }, // BC3
        { DXGI_FORMAT_BC4_UNORM,          DXGI_FORMAT_UNKNOWN              }, // BC4
        { DXGI_FORMAT_BC4_SNORM,          DXGI_FORMAT_UNKNOWN              }, // BC4S
        { DXGI_FORMAT_BC5_UNORM,          DXGI_FORMAT_UNKNOWN              }, // BC5
        { DXGI_FORMAT_BC5_SNORM,          DXGI_FORMAT_UNKNOWN              }, // BC5S
        { DXGI_FORMAT_BC6H_SF16,          DXGI_FORMAT_UNKNOWN              }, // BC6H
        { DXGI_FORMAT_BC6H_UF16,          DXGI_FORMAT_UNKNOWN              }, // BC6HU
        { DXGI_FORMAT_BC7_UNORM,          DXGI_FORMAT_BC7_UNORM_SRGB       }, // BC7
        { DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN              }, // ETC1
        { DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN              }, // ETC2
        { DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN              }, // ETC2A
        { DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN              }, // ETC2A1
        { DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN              }, // EACR11 UNORM
        { DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN              }, // EACR11 SNORM
        { DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN              }, // EACRG11 UNORM
        { DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN              }, // EACRG11 SNORM
        { DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN              }, // PTC12
        { DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN              }, // PTC14
        { DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN              }, // PTC12A
        { DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN              }, // PTC14A
        { DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN              }, // PTC22
        { DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN              }, // PTC24
        { DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN              }, // ATC
        { DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN              }, // ATCE
        { DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN              }, // ATCI
        { DXGI_FORMAT_ASTC_4X4_UNORM,     DXGI_FORMAT_ASTC_4X4_UNORM_SRGB  }, // ASTC4x4
        { DXGI_FORMAT_ASTC_5X4_UNORM,     DXGI_FORMAT_ASTC_5X4_UNORM_SRGB  }, // ASTC5x4
        { DXGI_FORMAT_ASTC_5X5_UNORM,     DXGI_FORMAT_ASTC_5X5_UNORM_SRGB  }, // ASTC5x5
        { DXGI_FORMAT_ASTC_6X5_UNORM,     DXGI_FORMAT_ASTC_6X5_UNORM_SRGB  }, // ASTC6x5
        { DXGI_FORMAT_ASTC_6X6_UNORM,     DXGI_FORMAT_ASTC_6X6_UNORM_SRGB  }, // ASTC6x6
        { DXGI_FORMAT_ASTC_8X5_UNORM,     DXGI_FORMAT_ASTC_8X5_UNORM_SRGB  }, // ASTC8x5
        { DXGI_FORMAT_ASTC_8X6_UNORM,     DXGI_FORMAT_ASTC_8X6_UNORM_SRGB  }, // ASTC8x6
        { DXGI_FORMAT_ASTC_8X8_UNORM,     DXGI_FORMAT_ASTC_8X8_UNORM_SRGB  }, // ASTC8x8
        { DXGI_FORMAT_ASTC_10X5_UNORM,    DXGI_FORMAT_ASTC_10X5_UNORM_SRGB }, // ASTC10x5
        { DXGI_FORMAT_ASTC_10X6_UNORM,    DXGI_FORMAT_ASTC_10X6_UNORM_SRGB }, // ASTC10x6
        { DXGI_FORMAT_ASTC_10X8_UNORM,    DXGI_FORMAT_ASTC_10X8_UNORM_SRGB }, // ASTC10x8
        { DXGI_FORMAT_ASTC_10X10_UNORM,   DXGI_FORMAT_ASTC_10X10_UNORM_SRGB}, // ASTC10x10
        { DXGI_FORMAT_ASTC_12X10_UNORM,   DXGI_FORMAT_ASTC_12X10_UNORM_SRGB}, // ASTC12x10
        { DXGI_FORMAT_ASTC_12X12_UNORM,   DXGI_FORMAT_ASTC_12X12_UNORM_SRGB}, // ASTC12x12
        { DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN              }, // Unknown
        { DXGI_FORMAT_R1_UNORM,           DXGI_FORMAT_UNKNOWN              }, // R1
        { DXGI_FORMAT_A8_UNORM,           DXGI_FORMAT_UNKNOWN              }, // A8
        { DXGI_FORMAT_R8_UNORM,           DXGI_FORMAT_UNKNOWN              }, // R8
        { DXGI_FORMAT_R8_SINT,            DXGI_FORMAT_UNKNOWN              }, // R8I
        { DXGI_FORMAT_R8_UINT,            DXGI_FORMAT_UNKNOWN              }, // R8U
        { DXGI_FORMAT_R8_SNORM,           DXGI_FORMAT_UNKNOWN              }, // R8S
        { DXGI_FORMAT_R16_UNORM,          DXGI_FORMAT_UNKNOWN              }, // R16
        { DXGI_FORMAT_R16_SINT,           DXGI_FORMAT_UNKNOWN              }, // R16I
        { DXGI_FORMAT_R16_UINT,           DXGI_FORMAT_UNKNOWN              }, // R16U
        { DXGI_FORMAT_R16_FLOAT,          DXGI_FORMAT_UNKNOWN              }, // R16F
        { DXGI_FORMAT_R16_SNORM,          DXGI_FORMAT_UNKNOWN              }, // R16S
        { DXGI_FORMAT_R32_SINT,           DXGI_FORMAT_UNKNOWN              }, // R32I
        { DXGI_FORMAT_R32_UINT,           DXGI_FORMAT_UNKNOWN              }, // R32U
        { DXGI_FORMAT_R32_FLOAT,          DXGI_FORMAT_UNKNOWN              }, // R32F
        { DXGI_FORMAT_R8G8_UNORM,         DXGI_FORMAT_UNKNOWN              }, // RG8
        { DXGI_FORMAT_R8G8_SINT,          DXGI_FORMAT_UNKNOWN              }, // RG8I
        { DXGI_FORMAT_R8G8_UINT,          DXGI_FORMAT_UNKNOWN              }, // RG8U
        { DXGI_FORMAT_R8G8_SNORM,         DXGI_FORMAT_UNKNOWN              }, // RG8S
        { DXGI_FORMAT_R16G16_UNORM,       DXGI_FORMAT_UNKNOWN              }, // RG16
        { DXGI_FORMAT_R16G16_SINT,        DXGI_FORMAT_UNKNOWN              }, // RG16I
        { DXGI_FORMAT_R16G16_UINT,        DXGI_FORMAT_UNKNOWN              }, // RG16U
        { DXGI_FORMAT_R16G16_FLOAT,       DXGI_FORMAT_UNKNOWN              }, // RG16F
        { DXGI_FORMAT_R16G16_SNORM,       DXGI_FORMAT_UNKNOWN              }, // RG16S
        { DXGI_FORMAT_R32G32_SINT,        DXGI_FORMAT_UNKNOWN              }, // RG32I
        { DXGI_FORMAT_R32G32_UINT,        DXGI_FORMAT_UNKNOWN              }, // RG32U
        { DXGI_FORMAT_R32G32_FLOAT,       DXGI_FORMAT_UNKNOWN              }, // RG32F
        { DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN              }, // RGB8
        { DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN              }, // RGB8I
        { DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN              }, // RGB8U
        { DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN              }, // RGB8S
        { DXGI_FORMAT_R9G9B9E5_SHAREDEXP, DXGI_FORMAT_UNKNOWN              }, // RGB9E5F
        { DXGI_FORMAT_B8G8R8A8_UNORM,     DXGI_FORMAT_B8G8R8A8_UNORM_SRGB  }, // BGRA8
        { DXGI_FORMAT_R8G8B8A8_UNORM,     DXGI_FORMAT_R8G8B8A8_UNORM_SRGB  }, // RGBA8
        { DXGI_FORMAT_R8G8B8A8_SINT,      DXGI_FORMAT_R8G8B8A8_UNORM_SRGB  }, // RGBA8I
        { DXGI_FORMAT_R8G8B8A8_UINT,      DXGI_FORMAT_R8G8B8A8_UNORM_SRGB  }, // RGBA8U
        { DXGI_FORMAT_R8G8B8A8_SNORM,     DXGI_FORMAT_UNKNOWN              }, // RGBA8S
        { DXGI_FORMAT_R16G16B16A16_UNORM, DXGI_FORMAT_UNKNOWN              }, // RGBA16
        { DXGI_FORMAT_R16G16B16A16_SINT,  DXGI_FORMAT_UNKNOWN              }, // RGBA16I
        { DXGI_FORMAT_R16G16B16A16_UINT,  DXGI_FORMAT_UNKNOWN              }, // RGBA16U
        { DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN              }, // RGBA16F
        { DXGI_FORMAT_R16G16B16A16_SNORM, DXGI_FORMAT_UNKNOWN              }, // RGBA16S
        { DXGI_FORMAT_R32G32B32A32_SINT,  DXGI_FORMAT_UNKNOWN              }, // RGBA32I
        { DXGI_FORMAT_R32G32B32A32_UINT,  DXGI_FORMAT_UNKNOWN              }, // RGBA32U
        { DXGI_FORMAT_R32G32B32A32_FLOAT, DXGI_FORMAT_UNKNOWN              }, // RGBA32F
        { DXGI_FORMAT_B5G6R5_UNORM,       DXGI_FORMAT_UNKNOWN              }, // B5G6R5
        { DXGI_FORMAT_B5G6R5_UNORM,       DXGI_FORMAT_UNKNOWN              }, // R5G6B5
        { DXGI_FORMAT_B4G4R4A4_UNORM,     DXGI_FORMAT_UNKNOWN              }, // BGRA4
        { DXGI_FORMAT_B4G4R4A4_UNORM,     DXGI_FORMAT_UNKNOWN              }, // RGBA4
        { DXGI_FORMAT_B5G5R5A1_UNORM,     DXGI_FORMAT_UNKNOWN              }, // BGR5A1
        { DXGI_FORMAT_B5G5R5A1_UNORM,     DXGI_FORMAT_UNKNOWN              }, // RGB5A1
        { DXGI_FORMAT_R10G10B10A2_UNORM,  DXGI_FORMAT_UNKNOWN              }, // RGB10A2
        { DXGI_FORMAT_R10G10B10A2_UINT,   DXGI_FORMAT_UNKNOWN              }, // RGB10A2U
        { DXGI_FORMAT_R11G11B10_FLOAT,    DXGI_FORMAT_UNKNOWN              }, // RG11B10F
        { DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN              }, // UnknownDepth
        { DXGI_FORMAT_R16_TYPELESS,       DXGI_FORMAT_UNKNOWN              }, // D16
        { DXGI_FORMAT_R24G8_TYPELESS,     DXGI_FORMAT_UNKNOWN              }, // D24
        { DXGI_FORMAT_R24G8_TYPELESS,     DXGI_FORMAT_UNKNOWN              }, // D24S8
        { DXGI_FORMAT_R24G8_TYPELESS,     DXGI_FORMAT_UNKNOWN              }, // D32
        { DXGI_FORMAT_R32_TYPELESS,       DXGI_FORMAT_UNKNOWN              }, // D16F
        { DXGI_FORMAT_R32_TYPELESS,       DXGI_FORMAT_UNKNOWN              }, // D24F
        { DXGI_FORMAT_R32_TYPELESS,       DXGI_FORMAT_UNKNOWN              }, // D32F
        { DXGI_FORMAT_R32G8X24_TYPELESS,  DXGI_FORMAT_UNKNOWN              }, // D32FS8
        { DXGI_FORMAT_R24G8_TYPELESS,     DXGI_FORMAT_UNKNOWN              }, // D0S8
    };
    static_assert(bgfx::TextureFormat::Count == BX_COUNTOF(s_textureFormat));

    struct BgfxTextureFormat
    {
        bgfx::TextureFormat::Enum Format;
        bool Srgb;
    };

    inline std::optional<BgfxTextureFormat> TryGetBgfxTextureFormat(DXGI_FORMAT format)
    {
        if (format == DXGI_FORMAT_UNKNOWN)
        {
            return std::nullopt;
        }

        for (int i = 0; i < BX_COUNTOF(s_textureFormat); ++i)
        {
            if (s_textureFormat[i].m_fmt == format)
            {
                return BgfxTextureFormat{static_cast<bgfx::TextureFormat::Enum>(i), false};
            }
            if (s_textureFormat[i].m_fmtSrgb == format)
            {
                return BgfxTextureFormat{static_cast<bgfx::TextureFormat::Enum>(i), true};
            }
        }

        return std::nullopt;
    }

    inline std::optional<bgfx::TextureFormat::Enum> TryGetBgfxDepthFormat(DXGI_FORMAT format)
    {
        switch (format)
        {
            case DXGI_FORMAT_D16_UNORM:
                return bgfx::TextureFormat::D16;
            case DXGI_FORMAT_D24_UNORM_S8_UINT:
                return bgfx::TextureFormat::D24S8;
            case DXGI_FORMAT_D32_FLOAT:
                return bgfx::TextureFormat::D32F;
            case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
                return bgfx::TextureFormat::D32FS8;
            default:
                return std::nullopt;
        }
    }
}

// clang-format on
