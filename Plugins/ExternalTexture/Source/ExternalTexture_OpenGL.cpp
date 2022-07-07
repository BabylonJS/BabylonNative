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
        Impl(Graphics::TextureT ptr);
        void Update(Graphics::TextureT ptr);
        uintptr_t Ptr() const;

    private:
        void Init(Graphics::TextureT ptr)
        {
            throw std::runtime_error{"not implemented"};
        }

        void GetInfo(uint16_t& width, uint16_t& height, bool& hasMips, bgfx::TextureFormat::Enum& format, uint64_t& flags)
        {
            throw std::runtime_error{"not implemented"};
        }

        unsigned int m_ptr;
    };
}

#include "ExternalTexture_Shared.h"
