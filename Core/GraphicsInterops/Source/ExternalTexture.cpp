#include <Babylon/Graphics/Device.h>
#include <Babylon/GraphicsInterops/ExternalTexture.h>
#include <Babylon/Graphics/Texture.h>
#include <Babylon/Graphics/DeviceContext.h>

#include <napi/env.h>
#include <napi/napi_pointer.h>

namespace Babylon::Graphics
{
    namespace
    {
        bgfx::TextureFormat::Enum GetBGFXFormatFromNativeTexture(TextureFormat)
        {
            return bgfx::TextureFormat::Enum::RGBA8;
        }

        bgfx::TextureFormat::Enum GetTextureFormat(ExternalTextureCreateInfo info)
        {
            D3D11_TEXTURE2D_DESC desc;
            info.Texture->GetDesc(&desc);
            return GetBGFXFormatFromNativeTexture(desc.Format);
        }

        TextureData CreateTextureDataFromNativeTexturePointer(ExternalTextureCreateInfo info)
        {
            D3D11_TEXTURE2D_DESC desc;
            info.Texture->GetDesc(&desc);
            uintptr_t texturePtr = reinterpret_cast<uintptr_t>(info.Texture);
            auto format = GetBGFXFormatFromNativeTexture(desc.Format);
            auto textureHandle = bgfx::createTexture2D(static_cast<uint16_t>(desc.Width), static_cast<uint16_t>(desc.Height), desc.MipLevels != 1, 1, format, BGFX_TEXTURE_RT);
            
            info.Device->StartRenderingCurrentFrame();
            info.Device->FinishRenderingCurrentFrame();

            if (bgfx::overrideInternal(textureHandle, texturePtr) == 0)
            {
                assert(false);
            }

            return TextureData{textureHandle, true, desc.Width, desc.Height};
        }
    }

    struct ExternalTexture::Impl
    {
        Impl(ExternalTextureCreateInfo info)
            : m_data{CreateTextureDataFromNativeTexturePointer(info)}
            , m_format{GetTextureFormat(info)}
        {
        }

        ~Impl()
        {
            bgfx::destroy(m_data.Handle);
        }

        Napi::Value AddToContext(Napi::Value& value)
        {
            DeviceContext& context = DeviceContext::GetFromJavaScript(value);
            auto* textureData = new TextureData(m_data);
            textureData->OwnsHandle = false;
            context.AddTexture(textureData->Handle, 0, 0, false, 0, m_format);
            return Napi::Pointer<TextureData>::Create(value.Env(), textureData, Napi::NapiPointerDeleter(textureData));
        }

        uint32_t GetWight() {
            return m_data.Width;
        }

        uint32_t GetHeight() {
            return m_data.Height;
        }

    private:
        TextureData m_data{};
        bgfx::TextureFormat::Enum m_format;
    };

    ExternalTexture::ExternalTexture(std::unique_ptr<Impl> impl)
        : m_impl{std::move(impl)}
    {
    }

    ExternalTexture::ExternalTexture(ExternalTexture&& other)
        : m_impl{std::move(other.m_impl)}
    {
    }

    ExternalTexture::~ExternalTexture()
    {
    }

    ExternalTexture::ExternalTexture(ExternalTextureCreateInfo info)
        : ExternalTexture{std::make_unique<ExternalTexture::Impl>(info)}
    {
    }

    Napi::Value ExternalTexture::AddToContext(Napi::Value& context)
    {
        return m_impl->AddToContext(context);
    }

    uint32_t ExternalTexture::GetWight() 
    {
        return m_impl->GetWight();
    }

    uint32_t ExternalTexture::GetHeight()
    {
        return m_impl->GetHeight();
    }
}