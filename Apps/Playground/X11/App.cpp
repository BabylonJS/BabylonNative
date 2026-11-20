#define XK_MISCELLANY
#define XK_LATIN1
#include <X11/keysymdef.h>
#include <X11/Xlib.h> // will include X11 which #defines None... Don't mess with order of includes.
#include <X11/Xutil.h>
#ifdef PLAYGROUND_EGL_VISUAL
#include <EGL/egl.h>
#endif
#include <unistd.h> // syscall
#undef None
#include <cstdio>
#include <cstring>
#include <iostream>
#include <optional>

#include <Babylon/Embedding/Runtime.h>
#include <Babylon/Embedding/View.h>
#include <Babylon/Plugins/TestUtils.h>
#include <Shared/CommandLine.h>
#include <Shared/Diagnostics.h>
#include <Shared/PlaygroundScripts.h>

static const char* s_applicationName  = "BabylonNative Playground";
static const char* s_applicationClass = "Playground";

namespace
{
    std::optional<Babylon::Embedding::Runtime> g_runtime;
    std::optional<Babylon::Embedding::View> g_view;

#ifdef PLAYGROUND_EGL_VISUAL
    // bgfx renders through the first EGL config matching these attributes (OpenGL ES 3.0, the
    // default BGRA8 back buffer with D24S8 depth/stencil). NVIDIA's EGL refuses to create a window
    // surface whose X visual differs from that config's, so the window is created with its visual.
    bool GetEglVisual(Display* display, XVisualInfo& visualInfo)
    {
        EGLDisplay eglDisplay = eglGetDisplay(display);
        if (eglDisplay == EGL_NO_DISPLAY || !eglInitialize(eglDisplay, nullptr, nullptr))
        {
            return false;
        }

        const EGLint attributes[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
            EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_ALPHA_SIZE, 8,
            EGL_DEPTH_SIZE, 24,
            EGL_STENCIL_SIZE, 8,
            EGL_SAMPLES, 0,
            EGL_NONE,
        };

        EGLConfig config{};
        EGLint count{};
        EGLint visualId{};
        const bool found = eglChooseConfig(eglDisplay, attributes, &config, 1, &count) && count > 0 &&
                           eglGetConfigAttrib(eglDisplay, config, EGL_NATIVE_VISUAL_ID, &visualId) && visualId != 0;
        eglTerminate(eglDisplay);
        if (!found)
        {
            return false;
        }

        XVisualInfo visualTemplate{};
        visualTemplate.visualid = static_cast<VisualID>(visualId);
        int visualCount{};
        XVisualInfo* visuals = XGetVisualInfo(display, VisualIDMask, &visualTemplate, &visualCount);
        if (visuals == nullptr)
        {
            return false;
        }

        visualInfo = visuals[0];
        XFree(visuals);
        return true;
    }
#endif

    void Uninitialize()
    {
        // View first (unbinds surface, closes in-flight frame), then
        // Runtime (joins JS thread).
        g_view.reset();
        g_runtime.reset();
    }

    void InitBabylon(Window window, const PlaygroundOptions& options)
    {
        Uninitialize();

        Babylon::Embedding::RuntimeOptions runtimeOptions{};
        if (options.DebugTrace)
        {
            runtimeOptions.enableDebugTrace = *options.DebugTrace;
        }
        runtimeOptions.log = Playground::MakeLogCallback([](std::string_view text) {
            std::cout << text << std::endl;
        });

        g_runtime.emplace(std::move(runtimeOptions));
        Playground::Initialize(options);
        Playground::QueueOptions(*g_runtime, options);
        Playground::LoadBootstrapScripts(*g_runtime);

        if (options.Scripts.empty())
        {
            g_runtime->LoadScript(options.ListTests ? "app:///Scripts/validation_native.js" : "app:///Scripts/experience.js");
        }
        else
        {
            for (const auto& script : options.Scripts)
            {
                g_runtime->LoadScript(script);
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
    std::setvbuf(stdout, nullptr, _IONBF, 0);
    std::setvbuf(stderr, nullptr, _IONBF, 0);
    Diagnostics::Initialize();
    Babylon::Plugins::TestUtils::SetExitCallback([](int code) {
        Diagnostics::SetExitCode(code);
        Diagnostics::PrintFinishLine();
    });

    const auto options = CommandLine::Parse(_argc, _argv);
    if (options.ParseError)
    {
        std::cerr << "Error: " << options.ErrorMessage << "\n\n";
        CommandLine::PrintUsage(_argv[0]);
        Diagnostics::SetExitCode(2);
        return 2;
    }
    if (options.ShowHelp)
    {
        CommandLine::PrintUsage(_argv[0]);
        return 0;
    }

    XInitThreads();
    Display* display = XOpenDisplay(NULL);

    int32_t screen = DefaultScreen(display);
    int32_t depth  = DefaultDepth(display, screen);
    Visual* visual = DefaultVisual(display, screen);
    Window root   = RootWindow(display, screen);
    Colormap colormap{};
#ifdef PLAYGROUND_EGL_VISUAL
    XVisualInfo eglVisual{};
    if (GetEglVisual(display, eglVisual))
    {
        visual = eglVisual.visual;
        depth = eglVisual.depth;
        colormap = XCreateColormap(display, root, visual, AllocNone);
    }
#endif
    const int width = 600;
    const int height = 400;

    XSetWindowAttributes windowAttrs;
    windowAttrs.background_pixel = 0;
    windowAttrs.background_pixmap = 0;
    windowAttrs.border_pixel = 0;
    windowAttrs.colormap = colormap;
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
                            , CWBorderPixel|CWEventMask|(colormap != 0 ? CWColormap : 0)
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

    if (!options.Headless)
    {
        XMapWindow(display, window);
    }
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

    InitBabylon(window, options);
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
    if (colormap != 0)
    {
        XFreeColormap(display, colormap);
    }
    XCloseDisplay(display);

    return 0;
}
