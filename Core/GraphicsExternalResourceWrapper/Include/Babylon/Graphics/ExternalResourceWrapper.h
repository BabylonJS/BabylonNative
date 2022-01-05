#pragma once

#include <napi/env.h>

#include <Babylon/Graphics.h>

#include <memory>

namespace Babylon
{
    template<typename T>
    class Shell
    {
    public:
        Shell(T&& t)
            : m_ptr{new T(std::forward<T>(t))}
        {
        }

        T Unshell()
        {
            return {std::move(*m_ptr)};
        }

    private:
        T* m_ptr{};
    };

    class ExternalTexture
    {
    public:
        ExternalTexture(ExternalTexture&&);
        ~ExternalTexture();

        static ExternalTexture CreateExternalTexture(Graphics&);
        static Napi::Object ConvertToNapiObject(Napi::Env, ExternalTexture);

    private:
        struct Impl;
        std::unique_ptr<Impl> m_impl{};
        
        ExternalTexture(std::unique_ptr<Impl>);
    };
}
