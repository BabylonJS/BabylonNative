#include <napi/env.h>

namespace Babylon::Plugins::Internal
{
    class ChromeDevTools
    {
        class Impl;

    public:
        static void Initialize(Napi::Env env);
        static ChromeDevTools* GetInstance();
        ChromeDevTools(Napi::Env env);
        bool SupportsInspector();
        void StartInspector(const unsigned short port, const std::string& appName);
        void StopInspector();

    private:
        static std::unique_ptr<ChromeDevTools> s_instance;
        std::unique_ptr<Impl> m_impl;
    };
}