#include "ChromeDevToolsImpl.h"

#include <Babylon/JsRuntime.h>

namespace Babylon::Plugins::Internal
{
    class ChromeDevTools::Impl
    {
    };

    std::unique_ptr<ChromeDevTools> ChromeDevTools::s_instance = nullptr;

    void ChromeDevTools::Initialize(Napi::Env env)
    {
        s_instance = std::make_unique<ChromeDevTools>(env);
    }

    ChromeDevTools* ChromeDevTools::GetInstance()
    {
        return s_instance.get();
    }

    ChromeDevTools::ChromeDevTools(Napi::Env)
    {
    }

    bool ChromeDevTools::SupportsInspector()
    {
        return false;
    }

    void ChromeDevTools::StartInspector(const unsigned short, const std::string&)
    {
        throw std::runtime_error{"This method is currently unsupported on this JavaScript engine."};
    }

    void ChromeDevTools::StopInspector()
    {
        throw std::runtime_error{"This method is currently unsupported on this JavaScript engine."};
    }
}