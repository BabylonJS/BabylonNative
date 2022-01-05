#include <ExternalResourceWrapper.h>

namespace Babylon
{
    struct ExternalTexture::Impl
    {
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

    ExternalTexture ExternalTexture::CreateExternalTexture(Graphics&)
    {
        return {std::make_unique<ExternalTexture::Impl>()};
    }

    Napi::Object ExternalTexture::ConvertToNapiObject(Napi::Env env, ExternalTexture texturePtr)
    {
        return Napi::Object::New(env);
    }
}
