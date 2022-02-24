#include <Babylon/Graphics/Device.h>
#include <Babylon/GraphicsInterops/ExternalTexture.h>
#include <Babylon/Graphics/RendererType.h>
#include <Babylon/Graphics/Texture.h>
#include <Babylon/Graphics/DeviceContext.h>

#include <ExternalTextureImpl.h>

#include <napi/env.h>
#include <napi/napi_pointer.h>

namespace Babylon::Graphics
{
    bgfx::TextureFormat::Enum NativeToBGFXImageFormat(TextureFormat format)
    {
        switch (format)
        {
            case DXGI_FORMAT_R32G32B32A32_FLOAT:
                return bgfx::TextureFormat::Enum::RGBA32F;

            case DXGI_FORMAT_R32G32B32A32_UINT:
                return bgfx::TextureFormat::Enum::RGBA32U;

            case DXGI_FORMAT_R32G32B32A32_SINT:
                return bgfx::TextureFormat::Enum::RGBA32I;

            case DXGI_FORMAT_R8G8B8A8_UINT:
                return bgfx::TextureFormat::Enum::RGBA8;

            case DXGI_FORMAT_R8G8B8A8_SINT:
                return bgfx::TextureFormat::Enum::RGBA8;

            case DXGI_FORMAT_UNKNOWN:
                return bgfx::TextureFormat::Enum::RGBA8;

            default:
                return bgfx::TextureFormat::Enum::RGBA8;
        }
    }

    void ExternalTexture::Impl::GetNativeTextureData(TextureType nativeTexture) 
    {
        D3D11_TEXTURE2D_DESC desc;
        nativeTexture->GetDesc(&desc);
        m_format = NativeToBGFXImageFormat(desc.Format);
        m_height = desc.Height;
        m_width = desc.Width;
        m_mips = desc.MipLevels;
    }
}
