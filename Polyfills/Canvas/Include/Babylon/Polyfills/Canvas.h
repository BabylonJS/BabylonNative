#pragma once

#include <napi/env.h>

namespace Babylon::Polyfills
{
    class Canvas final
    {
    public:
        class Impl;

        Canvas(const Canvas& other) = default;
        Canvas(Canvas&&) = default;
        ~Canvas();

        // This instance must live as long as the JS Runtime.
        // If JSRuntime is attached/detached (BabylonReactNative)
        // Then this instance must live forever.
        [[nodiscard]] static Canvas Initialize(Napi::Env env);

        void FlushGraphicResources();

    private:
        Canvas(std::shared_ptr<Impl> impl);
        std::shared_ptr<Impl> m_impl{};
    };
} // namespace