#pragma once

#include <Babylon/JsRuntime.h>
#include <Babylon/Graphics/Platform.h>
#include <Babylon/Graphics/RendererType.h>
#include <Babylon/Graphics/Device.h>

#include <future>
#include <memory>

namespace Babylon::Graphics
{
    struct ExternalTextureCreateInfo
    {
        TexturePointerType Texture;
        Device* Device;
    };

    class ExternalTexture
    {
    public:
        ExternalTexture(ExternalTextureCreateInfo info);
        ExternalTexture(ExternalTexture&&);
        ~ExternalTexture();

        Napi::Value AddToContext(Napi::Value&);

        uint32_t GetWight();
        uint32_t GetHeight();

    private:
        struct Impl;
        std::unique_ptr<Impl> m_impl{};

        ExternalTexture(std::unique_ptr<Impl>);
    };
}
