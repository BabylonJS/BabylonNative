#pragma once

#include <napi/env.h>
#include <Babylon/Api.h>

namespace Babylon::Plugins
{
    class NativeInput final
    {
    public:
        // TODO: Ideally instances of these should be scoped to individual views within an env, but we don't yet support multi-view.
        // See https://github.com/BabylonJS/BabylonNative/issues/147
        static NativeInput& BABYLON_API CreateForJavaScript(Napi::Env);
        static NativeInput& BABYLON_API GetFromJavaScript(Napi::Env);

        // The `x` and `y` coordinates are in device-independent pixels (a.k.a., dp or dip) and are relative to the top-left
        // corner of the target back buffer which is typically the window provided to `Babylon::Graphics::Device`.

        void MouseDown(uint32_t buttonIndex, int32_t x, int32_t y);
        void MouseUp(uint32_t buttonIndex, int32_t x, int32_t y);
        void MouseMove(int32_t x, int32_t y);
        void MouseWheel(uint32_t wheelAxis, int32_t scrollValue);
        void TouchDown(uint32_t pointerId, int32_t x, int32_t y);
        void TouchUp(uint32_t pointerId, int32_t x, int32_t y);
        void TouchMove(uint32_t pointerId, int32_t x, int32_t y);

        static constexpr uint32_t LEFT_MOUSE_BUTTON_ID{0};
        static constexpr uint32_t MIDDLE_MOUSE_BUTTON_ID{1};
        static constexpr uint32_t RIGHT_MOUSE_BUTTON_ID{2};
        static constexpr uint32_t MOUSEWHEEL_Y_ID{8};
        static constexpr uint32_t TOUCH_BUTTON_ID{0};

    private:
        NativeInput(const NativeInput&) = delete;
        NativeInput& operator=(const NativeInput&) = delete;

        NativeInput(Napi::Env);

        class Impl;
        std::unique_ptr<Impl> m_impl{};
    };
}
