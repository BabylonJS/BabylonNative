#include <Babylon/Graphics/RendererType.h>
#include "DeviceImpl.h"

#include <Babylon/Graphics/DeviceQueries.h>
#include <gsl/util>
#include <winrt/base.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <stdexcept>

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

    std::optional<BgfxTextureFormat> TryGetBgfxTextureFormat(DXGI_FORMAT format)
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

    std::optional<bgfx::TextureFormat::Enum> TryGetBgfxDepthFormat(DXGI_FORMAT format)
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

namespace
{
    void ThrowIfFailed(HRESULT result, const char* message)
    {
        if (FAILED(result))
        {
            throw std::runtime_error{message};
        }
    }

    uint64_t GetMsaaFlags(uint32_t sampleCount)
    {
        switch (sampleCount)
        {
            case 1:
                return BGFX_TEXTURE_NONE;
            case 2:
                return BGFX_TEXTURE_RT_MSAA_X2;
            case 4:
                return BGFX_TEXTURE_RT_MSAA_X4;
            case 8:
                return BGFX_TEXTURE_RT_MSAA_X8;
            case 16:
                return BGFX_TEXTURE_RT_MSAA_X16;
            default:
                throw std::runtime_error{"Unsupported D3D11 external back buffer sample count."};
        }
    }

    struct ViewInfo
    {
        winrt::com_ptr<ID3D11Texture2D> Texture;
        D3D11_TEXTURE2D_DESC TextureDesc{};
        DXGI_FORMAT ViewFormat{DXGI_FORMAT_UNKNOWN};
        uint32_t Mip{};
        uint32_t FirstLayer{};
        uint32_t NumLayers{1};
        uint32_t Width{};
        uint32_t Height{};
        uint8_t AttachmentFlags{BGFX_ATTACHMENT_NONE};
        bgfx::TextureFormat::Enum BgfxFormat{bgfx::TextureFormat::Unknown};
        bool Srgb{};
    };

    ViewInfo GetTextureInfo(ID3D11View* view)
    {
        winrt::com_ptr<ID3D11Resource> resource;
        view->GetResource(resource.put());

        D3D11_RESOURCE_DIMENSION dimension{};
        resource->GetType(&dimension);
        if (dimension != D3D11_RESOURCE_DIMENSION_TEXTURE2D)
        {
            throw std::runtime_error{"D3D11 external back buffers must reference a Texture2D resource."};
        }

        ViewInfo info{};
        info.Texture = resource.as<ID3D11Texture2D>();
        info.Texture->GetDesc(&info.TextureDesc);
        return info;
    }

    void SetViewDimensions(ViewInfo& info)
    {
        info.Width = std::max(1u, info.TextureDesc.Width >> info.Mip);
        info.Height = std::max(1u, info.TextureDesc.Height >> info.Mip);
        if (info.FirstLayer + info.NumLayers > info.TextureDesc.ArraySize)
        {
            throw std::runtime_error{"D3D11 external back buffer view exceeds its texture array."};
        }
    }

    ViewInfo GetColorInfo(ID3D11RenderTargetView* view)
    {
        ViewInfo info = GetTextureInfo(view);

        D3D11_RENDER_TARGET_VIEW_DESC desc{};
        view->GetDesc(&desc);
        info.ViewFormat = desc.Format;
        switch (desc.ViewDimension)
        {
            case D3D11_RTV_DIMENSION_TEXTURE2D:
                info.Mip = desc.Texture2D.MipSlice;
                break;
            case D3D11_RTV_DIMENSION_TEXTURE2DARRAY:
                info.Mip = desc.Texture2DArray.MipSlice;
                info.FirstLayer = desc.Texture2DArray.FirstArraySlice;
                info.NumLayers = desc.Texture2DArray.ArraySize;
                break;
            case D3D11_RTV_DIMENSION_TEXTURE2DMS:
                break;
            case D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY:
                info.FirstLayer = desc.Texture2DMSArray.FirstArraySlice;
                info.NumLayers = desc.Texture2DMSArray.ArraySize;
                break;
            default:
                throw std::runtime_error{"Unsupported D3D11 external color back buffer view dimension."};
        }

        const auto format = Babylon::Graphics::D3D11TextureFormats::TryGetBgfxTextureFormat(info.ViewFormat);
        if (!format || format->Format == bgfx::TextureFormat::Unknown)
        {
            throw std::runtime_error{"Unsupported D3D11 external color back buffer format."};
        }
        info.BgfxFormat = format->Format;
        info.Srgb = format->Srgb;
        SetViewDimensions(info);
        return info;
    }

