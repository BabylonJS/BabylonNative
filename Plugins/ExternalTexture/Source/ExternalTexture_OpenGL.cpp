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
    class ExternalTexture::Impl final extends ImplBase
    {
    public:
        Impl(Graphics::TextureT)
        {
            throw std::runtime_error{"not implemented"};
        }

        uint16_t Width() const
        {
            throw std::runtime_error{"not implemented"};
        }

        uint16_t Height() const
        {
            throw std::runtime_error{"not implemented"};
        }

        bgfx::TextureFormat::Enum Format() const
        {
            throw std::runtime_error{"not implemented"};
        }

        bool HasMips() const
        {
            throw std::runtime_error{"not implemented"};
        }

        uint64_t Flags() const
        {
            throw std::runtime_error{"not implemented"};
        }

        uintptr_t Ptr() const
        {
            throw std::runtime_error{"not implemented"};
        }
    };
}

#include "ExternalTexture_Shared.h"
