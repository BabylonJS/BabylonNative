#pragma once

#include <napi/env.h>

namespace Babylon::Plugins
{
    class Video final
    {
    public:
        class Impl;

        Video(const Video& other) = default;
        Video(Video&&) = default;
        ~Video() = default;

        static Video Initialize(Napi::Env env);


    private:
        Video(std::shared_ptr<Impl> impl);
        std::shared_ptr<Impl> m_impl{};
    };
}
