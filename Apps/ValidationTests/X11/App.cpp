#define XK_MISCELLANY
#define XK_LATIN1
#include <X11/keysymdef.h>
#include <X11/Xlib.h> // will include X11 which #defines None... Don't mess with order of includes.
#include <X11/Xutil.h>
#include <unistd.h> // syscall
#undef None
#include <filesystem>

#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics.h>
#include <Babylon/ScriptLoader.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <Babylon/Plugins/NativeOptimizations.h>
#include <Babylon/Plugins/TestUtils.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/Window.h>
#include <Babylon/Polyfills/XMLHttpRequest.h>
#include <Babylon/Polyfills/Canvas.h>

static const char* s_applicationName  = "BabylonNative Validation Tests";
static const char* s_applicationClass = "Validation Tests";

std::unique_ptr<Babylon::Graphics> graphics{};
std::unique_ptr<Babylon::Graphics::Update> update{};
std::unique_ptr<Babylon::AppRuntime> runtime{};
std::unique_ptr<Babylon::Polyfills::Canvas> nativeCanvas{};

static const int width = 600;
static const int height = 400;

namespace
{
    std::filesystem::path GetModulePath()
    {
        char exe[1024];

        int ret = readlink("/proc/self/exe", exe, sizeof(exe)-1);
        if(ret == -1)
        {
            exit(1);
        }
        exe[ret] = 0;
        return std::filesystem::path{exe};
    }

    std::string GetUrlFromPath(const std::filesystem::path path)
    {
        return std::string("file://") + path.generic_string();
    }

    void Uninitialize()
    {
        if (graphics)
        {
            update->Finish();
            graphics->FinishRenderingCurrentFrame();
        }
        runtime.reset();
        graphics.reset();
    }

    void InitBabylon(Window window)
    {
        std::string moduleRootUrl = GetUrlFromPath(GetModulePath().parent_path());

        Uninitialize();

        Babylon::WindowConfiguration graphicsConfig{};
        graphicsConfig.Window = window;
        graphicsConfig.Width = static_cast<size_t>(width);
        graphicsConfig.Height = static_cast<size_t>(height);

        graphics = Babylon::Graphics::CreateGraphics(graphicsConfig);
        update = std::make_unique<Babylon::Graphics::Update>(graphics->GetUpdate("update"));
        graphics->SetDiagnosticOutput([](const char* outputString) { printf("%s", outputString); fflush(stdout); });
        graphics->StartRenderingCurrentFrame();
        update->Start();

        runtime = std::make_unique<Babylon::AppRuntime>();

        // Initialize console plugin.
        runtime->Dispatch([window](Napi::Env env) {
            Babylon::Polyfills::Console::Initialize(env, [](const char* message, auto) {
                printf("%s", message);
                fflush(stdout);
            });

            Babylon::Plugins::TestUtils::Initialize(env, window);

            Babylon::Polyfills::Window::Initialize(env);
            Babylon::Polyfills::XMLHttpRequest::Initialize(env);
            nativeCanvas = std::make_unique <Babylon::Polyfills::Canvas>(Babylon::Polyfills::Canvas::Initialize(env));

            // Initialize NativeEngine plugin.
            graphics->AddToJavaScript(env);
            Babylon::Plugins::NativeEngine::Initialize(env);
            Babylon::Plugins::NativeOptimizations::Initialize(env);
        });

        Babylon::ScriptLoader loader{*runtime};
        loader.Eval("document = {}", "");
        loader.LoadScript(moduleRootUrl + "/Scripts/babylon.max.js");
        loader.LoadScript(moduleRootUrl + "/Scripts/babylonjs.loaders.js");
        loader.LoadScript(moduleRootUrl + "/Scripts/babylonjs.materials.js");
        loader.LoadScript(moduleRootUrl + "/Scripts/babylon.gui.js");
        loader.LoadScript(moduleRootUrl + "/Scripts/validation_native.js");
    }

    void UpdateWindowSize(float width, float height)
    {
        if (graphics != nullptr)
        {
            graphics->UpdateSize(static_cast<size_t>(width), static_cast<size_t>(height));
        }
    }
}

int main(int /*_argc*/, const char* const* /*_argv*/)
{
    XInitThreads();
    Display* display = XOpenDisplay(NULL);

    int32_t screen = DefaultScreen(display);
    int32_t depth  = DefaultDepth(display, screen);
    Visual* visual = DefaultVisual(display, screen);
    Window root   = RootWindow(display, screen);

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

    InitBabylon(window);
    UpdateWindowSize(width, height);

    bool doExit{false};
    while (!doExit)
    {
        if (!XPending(display) && graphics)
        {
            update->Finish();
            graphics->FinishRenderingCurrentFrame();
            graphics->StartRenderingCurrentFrame();
            update->Start();
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
                        doExit = true;
                    }
                    break;
                case ConfigureNotify:
                    {
                        const XConfigureEvent& xev = event.xconfigure;
                        UpdateWindowSize(xev.width, xev.height);
                    }
                    break;
            }
        }
    }
    Uninitialize();
    XDestroyIC(ic);
    XCloseIM(im);

    XUnmapWindow(display, window);
    XDestroyWindow(display, window);
    return Babylon::Plugins::TestUtils::errorCode;
}
