#define XK_MISCELLANY
#define XK_LATIN1
#include <X11/keysymdef.h>
#include <X11/Xlib.h> // will include X11 which #defines None... Don't mess with order of includes.
#include <X11/Xutil.h>
#include <unistd.h> // syscall
#undef None
#include <cstring>
#include <iostream>
#include <optional>

#include <Babylon/Embedding/Runtime.h>
#include <Babylon/Embedding/View.h>
#include <Babylon/Plugins/TestUtils.h>
#include <Shared/Diagnostics.h>
#include <Shared/PlaygroundScripts.h>

static const char* s_applicationName  = "BabylonNative Playground";
static const char* s_applicationClass = "Playground";

namespace
{
    std::optional<Babylon::Embedding::Runtime> g_runtime;
    std::optional<Babylon::Embedding::View> g_view;

    void Uninitialize()
    {
        // View first (unbinds surface, closes in-flight frame), then
        // Runtime (joins JS thread).
        g_view.reset();
        g_runtime.reset();
    }

    void InitBabylon(Window window, int argc, const char* const* argv)
    {
        Uninitialize();

        Babylon::Embedding::RuntimeOptions runtimeOptions{};
        runtimeOptions.log = Playground::MakeLogCallback([](std::string_view text) {
            std::cout << text << std::endl;
        });

        g_runtime.emplace(std::move(runtimeOptions));
        Playground::Initialize();
        Playground::LoadBootstrapScripts(*g_runtime);

        if (argc == 1)
        {
            g_runtime->LoadScript("app:///Scripts/experience.js");
        }
        else
        {
            for (int i = 1; i < argc; ++i)
            {
                g_runtime->LoadScript(argv[i]);
            }

            g_runtime->LoadScript("app:///Scripts/playground_runner.js");
        }

        // First View attach triggers Device construction, plugin init, and
        // flushes the queued scripts.
        g_view.emplace(*g_runtime, window);
    }

    void UpdateWindowSize(uint32_t width, uint32_t height)
    {
        if (g_view)
        {
            // X11 reports surface dimensions in physical pixels.
            g_view->Resize(width, height, Babylon::Embedding::CoordinateUnits::Physical);
        }
    }
}

