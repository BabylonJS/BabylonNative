#pragma once

#include <windows.ui.core.h>

using WindowType = ABI::Windows::UI::Core::ICoreWindow*;

struct GraphicsConfiguration
{
    WindowType windowPtr;
    void* windowTypePtr;
    size_t width;
    size_t height;
};