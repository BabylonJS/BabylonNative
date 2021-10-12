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

    void Video::Impl::Open(const std::string& /*source*/)
    {
        //throw std::runtime_error{ "Not implemented for this platform." };
    }
    
    void Video::Impl::UpdateTexture(bgfx::TextureHandle /*textureHandle*/)
    {
        //throw std::runtime_error{ "Not implemented for this platform." };
    }

    void Video::Impl::AddFrame(uint8_t* /*data*/, size_t /*dataLength*/)
    {
    }

    void Video::Impl::Stop()
    {
    }

    void Video::Impl::Close()
    {
        throw std::runtime_error{ "Not implemented for this platform." };
    }
}
 