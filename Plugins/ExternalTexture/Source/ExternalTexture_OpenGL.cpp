#include <Babylon/Graphics/Device.h>
#include <Babylon/Plugins/ExternalTexture/ExternalTexture.h>
#include <Babylon/Graphics/RendererType.h>
#include <Babylon/Graphics/Texture.h>
#include <Babylon/Graphics/DeviceContext.h>

#include <napi/env.h>
#include <napi/napi_pointer.h>

namespace Babylon::Plugins
{
    void ExternalTexture::ReadPropertiesFromNativeTexture(Babylon::Graphics::TextureType) 
    {
        //TODO: Add support for OpenGL external textures.
        throw std::runtime_error{"External textures is not currenlty supported for OpenGL."};
    }
}