    ViewInfo GetDepthInfo(ID3D11DepthStencilView* view)
    {
        ViewInfo info = GetTextureInfo(view);

        D3D11_DEPTH_STENCIL_VIEW_DESC desc{};
        view->GetDesc(&desc);
        info.ViewFormat = desc.Format;
        switch (desc.ViewDimension)
        {
            case D3D11_DSV_DIMENSION_TEXTURE2D:
                info.Mip = desc.Texture2D.MipSlice;
                break;
            case D3D11_DSV_DIMENSION_TEXTURE2DARRAY:
                info.Mip = desc.Texture2DArray.MipSlice;
                info.FirstLayer = desc.Texture2DArray.FirstArraySlice;
                info.NumLayers = desc.Texture2DArray.ArraySize;
                break;
            case D3D11_DSV_DIMENSION_TEXTURE2DMS:
                break;
            case D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY:
                info.FirstLayer = desc.Texture2DMSArray.FirstArraySlice;
                info.NumLayers = desc.Texture2DMSArray.ArraySize;
                break;
            default:
                throw std::runtime_error{"Unsupported D3D11 external depth back buffer view dimension."};
        }

        const auto format = Babylon::Graphics::D3D11TextureFormats::TryGetBgfxDepthFormat(info.ViewFormat);
        if (!format)
        {
            throw std::runtime_error{"Unsupported D3D11 external depth back buffer format."};
        }
        info.BgfxFormat = *format;
        info.AttachmentFlags = static_cast<uint8_t>(
            ((desc.Flags & D3D11_DSV_READ_ONLY_DEPTH) ? BGFX_ATTACHMENT_READ_ONLY_DEPTH : 0) |
            ((desc.Flags & D3D11_DSV_READ_ONLY_STENCIL) ? BGFX_ATTACHMENT_READ_ONLY_STENCIL : 0));
        SetViewDimensions(info);
        return info;
    }

    void ValidateDevice(const ViewInfo& info, ID3D11Device* bgfxDevice)
    {
        winrt::com_ptr<ID3D11Device> resourceDevice;
        info.Texture->GetDevice(resourceDevice.put());
        const auto resourceIdentity = resourceDevice.as<IUnknown>();

        winrt::com_ptr<ID3D11Device> retainedBgfxDevice;
        retainedBgfxDevice.copy_from(bgfxDevice);
        const auto bgfxIdentity = retainedBgfxDevice.as<IUnknown>();
        if (resourceIdentity.get() != bgfxIdentity.get())
        {
            throw std::runtime_error{"D3D11 external back buffer belongs to a different device."};
        }
    }

    bgfx::TextureHandle ImportTexture(const ViewInfo& info)
    {
        uint64_t flags = BGFX_TEXTURE_RT_WRITE_ONLY | GetMsaaFlags(info.TextureDesc.SampleDesc.Count);
        if (info.Srgb)
        {
            flags |= BGFX_TEXTURE_SRGB;
        }

        const auto handle = bgfx::createTexture2D(
            static_cast<uint16_t>(info.TextureDesc.Width),
            static_cast<uint16_t>(info.TextureDesc.Height),
            info.TextureDesc.MipLevels > 1,
            static_cast<uint16_t>(info.TextureDesc.ArraySize),
            info.BgfxFormat,
            flags,
            nullptr,
            static_cast<uint64_t>(reinterpret_cast<uintptr_t>(info.Texture.get())));
        if (!bgfx::isValid(handle))
        {
            throw std::runtime_error{"Failed to import D3D11 external back buffer texture."};
        }
        return handle;
    }

    bgfx::Attachment MakeAttachment(bgfx::TextureHandle handle, const ViewInfo& info)
    {
        bgfx::Attachment attachment{};
        attachment.init(
            handle,
            bgfx::Access::Write,
            static_cast<uint16_t>(info.FirstLayer),
            static_cast<uint16_t>(info.NumLayers),
            static_cast<uint16_t>(info.Mip),
            info.AttachmentFlags);
        return attachment;
    }

