#include "ChromeDevTools.h"

#include <Babylon/JsRuntime.h>

namespace Babylon::Plugins
{
    class ChromeDevTools::Impl
    {
    };

    ChromeDevTools ChromeDevTools::Initialize(Napi::Env)
    {
        return {nullptr};
    }

    ChromeDevTools::ChromeDevTools(std::shared_ptr<ChromeDevTools::Impl> impl)
        : m_impl{std::move(impl)}
    {
    }

    bool ChromeDevTools::SupportsInspector() const
    {
        return false;
    }

    void ChromeDevTools::StartInspector(const unsigned short, const std::string&) const
    {
        throw std::runtime_error{"This method is currently unsupported on this JavaScript engine."};
    }

    void ChromeDevTools::StopInspector() const
    {
        throw std::runtime_error{"This method is currently unsupported on this JavaScript engine."};
    }
}