#include "TestUtils.h"
#define XK_MISCELLANY
#define XK_LATIN1
#include <X11/keysymdef.h>
#include <X11/Xlib.h> // will include X11 which #defines None... Don't mess with order of includes.
#include <X11/Xutil.h>
#include <unistd.h> // syscall
#undef None
#include <filesystem>

namespace Babylon::Plugins::Internal
{
    void TestUtils::Exit(const Napi::CallbackInfo& info)
    {
        auto exitCode = info[0].As<Napi::Number>().Int32Value();
        if (exitCode != 0)
        {
            std::quick_exit(exitCode);
        }

        auto window = (Window)m_window;
        auto display = XOpenDisplay(NULL);
        XClientMessageEvent dummyEvent;
        memset(&dummyEvent, 0, sizeof(XClientMessageEvent));
        dummyEvent.type = ClientMessage;
        dummyEvent.window = window;
        dummyEvent.format = 32;
        dummyEvent.data.l[0] = XInternAtom(display, "WM_DELETE_WINDOW", False);
        XSendEvent(display, window, 0, 0, (XEvent*)&dummyEvent);
        XFlush(display);
        XCloseDisplay(display);
    }

    void TestUtils::UpdateSize(const Napi::CallbackInfo& /*info*/)
    {
    }

    void TestUtils::SetTitle(const Napi::CallbackInfo& info)
    {
        const auto title = info[0].As<Napi::String>().Utf8Value();
        auto display = XOpenDisplay(NULL);
        auto window = (Window)m_window;
        XStoreName(display, window, title.c_str());
        XCloseDisplay(display);
    }

    Napi::Value TestUtils::GetOutputDirectory(const Napi::CallbackInfo& info)
    {
        char exe[1024];
        int ret = readlink("/proc/self/exe", exe, sizeof(exe) - 1);
        if (ret == -1)
        {
            throw Napi::Error::New(info.Env(), "Unable to get executable location");
        }
        exe[ret] = 0;

        auto path = std::filesystem::path{exe}.parent_path().generic_string();
        return Napi::Value::From(info.Env(), path);
    }
}