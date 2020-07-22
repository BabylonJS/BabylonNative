#pragma once

#include <Babylon/JsRuntime.h>

#include <memory>

namespace Babylon
{
    class NativeGraphics
    {
    public:
        template<typename NativeWindowT>
        static NativeGraphics InitializeFromWindow(NativeWindowT window, size_t width, size_t height);

        NativeGraphics(const NativeGraphics&) = delete;
        NativeGraphics(NativeGraphics&&) = default;

    private:
        NativeGraphics();

        struct Impl;
        std::unique_ptr<Impl> m_impl{};
    };
}
