#include "ChromeDevToolsImpl.h"

namespace Babylon::Plugins::ChromeDevTools
{
    void Initialize(Napi::Env env)
    {
        Babylon::Plugins::Internal::ChromeDevTools::Initialize(env);
    }

    bool SupportsInspector()
    {
        return Babylon::Plugins::Internal::ChromeDevTools::GetInstance()->SupportsInspector();
    }

    void StartInspector(const unsigned short port, const std::string& appName)
    {
        Babylon::Plugins::Internal::ChromeDevTools::GetInstance()->StartInspector(port, appName);
    }

    void StopInspector()
    {
        Babylon::Plugins::Internal::ChromeDevTools::GetInstance()->StopInspector();
    }
}