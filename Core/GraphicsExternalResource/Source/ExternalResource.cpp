#include <ExternalResource.h>

#include <GraphicsContext.h>
#include <Texture.h>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#include <napi/napi_pointer.h>

#include <cassert>

namespace Babylon
{
    namespace
    {
        TextureData CreateTextureDataFromNativeTexturePointer(Graphics&, TexturePointerType)
        {
            // TODO: Implement
            // 
            // uintptr_t texturePtr = 0;
            // auto textureHandle = bgfx::createTexture2D(1, 1, false, 1, bgfx::TextureFormat::BGRA8, BGFX_TEXTURE_RT);
            // if (bgfx::overrideInternal(textureHandle, texturePtr) == 0)
            // {
            //     assert(false);
            // }
            
            throw std::runtime_error{"Not yet implemented"};
        }
    }

    struct ExternalTexture::Impl
    {
        Impl(Graphics& graphics, TexturePointerType texturePtr)
            : m_data{CreateTextureDataFromNativeTexturePointer(graphics, texturePtr)}
        {
        }

        ~Impl()
        {
            bgfx::destroy(m_data.Handle);
        }

        Napi::Value AddToContext(Napi::Value& value)
        {
            auto& context = GraphicsContext::GetFromJavaScript(value);
            auto* textureData = new TextureData(m_data);
            textureData->OwnsHandle = false;
            context.AddTexture(textureData->Handle, 0, 0, false, 0, bgfx::TextureFormat::BGRA8);
            return Napi::Pointer<TextureData>::Create(value.Env(), textureData, Napi::NapiPointerDeleter(textureData));
        }
    
    private:
        TextureData m_data{};
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

    ExternalTexture::ExternalTexture(Graphics& graphics, TexturePointerType texturePtr)
        : ExternalTexture{std::make_unique<ExternalTexture::Impl>(graphics, texturePtr)}
    {
    }

    Napi::Value ExternalTexture::AddToContext(Napi::Value& context)
    {
        return m_impl->AddToContext(context);
    }
}
