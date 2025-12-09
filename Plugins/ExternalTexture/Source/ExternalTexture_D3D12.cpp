#include <Babylon/Plugins/ExternalTexture.h>
#include <Babylon/Graphics/DeviceContext.h>
#include <Babylon/Graphics/Texture.h>
#include <napi/pointer.h>
#include <bx/bx.h>
#include <winrt/base.h>

#include "ExternalTexture_Base.h"

// clang-format off

// Copied from renderer_d3d.h
#define DXGI_FORMAT_ASTC_4X4_UNORM        DXGI_FORMAT(134)
#define DXGI_FORMAT_ASTC_4X4_UNORM_SRGB   DXGI_FORMAT(135)
#define DXGI_FORMAT_ASTC_5X4_UNORM        DXGI_FORMAT(138)
#define DXGI_FORMAT_ASTC_5X4_UNORM_SRGB   DXGI_FORMAT(139)
#define DXGI_FORMAT_ASTC_5X5_UNORM        DXGI_FORMAT(142)
#define DXGI_FORMAT_ASTC_5X5_UNORM_SRGB   DXGI_FORMAT(143)
#define DXGI_FORMAT_ASTC_6X5_UNORM        DXGI_FORMAT(146)
#define DXGI_FORMAT_ASTC_6X5_UNORM_SRGB   DXGI_FORMAT(147)
#define DXGI_FORMAT_ASTC_6X6_UNORM        DXGI_FORMAT(150)
#define DXGI_FORMAT_ASTC_6X6_UNORM_SRGB   DXGI_FORMAT(151)
#define DXGI_FORMAT_ASTC_8X5_UNORM        DXGI_FORMAT(154)
#define DXGI_FORMAT_ASTC_8X5_UNORM_SRGB   DXGI_FORMAT(155)
#define DXGI_FORMAT_ASTC_8X6_UNORM        DXGI_FORMAT(158)
#define DXGI_FORMAT_ASTC_8X6_UNORM_SRGB   DXGI_FORMAT(159)
#define DXGI_FORMAT_ASTC_8X8_UNORM        DXGI_FORMAT(162)
#define DXGI_FORMAT_ASTC_8X8_UNORM_SRGB   DXGI_FORMAT(163)
#define DXGI_FORMAT_ASTC_10X5_UNORM       DXGI_FORMAT(166)
#define DXGI_FORMAT_ASTC_10X5_UNORM_SRGB  DXGI_FORMAT(167)
#define DXGI_FORMAT_ASTC_10X6_UNORM       DXGI_FORMAT(170)
#define DXGI_FORMAT_ASTC_10X6_UNORM_SRGB  DXGI_FORMAT(171)
#define DXGI_FORMAT_ASTC_10X8_UNORM       DXGI_FORMAT(174)
#define DXGI_FORMAT_ASTC_10X8_UNORM_SRGB  DXGI_FORMAT(175)
#define DXGI_FORMAT_ASTC_10X10_UNORM      DXGI_FORMAT(178)
#define DXGI_FORMAT_ASTC_10X10_UNORM_SRGB DXGI_FORMAT(179)
#define DXGI_FORMAT_ASTC_12X10_UNORM      DXGI_FORMAT(182)
#define DXGI_FORMAT_ASTC_12X10_UNORM_SRGB DXGI_FORMAT(183)
#define DXGI_FORMAT_ASTC_12X12_UNORM      DXGI_FORMAT(186)
#define DXGI_FORMAT_ASTC_12X12_UNORM_SRGB DXGI_FORMAT(187)

// Copied from renderer_d3d12.cpp
namespace
{
    struct TextureFormatInfo
    {
        DXGI_FORMAT m_fmt;
        DXGI_FORMAT m_fmtSrgb;
    };

    const TextureFormatInfo s_textureFormat[] =
    {
        { DXGI_FORMAT_BC1_UNORM,          DXGI_FORMAT_BC1_UNORM_SRGB       }, // BC1
        { DXGI_FORMAT_BC2_UNORM,          DXGI_FORMAT_BC2_UNORM_SRGB       }, // BC2
        { DXGI_FORMAT_BC3_UNORM,          DXGI_FORMAT_BC3_UNORM_SRGB       }, // BC3
        { DXGI_FORMAT_BC4_UNORM,          DXGI_FORMAT_UNKNOWN              }, // BC4
        { DXGI_FORMAT_BC5_UNORM,          DXGI_FORMAT_UNKNOWN              }, // BC5
        { DXGI_FORMAT_BC6H_SF16,          DXGI_FORMAT_UNKNOWN              }, // BC6H
        { DXGI_FORMAT_BC7_UNORM,          DXGI_FORMAT_BC7_UNORM_SRGB       }, // BC7
        { DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN              }, // ETC1
        { DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN              }, // ETC2
        { DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN              }, // ETC2A
        { DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN              }, // ETC2A1
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
        { DXGI_FORMAT_R11G11B10_FLOAT,    DXGI_FORMAT_UNKNOWN              }, // RG11B10F
        { DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN              }, // UnknownDepth
        { DXGI_FORMAT_R16_TYPELESS,       DXGI_FORMAT_UNKNOWN              }, // D16
        { DXGI_FORMAT_R24G8_TYPELESS,     DXGI_FORMAT_UNKNOWN              }, // D24
        { DXGI_FORMAT_R24G8_TYPELESS,     DXGI_FORMAT_UNKNOWN              }, // D24S8
        { DXGI_FORMAT_R24G8_TYPELESS,     DXGI_FORMAT_UNKNOWN              }, // D32
        { DXGI_FORMAT_R32_TYPELESS,       DXGI_FORMAT_UNKNOWN              }, // D16F
        { DXGI_FORMAT_R32_TYPELESS,       DXGI_FORMAT_UNKNOWN              }, // D24F
        { DXGI_FORMAT_R32_TYPELESS,       DXGI_FORMAT_UNKNOWN              }, // D32F
        { DXGI_FORMAT_R24G8_TYPELESS,     DXGI_FORMAT_UNKNOWN              }, // D0S8
    };
    static_assert(bgfx::TextureFormat::Count == BX_COUNTOF(s_textureFormat));
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
            D3D12_RESOURCE_DESC desc = ptr->GetDesc();

            if (desc.Dimension != D3D12_RESOURCE_DIMENSION_TEXTURE2D)
            {
                throw std::runtime_error{"Unsupported texture type"};
            }

            info.Width = static_cast<uint16_t>(desc.Width);
            info.Height = static_cast<uint16_t>(desc.Height);
            info.MipLevels = static_cast<uint16_t>(desc.MipLevels);
            info.NumLayers = static_cast<uint16_t>(desc.DepthOrArraySize);

            if ((desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET) != 0)
            {
                info.Flags |= BGFX_TEXTURE_RT;

                if (desc.SampleDesc.Count > 1)
                {
                    info.Flags |= BGFX_TEXTURE_MSAA_SAMPLE | RenderTargetSamplesToBgfxMsaaFlag(desc.SampleDesc.Count);
                }
            }

            DXGI_FORMAT targetFormat = overrideFormat.has_value() ? overrideFormat.value() : desc.Format;
            for (int i = 0; i < BX_COUNTOF(s_textureFormat); ++i)
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
            m_ptr.copy_from(ptr);
        }

        winrt::com_ptr<ID3D12Resource> m_ptr{};
    };
}

#include "ExternalTexture_Shared.h"
