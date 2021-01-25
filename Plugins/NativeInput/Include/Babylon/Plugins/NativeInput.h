#pragma once

#include <napi/env.h>

namespace Babylon::Plugins
{
    class NativeInput final
    {
    public:
        enum class DeviceType
        {
            Generic = 0,
            Keyboard = 1,
            Mouse = 2,
            Touch = 3,
            DualShock = 4,
            Xbox = 5,
            Switch = 6,
        };

        // TODO: Ideally instances of these should be scoped to individual views within an env, but we don't yet support multi-view.
        // See https://github.com/BabylonJS/BabylonNative/issues/147
        static NativeInput& CreateForJavaScript(Napi::Env);
        static NativeInput& GetFromJavaScript(Napi::Env);

        void PointerDown(uint32_t pointerId, uint32_t buttonIndex, uint32_t x, uint32_t y, DeviceType deviceType);
        void PointerUp(uint32_t pointerId, uint32_t buttonIndex, uint32_t x, uint32_t y, DeviceType deviceType);
        void PointerMove(uint32_t pointerId, uint32_t x, uint32_t y, DeviceType deviceType);

    private:
        NativeInput(const NativeInput&) = delete;
        NativeInput(NativeInput&&) = delete;
        NativeInput(Napi::Env);
        class Impl;
        std::unique_ptr<Impl> m_impl{};
    };
}
