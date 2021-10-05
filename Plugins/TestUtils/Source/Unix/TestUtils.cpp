#include "TestUtils.h"


namespace Babylon::Plugins::TestUtils
{
    int errorCode{};
}

namespace Babylon::Plugins::Internal
{
    void TestUtils::Exit(const Napi::CallbackInfo& info)
    {
        const int32_t exitCode = info[0].As<Napi::Number>().Int32Value();
        Plugins::TestUtils::errorCode = exitCode;
        Display* display = XOpenDisplay(NULL);
        XClientMessageEvent dummyEvent;
        memset(&dummyEvent, 0, sizeof(XClientMessageEvent));
        dummyEvent.type = ClientMessage;
        dummyEvent.window = (Window)_nativeWindowPtr;
        dummyEvent.format = 32;
        XSendEvent(display, (Window)_nativeWindowPtr, 0, 0, (XEvent*)&dummyEvent);
        XFlush(display);
    }

    void TestUtils::UpdateSize(const Napi::CallbackInfo& /*info*/)
    {
    }

    void TestUtils::SetTitle(const Napi::CallbackInfo & info)
    {
        const auto title = info[0].As<Napi::String>().Utf8Value();
        Display* display = XOpenDisplay(NULL);
        XStoreName(display, (Window)_nativeWindowPtr, title.c_str());
    }

    Napi::Value TestUtils::GetOutputDirectory(const Napi::CallbackInfo& info)
    {
    }
}