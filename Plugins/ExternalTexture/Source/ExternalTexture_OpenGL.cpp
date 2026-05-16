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
    // The OpenGL backend does not implement ExternalTexture. The Impl methods
    // below return default-constructed values rather than throwing so that the
    // shared dispatchers in ExternalTexture_Shared.h (which is included after
    // this class definition) don't produce unreachable-code paths that MSVC
    // flags as C4702 under /WX. Any caller that actually invokes an
    // ExternalTexture API on OpenGL will receive an inert/null texture and
    // get a graphics-level error downstream.
    class ExternalTexture::Impl final : public ImplBase
    {
    public:
        // Implemented in ExternalTexture_Shared.h
        Impl(Graphics::TextureT, std::optional<Graphics::TextureFormatT>);
        void Update(Graphics::TextureT, std::optional<Graphics::TextureFormatT>, std::optional<uint16_t>);

        Graphics::TextureT Get() const
        {
            return Graphics::TextureT{};
        }

    private:
        static void GetInfo(Graphics::TextureT, std::optional<Graphics::TextureFormatT>, Info&)
        {
        }

        void Set(Graphics::TextureT)
        {
        }
    };
}

#include "ExternalTexture_Shared.h"
