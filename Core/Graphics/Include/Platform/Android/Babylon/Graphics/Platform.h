#pragma once

#include <android/native_window.h>

namespace Babylon::Graphics
{
    using WindowType = ANativeWindow*;

    struct WindowConfiguration
    {
        WindowType Window{};
        size_t Width{};
        size_t Height{};
    };
}
