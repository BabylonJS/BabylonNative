#pragma once

#include <napi/env.h>
#include <Babylon/Api.h>

#include <memory>

namespace Babylon::Polyfills
{
    class WebAudio final
    {
    public:
        class Impl;

        WebAudio(const WebAudio& other) = default;
        WebAudio& operator=(const WebAudio& other) = default;

        WebAudio(WebAudio&&) noexcept = default;
        WebAudio& operator=(WebAudio&&) noexcept = default;

        ~WebAudio();

        void SuspendPlayback() const;
        void ResumePlayback() const;
        void ShutdownPlayback() const;

        // This instance must live as long as the JS Runtime.
        // If JSRuntime is attached/detached (BabylonReactNative),
        // then this instance must live forever.
        [[nodiscard]] static WebAudio BABYLON_API Initialize(Napi::Env env);

    private:
        WebAudio(std::shared_ptr<Impl> impl);
        std::shared_ptr<Impl> m_impl{};
    };
}
