#pragma once

#include <Babylon/JsRuntime.h>

#include <memory>

namespace Babylon
{
    class NativeGraphics
    {
    public:
        class Impl;
        friend Impl;

        ~NativeGraphics();

        template<typename NativeWindowT>
        static std::unique_ptr<NativeGraphics> InitializeFromWindow(NativeWindowT window, size_t width, size_t height);

        template<typename NativeWindowT>
        void ReinitializeFromWindow(NativeWindowT window, size_t width, size_t height);

        struct Frame
        {
            Frame(NativeGraphics::Impl& graphicsImpl);
            ~Frame();

            Frame(const Frame&) = delete;
            Frame(Frame&&) = delete;

        private:
            NativeGraphics::Impl& m_graphicsImpl;
        };
        std::unique_ptr<Frame> AdvanceFrame();

        void UpdateSize(size_t width, size_t height);

    private:
        NativeGraphics();
        NativeGraphics(const NativeGraphics&) = delete;
        NativeGraphics(NativeGraphics&&) = delete;

        std::unique_ptr<Impl> m_impl{};
    };
}
