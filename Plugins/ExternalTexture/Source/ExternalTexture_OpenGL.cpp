#include <Babylon/Plugins/ExternalTexture.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Graphics/DeviceContext.h>
#include <Babylon/Graphics/RendererType.h>
#include <Babylon/Graphics/Texture.h>
#include <napi/pointer.h>
#include <bx/bx.h>
#include <bgfx/bgfx.h>

#include "ExternalTexture_Base.h"

namespace Babylon::Plugins
{
    class ExternalTexture::Impl final : public ImplBase
    {
    public:
        // Implemented in ExternalTexture_Shared.h
        Impl(Graphics::TextureT, std::optional<Graphics::TextureFormatT>);
        void Update(Graphics::TextureT, std::optional<Graphics::TextureFormatT>, std::optional<uint16_t>);

        Graphics::TextureT Get() const
        {
            throw std::runtime_error{"not implemented"};
        }

    private:
        static void GetInfo(Graphics::TextureT, std::optional<Graphics::TextureFormatT>, Info&)
        {
            throw std::runtime_error{"not implemented"};
        }

        void Set(Graphics::TextureT)
        {
            throw std::runtime_error{"not implemented"};
        }
    };
}

#include "ExternalTexture_Shared.h"
