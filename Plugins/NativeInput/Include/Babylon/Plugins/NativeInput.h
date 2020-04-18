#pragma once

#include <napi/env.h>

namespace Babylon::Plugins
{
    class NativeInput
    {
    public:
        // TODO: Ideally instances of these should be scoped to individual views within an env, but we don't yet support multi-view.
        static NativeInput& CreateForJavaScript(Napi::Env);

        void PointerDown(uint32_t pointerId, uint32_t buttonIndex, uint32_t x, uint32_t y);
        void PointerUp(uint32_t pointerId, uint32_t buttonIndex, uint32_t x, uint32_t y);
        void PointerMove(uint32_t pointerId, uint32_t x, uint32_t y);

    protected:
        NativeInput(const NativeInput&) = delete;
        NativeInput(NativeInput&&) = delete;

    private:
        NativeInput(Napi::Env);
        class Impl;
        std::unique_ptr<Impl> m_impl{};

    private:
    };
}
