#pragma once

#include <Babylon/JsRuntime.h>

#include <memory>

namespace Babylon
{
    class NativeGraphics
    {
        struct Impl;

    public:
        ~NativeGraphics();

        class CallbackTicket
        {
            friend NativeGraphics::Impl;

        public:
            CallbackTicket(const CallbackTicket&) = delete;
            CallbackTicket(CallbackTicket&&) = default;

        private:
            struct Impl;
            std::unique_ptr<Impl> m_impl{};

            CallbackTicket();
        };

        template<typename NativeWindowT>
        static std::unique_ptr<NativeGraphics> InitializeFromWindow(NativeWindowT window, size_t width, size_t height);

        NativeGraphics(const NativeGraphics&) = delete;
        NativeGraphics(NativeGraphics&&) = delete;
        
        template<typename NativeWindowT>
        void ReinitializeFromWindow(NativeWindowT window, size_t width, size_t height);

    private:
        NativeGraphics();

        std::unique_ptr<Impl> m_impl{};
    };
}
