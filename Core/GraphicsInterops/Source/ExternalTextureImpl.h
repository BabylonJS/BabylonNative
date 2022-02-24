#include <Babylon/Graphics/Device.h>
#include <Babylon/GraphicsInterops/ExternalTexture.h>
#include <Babylon/Graphics/RendererType.h>
#include <Babylon/Graphics/Texture.h>
#include <Babylon/Graphics/DeviceContext.h>

#include <napi/env.h>
#include <napi/napi_pointer.h>

namespace Babylon::Graphics
{
    struct ExternalTexture::Impl
    {
        Impl(TextureType nativeTexture);
        ~Impl();

        //TODO: Change this to take a context.
        Napi::Promise AddToContext(Napi::Env& env);

        uint32_t GetWight();

        uint32_t GetHeight();

        uint32_t GetFormat();

    private:
        bgfx::TextureFormat::Enum m_format{};
        uintptr_t m_nativeTexture{};
        uint32_t m_width{};
        uint32_t m_height{};
        uint32_t m_mips{};

        void GetNativeTextureData(TextureType nativeTexture);
    };
}