int main(int _argc, const char* const* _argv)
{
    Diagnostics::Initialize();

    XInitThreads();
    Display* display = XOpenDisplay(NULL);

    int32_t screen = DefaultScreen(display);
    int32_t depth  = DefaultDepth(display, screen);
    Visual* visual = DefaultVisual(display, screen);
    Window root   = RootWindow(display, screen);
    const int width = 600;
    const int height = 400;

    XSetWindowAttributes windowAttrs;
    windowAttrs.background_pixel = 0;
    windowAttrs.background_pixmap = 0;
    windowAttrs.border_pixel = 0;
    windowAttrs.event_mask = 0
            | ButtonPressMask
            | ButtonReleaseMask
            | ExposureMask
            | KeyPressMask
            | KeyReleaseMask
            | PointerMotionMask
            | StructureNotifyMask
            ;

    Window window = XCreateWindow(display
                            , root
                            , 0, 0
                            , width, height, 0
                            , depth
                            , InputOutput
                            , visual
                            , CWBorderPixel|CWEventMask
                            , &windowAttrs
                            );

    // Clear window to black.
    XSetWindowAttributes attr;
    memset(&attr, 0, sizeof(attr) );
    XChangeWindowAttributes(display, window, CWBackPixel, &attr);

    const char* wmDeleteWindowName = "WM_DELETE_WINDOW";
    Atom wmDeleteWindow;
    XInternAtoms(display, (char **)&wmDeleteWindowName, 1, False, &wmDeleteWindow);
    XSetWMProtocols(display, window, &wmDeleteWindow, 1);

    XMapWindow(display, window);
    XStoreName(display, window, s_applicationName);

    XClassHint* hint = XAllocClassHint();
    hint->res_name  = const_cast<char*>(s_applicationName);
    hint->res_class = const_cast<char*>(s_applicationClass);
    XSetClassHint(display, window, hint);
    XFree(hint);

    XIM im = XOpenIM(display, NULL, NULL, NULL);

    XIC ic = XCreateIC(im
            , XNInputStyle
            , 0
            | XIMPreeditNothing
            | XIMStatusNothing
            , XNClientWindow
            , window
            , NULL
            );

    InitBabylon(window, _argc, _argv);
    UpdateWindowSize(width, height);

    bool exit{};
    while (!exit)
    {
        if (!XPending(display) && g_view)
        {
            g_view->RenderFrame();
        }
        else
        {
            XEvent event;
            XNextEvent(display, &event);

            switch (event.type)
            {
                case Expose:
                    break;
                case ClientMessage:
                    if ( (Atom)event.xclient.data.l[0] == wmDeleteWindow)
                    {
                        Uninitialize();
                        exit = true;
                    }
                    break;
                case ConfigureNotify:
                    {
                        const XConfigureEvent& xev = event.xconfigure;
                        UpdateWindowSize(xev.width, xev.height);
                    }
                    break;
                case ButtonPress:
                    {
                        const XMotionEvent& xmotion = event.xmotion;
                        const XButtonEvent& xbutton = event.xbutton;

                        if (g_view) {
                            switch (xbutton.button) {
                                case Button1:
                                    g_view->OnMouseDown(Babylon::Embedding::View::LeftMouseButton(), xmotion.x, xmotion.y, Babylon::Embedding::CoordinateUnits::Physical);
                                    break;
                                case Button2:
                                    g_view->OnMouseDown(Babylon::Embedding::View::MiddleMouseButton(), xmotion.x, xmotion.y, Babylon::Embedding::CoordinateUnits::Physical);
                                    break;
                                case Button3:
                                    g_view->OnMouseDown(Babylon::Embedding::View::RightMouseButton(), xmotion.x, xmotion.y, Babylon::Embedding::CoordinateUnits::Physical);
                                    break;
                                case Button4:
                                    g_view->OnMouseWheel(Babylon::Embedding::View::MouseWheelY(), -120);
                                    break;
                                case Button5:
                                    g_view->OnMouseWheel(Babylon::Embedding::View::MouseWheelY(), 120);
                                    break;
                            }
                        }
                    }
                    break;
                case ButtonRelease:
                    {
                        const XMotionEvent& xmotion = event.xmotion;
                        const XButtonEvent& xbutton = event.xbutton;

                        if (g_view)
                        {
                            switch (xbutton.button)
                            {
                                case Button1:
                                    g_view->OnMouseUp(Babylon::Embedding::View::LeftMouseButton(), xmotion.x, xmotion.y, Babylon::Embedding::CoordinateUnits::Physical);
                                    break;
                                case Button2:
                                    g_view->OnMouseUp(Babylon::Embedding::View::MiddleMouseButton(), xmotion.x, xmotion.y, Babylon::Embedding::CoordinateUnits::Physical);
                                    break;
                                case Button3:
                                    g_view->OnMouseUp(Babylon::Embedding::View::RightMouseButton(), xmotion.x, xmotion.y, Babylon::Embedding::CoordinateUnits::Physical);
                                    break;
                            }
                        }
                    }
                    break;
                case MotionNotify:
                    {
                        const XMotionEvent& xmotion = event.xmotion;
                        if (g_view) {
                            g_view->OnMouseMove(xmotion.x, xmotion.y, Babylon::Embedding::CoordinateUnits::Physical);
                        }
                    }
                    break;
            }
        }
    }
    XDestroyIC(ic);
    XCloseIM(im);

    XUnmapWindow(display, window);
    XDestroyWindow(display, window);
    XCloseDisplay(display);

    return 0;
}
