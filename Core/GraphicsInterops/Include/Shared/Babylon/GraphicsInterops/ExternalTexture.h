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
        ~ExternalTexture();

        Napi::Promise AddToContext(Napi::Env&);

        uint32_t GetWidth();
        uint32_t GetHeight();
        uint32_t GetFormat();

    private:
        uintptr_t m_nativeTexture{};
        uint32_t m_width{};
        uint32_t m_height{};
        uint32_t m_mips{};
        uint32_t m_format{};

        void ReadPropertiesFromNativeTexture(TextureType nativeTexture);
    };
}
