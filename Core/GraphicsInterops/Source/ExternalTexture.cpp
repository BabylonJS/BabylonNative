#include <Babylon/Graphics/Device.h>
#include <Babylon/GraphicsInterops/ExternalTexture.h>
#include <Babylon/Graphics/Texture.h>
#include <Babylon/Graphics/DeviceContext.h>

#include "ExternalTextureImpl.h"

#include <napi/env.h>
#include <napi/napi_pointer.h>

namespace Babylon::Graphics
{

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

    ExternalTexture::ExternalTexture(TextureType nativeTexture)
        : ExternalTexture{std::make_unique<ExternalTexture::Impl>(nativeTexture)}
    {
    }

    Napi::Promise ExternalTexture::AddToContext(Napi::Env& env)
    {
        return m_impl->AddToContext(env);
    }

    uint32_t ExternalTexture::GetWight() 
    {
        return m_impl->GetWight();
    }

    uint32_t ExternalTexture::GetHeight()
    {
        return m_impl->GetHeight();
    }

    uint32_t ExternalTexture::GetFormat() 
    {
        return m_impl->GetFormat();
    }
}