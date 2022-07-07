#include <Babylon/Plugins/ExternalTexture.h>
#include <Babylon/Graphics/DeviceContext.h>
#include <Babylon/Graphics/Texture.h>
#include <napi/napi_pointer.h>
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
#define DXGI_FORMAT_ASTC_10X5_UNORM       DXGI_FORMAT(166)
#define DXGI_FORMAT_ASTC_10X5_UNORM_SRGB  DXGI_FORMAT(167)

// Copied from renderer_d3d11.cpp
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
        { DXGI_FORMAT_ASTC_5X5_UNORM,     DXGI_FORMAT_ASTC_5X5_UNORM_SRGB  }, // ASTC5x5
        { DXGI_FORMAT_ASTC_6X6_UNORM,     DXGI_FORMAT_ASTC_6X6_UNORM_SRGB  }, // ASTC6x6
        { DXGI_FORMAT_ASTC_8X5_UNORM,     DXGI_FORMAT_ASTC_8X5_UNORM_SRGB  }, // ASTC8x5
        { DXGI_FORMAT_ASTC_8X6_UNORM,     DXGI_FORMAT_ASTC_8X6_UNORM_SRGB  }, // ASTC8x6
        { DXGI_FORMAT_ASTC_10X5_UNORM,    DXGI_FORMAT_ASTC_10X5_UNORM_SRGB }, // ASTC10x5
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
        { DXGI_FORMAT_B5G6R5_UNORM,       DXGI_FORMAT_UNKNOWN              }, // R5G6B5
        { DXGI_FORMAT_B4G4R4A4_UNORM,     DXGI_FORMAT_UNKNOWN              }, // RGBA4
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
    BX_STATIC_ASSERT(bgfx::TextureFormat::Count == BX_COUNTOF(s_textureFormat));
}

// clang-format on

namespace Babylon::Plugins
{
    class ExternalTexture::Impl final : public ImplBase
    {
    public:
        Impl(Graphics::TextureT ptr);

        void Update(Graphics::TextureT ptr);

        uintptr_t Ptr() const
        {
            return reinterpret_cast<uintptr_t>(m_ptr.get());
        }

    private:
        void Init(Graphics::TextureT ptr)
        {
            m_ptr.copy_from(ptr);

            D3D11_RESOURCE_DIMENSION type;
            ptr->GetType(&type);
            if (type != D3D11_RESOURCE_DIMENSION_TEXTURE2D)
            {
                throw std::runtime_error{"Unsupported texture type"};
            }
        }

        void GetInfo(uint16_t& width, uint16_t& height, bool& hasMips, bgfx::TextureFormat::Enum& format, uint64_t& flags)
        {
            D3D11_TEXTURE2D_DESC desc;
            m_ptr.as<ID3D11Texture2D>()->GetDesc(&desc);

            width = static_cast<uint16_t>(desc.Width);
            height = static_cast<uint16_t>(desc.Height);

            if (desc.MipLevels == 1 || desc.MipLevels == 0 || IsFullMipChain(desc.MipLevels, desc.Width, desc.Height))
            {
                hasMips = (desc.MipLevels != 1);
            }
            else
            {
                throw std::runtime_error{"Unsupported texture mip levels"};
            }

            for (int i = 0; i < BX_COUNTOF(s_textureFormat); ++i)
            {
                const auto& info = s_textureFormat[i];
                if (info.m_fmt == desc.Format || info.m_fmtSrgb == desc.Format)
                {
                    format = static_cast<bgfx::TextureFormat::Enum>(i);
                    if (info.m_fmtSrgb == desc.Format)
                    {
                        flags |= BGFX_TEXTURE_SRGB;
                    }

                    break;
                }
            }
        }

        winrt::com_ptr<ID3D11Resource> m_ptr{};
    };
}

#include "ExternalTexture_Shared.h"
