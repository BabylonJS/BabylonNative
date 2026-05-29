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
    // Suppress C4702 for the shared dispatcher - Impl stubs always throw,
    // making the dispatcher's post-call code unreachable.
    class ExternalTexture::Impl final : public ImplBase
    {
    public:
        // Implemented in ExternalTexture_Shared.h
        Impl(Graphics::TextureT, std::optional<Graphics::TextureFormatT>);
        void Update(Graphics::TextureT, std::optional<Graphics::TextureFormatT>);

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

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4702) // unreachable code (Impl stubs always throw)
#endif

#include "ExternalTexture_Shared.h"

#if defined(_MSC_VER)
#pragma warning(pop)
#endif
