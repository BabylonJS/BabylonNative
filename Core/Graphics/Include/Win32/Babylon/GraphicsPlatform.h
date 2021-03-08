#pragma once

#include <Windows.h>

using WindowType = HWND;

struct GraphicsConfiguration
{
    WindowType windowPtr;
    size_t width;
    size_t height;
};