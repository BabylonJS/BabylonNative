#pragma once
#include <Babylon/JsRuntime.h>
#include <Babylon/Graphics/Platform.h>
#include <Babylon/Graphics/RendererType.h>
#include <Babylon/Graphics/Device.h>

#include <future>
#include <memory>

namespace Babylon::Graphics
{
    class ExternalTexture
    {
    public:
        ExternalTexture(TextureType nativeTexture);
        ExternalTexture(ExternalTexture&&);
        ~ExternalTexture();

        Napi::Promise AddToContext(Napi::Env&);

        uint32_t GetWight();
        uint32_t GetHeight();
        uint32_t GetFormat();

    private:
        struct Impl;
        std::unique_ptr<Impl> m_impl{};

        ExternalTexture(std::unique_ptr<Impl>);
    };
}
