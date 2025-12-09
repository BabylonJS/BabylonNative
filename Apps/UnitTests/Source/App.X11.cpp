// gtest.h included here and in App.h because of a preprocessor conflict
#include <gtest/gtest.h>
#define XK_MISCELLANY
#define XK_LATIN1
#include <X11/Xlib.h> // will include X11 which #defines None... Don't mess with order of includes.
#include <X11/Xutil.h>
#undef None
#include "App.h"
#include <Babylon/DebugTrace.h>

namespace
{
    constexpr const char* applicationName = "Babylon Native Unit Tests";
    constexpr const  int width = 640;
    constexpr const int height = 480;
    constexpr const char* wmDeleteWindowName = "WM_DELETE_WINDOW";
}

int main(int argc, char* argv[])
{
    XInitThreads();
    Display* display = XOpenDisplay(NULL);
    int32_t screen = DefaultScreen(display);
    int32_t depth = DefaultDepth(display, screen);
    Visual* visual = DefaultVisual(display, screen);
    Window root = RootWindow(display, screen);

    XSetWindowAttributes windowAttrs;
    windowAttrs.background_pixel = 0;
    windowAttrs.background_pixmap = 0;
    windowAttrs.border_pixel = 0;
    windowAttrs.event_mask = 0;

    Window window = XCreateWindow(display, root, 0, 0, width, height, 0, depth, InputOutput, visual, CWBorderPixel | CWEventMask, &windowAttrs);

    // Clear window to black.
    XSetWindowAttributes attr;
    memset(&attr, 0, sizeof(attr));
    XChangeWindowAttributes(display, window, CWBackPixel, &attr);

    Atom wmDeleteWindow;
    XInternAtoms(display, (char**)&wmDeleteWindowName, 1, False, &wmDeleteWindow);
    XSetWMProtocols(display, window, &wmDeleteWindow, 1);

    XMapWindow(display, window);
    XStoreName(display, window, applicationName);

    Babylon::Graphics::Configuration config{};
    config.Window = window;
    config.Width = static_cast<size_t>(width);
    config.Height = static_cast<size_t>(height);

    Babylon::DebugTrace::EnableDebugTrace(true);
    Babylon::DebugTrace::SetTraceOutput([](const char* trace) { printf("%s\n", trace); fflush(stdout); });

    int ret = RunTests(config, argc, argv);
    XCloseDisplay(display);
    return ret;
}