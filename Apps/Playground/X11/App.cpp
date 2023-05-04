#define XK_MISCELLANY
#define XK_LATIN1
#include <X11/keysymdef.h>
#include <X11/Xlib.h> // will include X11 which #defines None... Don't mess with order of includes.
#include <X11/Xutil.h>
#include <unistd.h> // syscall
#undef None
#include <filesystem>
#include <optional>

#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/ScriptLoader.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <Babylon/Plugins/NativeOptimizations.h>
#include <Babylon/Plugins/NativeInput.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/Window.h>
#include <Babylon/Polyfills/XMLHttpRequest.h>
#include <Babylon/Polyfills/Canvas.h>

static const char* s_applicationName  = "BabylonNative Playground";
static const char* s_applicationClass = "Playground";

std::optional<Babylon::Graphics::Device> device{};
std::optional<Babylon::Graphics::DeviceUpdate> update{};
std::optional<Babylon::AppRuntime> runtime{};
std::optional<Babylon::Polyfills::Canvas> nativeCanvas{};
Babylon::Plugins::NativeInput* nativeInput{};

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
        if (device)
        {
            update->Finish();
            device->FinishRenderingCurrentFrame();
        }

        nativeInput = {};
        nativeCanvas.reset();
        runtime.reset();
        update.reset();
        device.reset();
    }

    void InitBabylon(Window window, int width, int height, int argc, const char* const* argv)
    {
        std::vector<std::string> scripts(argv + 1, argv + argc);
        std::string moduleRootUrl = GetUrlFromPath(GetModulePath().parent_path());

        Uninitialize();

        Babylon::Graphics::Configuration graphicsConfig{};
        graphicsConfig.Window = window;
        graphicsConfig.Width = static_cast<size_t>(width);
        graphicsConfig.Height = static_cast<size_t>(height);
        graphicsConfig.MSAASamples = 4;

        device.emplace(graphicsConfig);
        update.emplace(device->GetUpdate("update"));
        device->StartRenderingCurrentFrame();
        update->Start();

        runtime.emplace();

        runtime->Dispatch([](Napi::Env env) {
            Babylon::Polyfills::Console::Initialize(env, [](const char* message, auto) {
                printf("%s", message);
                fflush(stdout);
            });

            Babylon::Polyfills::Window::Initialize(env);
            Babylon::Polyfills::XMLHttpRequest::Initialize(env);
            nativeCanvas.emplace(Babylon::Polyfills::Canvas::Initialize(env));

            // Initialize NativeEngine plugin.
            device->AddToJavaScript(env);
            Babylon::Plugins::NativeEngine::Initialize(env);

            Babylon::Plugins::NativeOptimizations::Initialize(env);

            nativeInput = &Babylon::Plugins::NativeInput::CreateForJavaScript(env);
        });


        Babylon::ScriptLoader loader{*runtime};
        loader.Eval("document = {}", "");
        loader.LoadScript(moduleRootUrl + "/Scripts/ammo.js");
        loader.LoadScript(moduleRootUrl + "/Scripts/recast.js");
        loader.LoadScript(moduleRootUrl + "/Scripts/babylon.max.js");
        loader.LoadScript(moduleRootUrl + "/Scripts/babylonjs.loaders.js");
        loader.LoadScript(moduleRootUrl + "/Scripts/babylonjs.materials.js");
        loader.LoadScript(moduleRootUrl + "/Scripts/babylon.gui.js");

        if (scripts.empty())
        {
            loader.LoadScript(moduleRootUrl + "/Scripts/experience.js");
        }
        else
        {
            for (const auto& script : scripts)
            {
                loader.LoadScript(GetUrlFromPath(script));
            }

            loader.LoadScript(moduleRootUrl + "/Scripts/playground_runner.js");
        }
    }

    void UpdateWindowSize(float width, float height)
    {
        if (device)
        {
            device->UpdateSize(width, height);
        }
    }
}

int main(int _argc, const char* const* _argv)
{
    XInitThreads();
    Display* display = XOpenDisplay(NULL);

    int32_t screen = DefaultScreen(display);
    int32_t depth  = DefaultDepth(display, screen);
    Visual* visual = DefaultVisual(display, screen);
    Window root   = RootWindow(display, screen);
    const int width = 640;
    const int height = 480;

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

    InitBabylon(window, width, height, _argc, _argv);
    UpdateWindowSize(width, height);

    bool exit{};
    while (!exit)
    {
        if (!XPending(display) && device)
        {
            update->Finish();
            device->FinishRenderingCurrentFrame();
            device->StartRenderingCurrentFrame();
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

                        if (nativeInput != nullptr) {
                            switch (xbutton.button) {
                                case Button1:
                                    nativeInput->MouseDown(Babylon::Plugins::NativeInput::LEFT_MOUSE_BUTTON_ID, xmotion.x, xmotion.y);
                                    break;
                                case Button2:
                                    nativeInput->MouseDown(Babylon::Plugins::NativeInput::MIDDLE_MOUSE_BUTTON_ID, xmotion.x, xmotion.y);
                                    break;
                                case Button3:
                                    nativeInput->MouseDown(Babylon::Plugins::NativeInput::RIGHT_MOUSE_BUTTON_ID, xmotion.x, xmotion.y);
                                    break;
                                case Button4:
                                    nativeInput->MouseWheel(Babylon::Plugins::NativeInput::MOUSEWHEEL_Y_ID, -120);
                                    break;
                                case Button5:
                                    nativeInput->MouseWheel(Babylon::Plugins::NativeInput::MOUSEWHEEL_Y_ID, 120);
                                    break;
                            }
                        }
                    }
                    break;
                case ButtonRelease:
                    {
                        const XMotionEvent& xmotion = event.xmotion;
                        const XButtonEvent& xbutton = event.xbutton;

                        if (nativeInput != nullptr) {
                            switch (xbutton.button) {
                                case Button1:
                                    nativeInput->MouseUp(Babylon::Plugins::NativeInput::LEFT_MOUSE_BUTTON_ID, xmotion.x, xmotion.y);
                                    break;
                                case Button2:
                                    nativeInput->MouseUp(Babylon::Plugins::NativeInput::MIDDLE_MOUSE_BUTTON_ID, xmotion.x, xmotion.y);
                                    break;
                                case Button3:
                                    nativeInput->MouseUp(Babylon::Plugins::NativeInput::RIGHT_MOUSE_BUTTON_ID, xmotion.x, xmotion.y);
                                    break;
                            }
                        }
                    }
                    break;
                case MotionNotify:
                    {
                        const XMotionEvent& xmotion = event.xmotion;
                        if (nativeInput != nullptr) {
                            nativeInput->MouseMove(xmotion.x, xmotion.y);
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
    return 0;
}