    bgfx::TextureFormat::Enum GetCaptureFormat(DXGI_FORMAT format)
    {
        switch (format)
        {
            case DXGI_FORMAT_B8G8R8A8_UNORM:
            case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
                return bgfx::TextureFormat::BGRA8;
            case DXGI_FORMAT_R8G8B8A8_UNORM:
            case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
                return bgfx::TextureFormat::RGBA8;
            default:
                throw std::runtime_error{"Unsupported D3D11 external back buffer capture format."};
        }
    }

    struct DepthFormats
    {
        DXGI_FORMAT Resource;
        DXGI_FORMAT View;
    };

    DepthFormats GetDepthFormats(bgfx::TextureFormat::Enum format)
    {
        switch (format)
        {
            case bgfx::TextureFormat::D16:
                return {DXGI_FORMAT_R16_TYPELESS, DXGI_FORMAT_D16_UNORM};
            case bgfx::TextureFormat::D24S8:
                return {DXGI_FORMAT_R24G8_TYPELESS, DXGI_FORMAT_D24_UNORM_S8_UINT};
            case bgfx::TextureFormat::D32F:
                return {DXGI_FORMAT_R32_TYPELESS, DXGI_FORMAT_D32_FLOAT};
            case bgfx::TextureFormat::D32FS8:
                return {DXGI_FORMAT_R32G8X24_TYPELESS, DXGI_FORMAT_D32_FLOAT_S8X24_UINT};
            default:
                throw std::runtime_error{"Unsupported generated D3D11 back buffer depth format."};
        }
    }

    ViewInfo CreateDepthInfo(
        ID3D11Device* device,
        const ViewInfo& colorInfo,
        bgfx::TextureFormat::Enum format,
        winrt::com_ptr<ID3D11Texture2D>& texture,
        winrt::com_ptr<ID3D11DepthStencilView>& view)
    {
        const DepthFormats formats = GetDepthFormats(format);

        D3D11_TEXTURE2D_DESC textureDesc{};
        textureDesc.Width = colorInfo.Width;
        textureDesc.Height = colorInfo.Height;
        textureDesc.MipLevels = 1;
        textureDesc.ArraySize = colorInfo.NumLayers;
        textureDesc.Format = formats.Resource;
        textureDesc.SampleDesc = colorInfo.TextureDesc.SampleDesc;
        textureDesc.Usage = D3D11_USAGE_DEFAULT;
        textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        ThrowIfFailed(
            device->CreateTexture2D(&textureDesc, nullptr, texture.put()),
            "Failed to create native depth texture for D3D11 external back buffer.");

        D3D11_DEPTH_STENCIL_VIEW_DESC viewDesc{};
        viewDesc.Format = formats.View;
        if (textureDesc.SampleDesc.Count > 1)
        {
            if (textureDesc.ArraySize > 1)
            {
                viewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY;
                viewDesc.Texture2DMSArray.ArraySize = textureDesc.ArraySize;
            }
            else
            {
                viewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
            }
        }
        else if (textureDesc.ArraySize > 1)
        {
            viewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
            viewDesc.Texture2DArray.ArraySize = textureDesc.ArraySize;
        }
        else
        {
            viewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        }

        ThrowIfFailed(
            device->CreateDepthStencilView(texture.get(), &viewDesc, view.put()),
            "Failed to create native depth view for D3D11 external back buffer.");
        return GetDepthInfo(view.get());
    }
}

namespace Babylon::Graphics
{
    const bgfx::RendererType::Enum DeviceImpl::s_bgfxRenderType = bgfx::RendererType::Direct3D11;

    PlatformInfo DeviceImpl::GetPlatformInfo() const
    {
        return {static_cast<DeviceT>(bgfx::getInternalData()->context)};
    }

    void DeviceImpl::ResizeRenderSurface(WindowT /*window*/, uint32_t /*width*/, uint32_t /*height*/)
    {
        // No-op: the swap chain size is managed elsewhere on this platform.
    }

