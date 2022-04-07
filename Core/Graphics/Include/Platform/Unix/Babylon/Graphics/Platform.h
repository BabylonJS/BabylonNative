#pragma once

#include <X11/Xlib.h>

namespace Babylon::Graphics
{
    using WindowType = Window;

    struct WindowConfiguration
    {
        WindowType Window{};
        size_t Width{};
        size_t Height{};
    };
}
