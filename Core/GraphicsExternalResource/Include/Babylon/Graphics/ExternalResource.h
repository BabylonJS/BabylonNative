#pragma once

#include <napi/env.h>

#include <Babylon/Graphics.h>

#include <memory>

namespace Babylon
{
    class ExternalTexture
    {
    public:
        ExternalTexture(Graphics&, TexturePointerType nativeTexturePointer);
        ExternalTexture(ExternalTexture&&);
        ~ExternalTexture();

        Napi::Value AddToContext(Napi::Value&);

    private:
        struct Impl;
        std::unique_ptr<Impl> m_impl{};
        
        ExternalTexture(std::unique_ptr<Impl>);
    };
}
