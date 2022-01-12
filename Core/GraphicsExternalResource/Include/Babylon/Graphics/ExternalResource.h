#pragma once

#include <napi/env.h>

#include <Babylon/Graphics.h>

#include <memory>

namespace Babylon
{
    class ExternalTexture
    {
    public:
        ExternalTexture(Graphics&, uintptr_t nativeTexturePointer = 0);
        ExternalTexture(ExternalTexture&&);
        ~ExternalTexture();

        void OverrideInternalTexture(uintptr_t nativeTexturePtr);

        static Napi::Object ConvertToNapiObject(Napi::Env, ExternalTexture);

    private:
        struct Impl;
        std::unique_ptr<Impl> m_impl{};
        
        ExternalTexture(std::unique_ptr<Impl>);
    };
}
