#include <Babylon/Graphics/Device.h>
#include <Babylon/Plugins/ExternalTexture/ExternalTexture.h>
#include <Babylon/Graphics/RendererType.h>
#include <Babylon/Graphics/Texture.h>
#include <Babylon/Graphics/DeviceContext.h>

#include <napi/env.h>
#include <napi/napi_pointer.h>

namespace Babylon::Plugins
{
    //For refrence see renderer_d3d11.cpp line 203.
    bgfx::TextureFormat::Enum NativeToBGFXImageFormat(DXGI_FORMAT format, uint64_t& flags)
    {
        switch (format)
        {
            case DXGI_FORMAT_BC1_UNORM:
                return bgfx::TextureFormat::Enum::BC1;

            case DXGI_FORMAT_BC1_UNORM_SRGB:
                flags |= BGFX_TEXTURE_SRGB;
                return bgfx::TextureFormat::Enum::BC1;

            case DXGI_FORMAT_BC2_UNORM:
                return bgfx::TextureFormat::Enum::BC2;

            case DXGI_FORMAT_BC2_UNORM_SRGB:
                flags |= BGFX_TEXTURE_SRGB;
                return bgfx::TextureFormat::Enum::BC2;

            case DXGI_FORMAT_BC3_UNORM:
                return bgfx::TextureFormat::Enum::BC3;

            case DXGI_FORMAT_BC3_UNORM_SRGB:
                flags |= BGFX_TEXTURE_SRGB;
                return bgfx::TextureFormat::Enum::BC3;

            case DXGI_FORMAT_BC4_UNORM:
                return bgfx::TextureFormat::Enum::BC4;

            case DXGI_FORMAT_BC5_UNORM:
                return bgfx::TextureFormat::Enum::BC5;

            case DXGI_FORMAT_BC6H_SF16:
                return bgfx::TextureFormat::Enum::BC6H; 
            
            case DXGI_FORMAT_BC7_UNORM:
                flags |= BGFX_TEXTURE_SRGB;
                return bgfx::TextureFormat::Enum::BC7;

            case DXGI_FORMAT_BC7_UNORM_SRGB:
                return bgfx::TextureFormat::Enum::BC7;

            case DXGI_FORMAT_R1_UNORM:
                return bgfx::TextureFormat::Enum::R1;

            case DXGI_FORMAT_A8_UNORM:
                return bgfx::TextureFormat::Enum::A8;

            case DXGI_FORMAT_R8_UNORM:
                return  bgfx::TextureFormat::Enum::R8;

            case DXGI_FORMAT_R8_SINT:
                return  bgfx::TextureFormat::Enum::R8I;

            case DXGI_FORMAT_R8_UINT:
                return  bgfx::TextureFormat::Enum::R8U;

            case DXGI_FORMAT_R8_SNORM:
                return  bgfx::TextureFormat::Enum::R8S;

            case DXGI_FORMAT_R16_UNORM:
                return  bgfx::TextureFormat::Enum::R16;

            case DXGI_FORMAT_R16_SINT:
                return  bgfx::TextureFormat::Enum::R16I;

            case DXGI_FORMAT_R16_UINT:
                return  bgfx::TextureFormat::Enum::R16U;

            case DXGI_FORMAT_R16_FLOAT:
                return  bgfx::TextureFormat::Enum::R16F;

            case DXGI_FORMAT_R16_SNORM:
                return  bgfx::TextureFormat::Enum::R16S;

            case DXGI_FORMAT_R32_SINT:
                return bgfx::TextureFormat::Enum::R32I;

            case DXGI_FORMAT_R32_UINT:
                return bgfx::TextureFormat::Enum::R32U;

            case DXGI_FORMAT_R32_FLOAT:
                return bgfx::TextureFormat::Enum::R32F;

            case DXGI_FORMAT_R8G8_UNORM:
                return bgfx::TextureFormat::Enum::RG8;

            case DXGI_FORMAT_R8G8_SINT:
                return bgfx::TextureFormat::Enum::RG8I;

            case DXGI_FORMAT_R8G8_UINT:
                return bgfx::TextureFormat::Enum::RG8U;

            case DXGI_FORMAT_R8G8_SNORM:
                return bgfx::TextureFormat::Enum::RG8S;

            case DXGI_FORMAT_R16G16_UNORM:
                return bgfx::TextureFormat::Enum::RG16;

            case DXGI_FORMAT_R16G16_SINT:
                return bgfx::TextureFormat::Enum::RG16I;

            case DXGI_FORMAT_R16G16_UINT:
                return bgfx::TextureFormat::Enum::RG16U;

            case DXGI_FORMAT_R16G16_FLOAT:
                return bgfx::TextureFormat::Enum::RG16F;

            case DXGI_FORMAT_R16G16_SNORM:
                return bgfx::TextureFormat::Enum::RG16S;

            case DXGI_FORMAT_R32G32_SINT:
                return bgfx::TextureFormat::Enum::RG32I;

            case DXGI_FORMAT_R32G32_UINT:
                return bgfx::TextureFormat::Enum::RG32U;

            case DXGI_FORMAT_R32G32_FLOAT:
                return bgfx::TextureFormat::Enum::RG32F;

            case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
                return bgfx::TextureFormat::Enum::RGB9E5F;

            case DXGI_FORMAT_R8G8B8A8_UNORM:
                return bgfx::TextureFormat::Enum::RGBA8;

            case DXGI_FORMAT_R8G8B8A8_SINT:
                return bgfx::TextureFormat::Enum::RGBA8I;

            case DXGI_FORMAT_R8G8B8A8_UINT:
                return bgfx::TextureFormat::Enum::RGBA8U;

            case DXGI_FORMAT_R8G8B8A8_SNORM:
                return bgfx::TextureFormat::Enum::RGBA8S;

            case DXGI_FORMAT_R16G16B16A16_UNORM:
                return bgfx::TextureFormat::Enum::RGBA16;

            case DXGI_FORMAT_R16G16B16A16_SINT:
                return bgfx::TextureFormat::Enum::RGBA16I;

            case DXGI_FORMAT_R16G16B16A16_UINT:
                return bgfx::TextureFormat::Enum::RGBA16U;

            case DXGI_FORMAT_R16G16B16A16_FLOAT:
                return bgfx::TextureFormat::Enum::RGBA16F;

            case DXGI_FORMAT_R16G16B16A16_SNORM:
                return bgfx::TextureFormat::Enum::RGBA16F;
            
            case DXGI_FORMAT_R32G32B32A32_SINT:
                return bgfx::TextureFormat::Enum::RGBA32I;

            case DXGI_FORMAT_R32G32B32A32_UINT:
                return bgfx::TextureFormat::Enum::RGBA32U;

            case DXGI_FORMAT_R32G32B32A32_FLOAT:
                return bgfx::TextureFormat::Enum::RGBA32F;

            case DXGI_FORMAT_B5G6R5_UNORM:
                return bgfx::TextureFormat::Enum::R5G6B5;

            case DXGI_FORMAT_B4G4R4A4_UNORM:
                return bgfx::TextureFormat::Enum::RGBA4;

            case DXGI_FORMAT_B5G5R5A1_UNORM:
                return bgfx::TextureFormat::Enum::RGB5A1;

            case DXGI_FORMAT_R10G10B10A2_UNORM:
                return bgfx::TextureFormat::Enum::RGB10A2;

            case DXGI_FORMAT_R11G11B10_FLOAT:
                return bgfx::TextureFormat::Enum::RG11B10F;

            case DXGI_FORMAT_UNKNOWN:
                return bgfx::TextureFormat::Enum::Unknown;

            default:
                return bgfx::TextureFormat::Enum::Unknown;
        }
    }

    void ExternalTexture::ReadPropertiesFromNativeTexture(Babylon::Graphics::TextureType nativeTexture) 
    {
        D3D11_TEXTURE2D_DESC desc;
        nativeTexture->GetDesc(&desc);
        uint64_t flags = 0Ui64;
        m_format = NativeToBGFXImageFormat(desc.Format, flags);
        m_height = desc.Height;
        m_width = desc.Width;
        m_mips = desc.MipLevels;
        m_flags = flags;
    }
}
