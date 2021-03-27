#pragma once

#include <Windows.h>
using WindowType = HWND;

struct Babylon::Graphics::Configuration
{
    WindowType WindowPtr;
    size_t Width;
    size_t Height;
};
