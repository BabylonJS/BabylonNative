#pragma once

#include <Windows.h>
#include <Babylon/JsRuntime.h>

#include "..\Window.h"

using WindowType = HWND;

namespace Babylon::Polyfills::Internal
{
    Napi::Value Window::GetDevicePixelRatio(const Napi::CallbackInfo& info)
    {
        auto& window = *static_cast<Window*>(info.Data());

        UINT dpi = GetDpiForWindow(*((HWND*)(window.m_windowPtr)));
        return Napi::Value::From(info.Env(), (float)dpi / 96.0f);
    }

    void Window::SetDevicePixelRatio(const Napi::CallbackInfo&, const Napi::Value& ){};
}