#pragma once

#include <Windows.h>
#include <Babylon/Graphics.h>

using WindowType = HWND;

struct Babylon::GraphicsConfiguration
{
    WindowType WindowPtr;
    size_t Width;
    size_t Height;
};