    void DeviceImpl::CreateExternalBackBuffer(const bgfx::SwapChain& descriptor)
    {
        auto* color = m_state.BackBufferColor.get();
        auto* depth = m_state.BackBufferDepthStencil.get();
        const auto* internalData = bgfx::getInternalData();
        auto* bgfxDevice = internalData == nullptr ? nullptr : static_cast<ID3D11Device*>(internalData->context);
        if (bgfxDevice == nullptr)
        {
            throw std::runtime_error{"D3D11 device is unavailable while importing an external back buffer."};
        }

        bool cleanupNeeded = true;
        const auto cleanup = gsl::finally([this, &cleanupNeeded] {
            if (cleanupNeeded)
            {
                DestroyExternalBackBuffer();
            }
        });

        std::optional<ViewInfo> colorInfo;
        if (color != nullptr)
        {
            m_externalBackBuffer.Color.copy_from(color);
            colorInfo = GetColorInfo(color);
            ValidateDevice(*colorInfo, bgfxDevice);
            m_externalBackBuffer.ColorTexture = colorInfo->Texture;
            m_externalBackBuffer.ColorHandle = ImportTexture(*colorInfo);
        }

        std::optional<ViewInfo> depthInfo;
        if (depth != nullptr)
        {
            m_externalBackBuffer.Depth.copy_from(depth);
            depthInfo = GetDepthInfo(depth);
            ValidateDevice(*depthInfo, bgfxDevice);
            if (color == nullptr && descriptor.nwh != nullptr)
            {
                D3D11_DEPTH_STENCIL_VIEW_DESC viewDesc{};
                depth->GetDesc(&viewDesc);
                // SwapChain::depth cannot carry a mip, array range or read-only flags.
                if ((viewDesc.ViewDimension != D3D11_DSV_DIMENSION_TEXTURE2D &&
                        viewDesc.ViewDimension != D3D11_DSV_DIMENSION_TEXTURE2DMS) ||
                    depthInfo->Mip != 0 || depthInfo->TextureDesc.ArraySize != 1 || viewDesc.Flags != 0)
                {
                    throw std::runtime_error{
                        "A D3D11 window back buffer requires a writable, non-array, mip-0 depth view when no color view is supplied."};
                }
            }
            m_externalBackBuffer.DepthTexture = depthInfo->Texture;
            m_externalBackBuffer.DepthHandle = ImportTexture(*depthInfo);
        }

        if (colorInfo && depthInfo &&
            (colorInfo->Width != depthInfo->Width ||
                colorInfo->Height != depthInfo->Height ||
                colorInfo->NumLayers != depthInfo->NumLayers ||
                colorInfo->TextureDesc.SampleDesc.Count != depthInfo->TextureDesc.SampleDesc.Count ||
                colorInfo->TextureDesc.SampleDesc.Quality != depthInfo->TextureDesc.SampleDesc.Quality))
        {
            throw std::runtime_error{"D3D11 external color and depth back buffer views do not match."};
        }

        if (colorInfo && !depthInfo && descriptor.formatDepthStencil != bgfx::TextureFormat::Count)
        {
            depthInfo = CreateDepthInfo(
                bgfxDevice,
                *colorInfo,
                descriptor.formatDepthStencil,
                m_externalBackBuffer.DepthTexture,
                m_externalBackBuffer.Depth);
            m_externalBackBuffer.DepthHandle = ImportTexture(*depthInfo);
        }

        std::array<bgfx::Attachment, 2> attachments{};
        uint8_t attachmentCount{};
        if (colorInfo)
        {
            attachments[attachmentCount++] = MakeAttachment(m_externalBackBuffer.ColorHandle, *colorInfo);
        }
        if (depthInfo && (colorInfo || descriptor.nwh == nullptr))
        {
            attachments[attachmentCount++] = MakeAttachment(m_externalBackBuffer.DepthHandle, *depthInfo);
        }

        if (attachmentCount != 0)
        {
            m_externalBackBuffer.FrameBuffer = bgfx::createFrameBuffer(attachmentCount, attachments.data(), false);
            if (!bgfx::isValid(m_externalBackBuffer.FrameBuffer))
            {
                throw std::runtime_error{"Failed to create D3D11 external back buffer framebuffer."};
            }
        }

        cleanupNeeded = false;
    }

