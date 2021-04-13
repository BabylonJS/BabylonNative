#include "ChromeDevToolsImpl.h"

#include <Babylon/JsRuntime.h>

namespace Babylon::Plugins::Internal
{
    class ChromeDevTools::Impl
    {
    };

    ChromeDevTools::ChromeDevTools(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<ChromeDevTools>{info}
    {
    }

    Napi::Value ChromeDevTools::SupportsInspector(const Napi::CallbackInfo& info)
    {
        return Napi::Boolean::From(info.Env(), false);
    }

    void ChromeDevTools::StartInspector(const Napi::CallbackInfo& info)
    {
        throw Napi::Error::New(info.Env(), "This method is currently unsupported on this JavaScript engine.");
    }

    void ChromeDevTools::StopInspector(const Napi::CallbackInfo& info)
    {
        throw Napi::Error::New(info.Env(), "This method is currently unsupported on this JavaScript engine.");
    }
}