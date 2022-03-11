#include <Babylon/Graphics/Device.h>
#include <Babylon/GraphicsInterops/ExternalTexture.h>
#include <Babylon/Graphics/RendererType.h>
#include <Babylon/Graphics/Texture.h>
#include <Babylon/Graphics/DeviceContext.h>

#include <ExternalTextureImpl.h>

#include <napi/env.h>
#include <napi/napi_pointer.h>

namespace Babylon::Graphics
{
    void ExternalTexture::Impl::GetNativeTextureData(TextureType nativeTexture) 
    {
        //TODO: Add support for Metal external textures.
        throw std::runtime_error{"External textures is not currenlty supported for Metal."};
    }
}
