#pragma once
#include <Babylon/JsRuntime.h>
#include <Babylon/Graphics/Platform.h>
#include <Babylon/Graphics/RendererType.h>

#include <future>
#include <memory>

namespace Babylon::Plugins
{
    class ExternalTexture
    {
    public:
        ExternalTexture(Babylon::Graphics::TextureT nativeTexture);
        ~ExternalTexture();

        Napi::Promise AddToContext(Napi::Env&) const;

        uint32_t GetWidth() const;
        uint32_t GetHeight() const;
        uint32_t GetFormat() const;

    private:
        uintptr_t m_nativeTexture{};
        uint32_t m_width{};
        uint32_t m_height{};
        uint32_t m_mips{};
        uint32_t m_format{};
        uint64_t m_flags{};

        void ReadPropertiesFromNativeTexture(Babylon::Graphics::TextureT nativeTexture);
    };
}