    void DeviceImpl::ReadExternalBackBuffer()
    {
        if (!m_externalBackBuffer.Color || !m_externalBackBuffer.ColorTexture)
        {
            throw std::runtime_error{"Cannot capture a D3D11 external back buffer without a color view."};
        }

        const auto colorInfo = GetColorInfo(m_externalBackBuffer.Color.get());
        const bgfx::TextureFormat::Enum captureFormat = GetCaptureFormat(colorInfo.ViewFormat);

        winrt::com_ptr<ID3D11Device> device;
        m_externalBackBuffer.ColorTexture->GetDevice(device.put());
        winrt::com_ptr<ID3D11DeviceContext> context;
        device->GetImmediateContext(context.put());

        D3D11_TEXTURE2D_DESC copyDesc{};
        copyDesc.Width = colorInfo.Width;
        copyDesc.Height = colorInfo.Height;
        copyDesc.MipLevels = 1;
        copyDesc.ArraySize = 1;
        copyDesc.Format = colorInfo.ViewFormat;
        copyDesc.SampleDesc.Count = 1;
        copyDesc.Usage = D3D11_USAGE_STAGING;
        copyDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

        winrt::com_ptr<ID3D11Texture2D> staging;
        ThrowIfFailed(
            device->CreateTexture2D(&copyDesc, nullptr, staging.put()),
            "Failed to create staging texture for D3D11 external back buffer capture.");

        const uint32_t sourceSubresource = D3D11CalcSubresource(
            colorInfo.Mip,
            colorInfo.FirstLayer,
            colorInfo.TextureDesc.MipLevels);

        if (colorInfo.TextureDesc.SampleDesc.Count > 1)
        {
            D3D11_TEXTURE2D_DESC resolveDesc = copyDesc;
            resolveDesc.Usage = D3D11_USAGE_DEFAULT;
            resolveDesc.CPUAccessFlags = 0;

            winrt::com_ptr<ID3D11Texture2D> resolved;
            ThrowIfFailed(
                device->CreateTexture2D(&resolveDesc, nullptr, resolved.put()),
                "Failed to create resolve texture for D3D11 external back buffer capture.");
            context->ResolveSubresource(
                resolved.get(),
                0,
                m_externalBackBuffer.ColorTexture.get(),
                sourceSubresource,
                colorInfo.ViewFormat);
            context->CopyResource(staging.get(), resolved.get());
        }
        else
        {
            context->CopySubresourceRegion(
                staging.get(),
                0,
                0,
                0,
                0,
                m_externalBackBuffer.ColorTexture.get(),
                sourceSubresource,
                nullptr);
        }

        D3D11_MAPPED_SUBRESOURCE mapped{};
        ThrowIfFailed(
            context->Map(staging.get(), 0, D3D11_MAP_READ, 0, &mapped),
            "Failed to map D3D11 external back buffer capture.");
        const auto unmap = gsl::finally([&] { context->Unmap(staging.get(), 0); });

        m_bgfxCallback.CompleteScreenShot(BgfxCallback::CaptureData{
            colorInfo.Width,
            colorInfo.Height,
            mapped.RowPitch,
            captureFormat,
            false,
            mapped.pData,
            mapped.RowPitch * colorInfo.Height});
    }

    void DeviceImpl::DestroyExternalBackBuffer()
    {
        if (bgfx::isValid(m_externalBackBuffer.FrameBuffer))
        {
            bgfx::destroy(m_externalBackBuffer.FrameBuffer);
            m_externalBackBuffer.FrameBuffer = BGFX_INVALID_HANDLE;
        }
        if (bgfx::isValid(m_externalBackBuffer.ColorHandle))
        {
            bgfx::destroy(m_externalBackBuffer.ColorHandle);
            m_externalBackBuffer.ColorHandle = BGFX_INVALID_HANDLE;
        }
        if (bgfx::isValid(m_externalBackBuffer.DepthHandle))
        {
            bgfx::destroy(m_externalBackBuffer.DepthHandle);
            m_externalBackBuffer.DepthHandle = BGFX_INVALID_HANDLE;
        }
        m_externalBackBuffer = {};
    }
}
