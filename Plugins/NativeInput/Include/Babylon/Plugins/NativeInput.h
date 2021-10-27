#pragma once

#include <napi/env.h>

namespace Babylon::Plugins
{
    class NativeInput final
    {
    public:
        // TODO: Ideally instances of these should be scoped to individual views within an env, but we don't yet support multi-view.
        // See https://github.com/BabylonJS/BabylonNative/issues/147
        static NativeInput& CreateForJavaScript(Napi::Env);
        static NativeInput& GetFromJavaScript(Napi::Env);

        void MouseDown(uint32_t buttonIndex, uint32_t x, uint32_t y);
        void MouseUp(uint32_t buttonIndex, uint32_t x, uint32_t y);
        void MouseMove(uint32_t x, uint32_t y);
        void MouseWheel(uint32_t deltaY);
        void TouchDown(uint32_t pointerId, uint32_t x, uint32_t y);
        void TouchUp(uint32_t pointerId, uint32_t x, uint32_t y);
        void TouchMove(uint32_t pointerId, uint32_t x, uint32_t y);

        static constexpr uint32_t LEFT_MOUSE_BUTTON_ID{ 0 };
        static constexpr uint32_t MIDDLE_MOUSE_BUTTON_ID{ 1 };
        static constexpr uint32_t RIGHT_MOUSE_BUTTON_ID{ 2 };
        static constexpr uint32_t TOUCH_BUTTON_ID{ 0 };

    private:
        NativeInput(const NativeInput&) = delete;
        NativeInput(NativeInput&&) = delete;
        NativeInput(Napi::Env);
        class Impl;
        std::unique_ptr<Impl> m_impl{};
    };
}
