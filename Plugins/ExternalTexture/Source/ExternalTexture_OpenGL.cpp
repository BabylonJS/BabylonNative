#include <Babylon/Plugins/ExternalTexture.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Graphics/DeviceContext.h>
#include <Babylon/Graphics/RendererType.h>
#include <Babylon/Graphics/Texture.h>
#include <napi/napi_pointer.h>
#include <bx/bx.h>
#include <bgfx/bgfx.h>

#include "ExternalTexture_Base.h"

namespace Babylon::Plugins
{
    class ExternalTexture::Impl final : public ImplBase
    {
    public:
        Impl(Graphics::TextureT);

        void Update(Graphics::TextureT);

        uintptr_t Ptr() const
        {
            throw std::runtime_error{"not implemented"};
        }

    private:
        void Init(Graphics::TextureT)
        {
            throw std::runtime_error{"not implemented"};
        }

        void GetInfo(uint16_t&, uint16_t&, bool&, bgfx::TextureFormat::Enum&, uint64_t&)
        {
            throw std::runtime_error{"not implemented"};
        }
    };
}

#include "ExternalTexture_Shared.h"
