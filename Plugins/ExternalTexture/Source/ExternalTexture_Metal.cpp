#include <Babylon/Plugins/ExternalTexture.h>
#include <Babylon/Graphics/RendererType.h>

#include <bgfx/bgfx.h>

namespace Babylon::Plugins
{
    void ExternalTexture::ReadPropertiesFromNativeTexture(Babylon::Graphics::TextureType) 
    {
        //TODO: Add support for Metal external textures.
        throw std::runtime_error{"External textures is not currenlty supported for Metal."};
    }
}
