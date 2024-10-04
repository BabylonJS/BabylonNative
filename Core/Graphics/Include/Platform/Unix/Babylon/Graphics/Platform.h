#pragma once

#include <X11/Xlib.h>

#include <tuple>

namespace Babylon::Graphics
{
    using WindowT = std::tuple<Window, Display*>;
}
