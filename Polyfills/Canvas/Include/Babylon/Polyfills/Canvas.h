#pragma once

#include <napi/env.h>
#include <Babylon/Api.h>

namespace Babylon::Polyfills
{
    class Canvas final
    {
    public:
        class Impl;

        Canvas(const Canvas& other) = default;
        Canvas& operator=(const Canvas& other) = default;

        Canvas(Canvas&&) noexcept = default;
        Canvas& operator=(Canvas&&) noexcept = default;

        ~Canvas();

        // Must outlive every JS runtime it is attached to.
        [[nodiscard]] static Canvas BABYLON_API Initialize(Napi::Env env);

        void FlushGraphicResources();

    private:
        Canvas(std::shared_ptr<Impl> impl);
        std::shared_ptr<Impl> m_impl{};
    };
} // namespace