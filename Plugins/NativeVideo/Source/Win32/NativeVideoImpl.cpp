#include "NativeVideoImpl.h"
#include <napi/napi.h>

namespace Babylon::Plugins
{
    Video::Impl::Impl(Napi::Env /*env*/)
    {
    }

    Video::Impl::~Impl()
    {
    }

    void Video::Impl::Open(uint32_t /*width*/, uint32_t /*height*/, bool /*frontCamera*/)
    {
        throw std::runtime_error{ "Not implemented for this platform." };
    }
    
    void Video::Impl::UpdateTexture(bgfx::TextureHandle /*textureHandle*/)
    {
        throw std::runtime_error{ "Not implemented for this platform." };
    }

    void Video::Impl::Close()
    {
        throw std::runtime_error{ "Not implemented for this platform." };
    }
}
 