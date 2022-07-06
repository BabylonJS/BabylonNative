#pragma once

#include <napi/env.h>

namespace Babylon::Plugins
{
    class ChromeDevTools
    {
    public:
        class Impl;

        ChromeDevTools(const ChromeDevTools&) = default;
        ChromeDevTools& operator=(const ChromeDevTools&) = default;

        ChromeDevTools(ChromeDevTools&&) noexcept = default;
        ChromeDevTools& operator=(ChromeDevTools&&) noexcept = default;

        bool SupportsInspector() const;
        void StartInspector(const unsigned short port, const std::string& appName) const;
        void StopInspector() const;

        static ChromeDevTools Initialize(Napi::Env env);

    private:
        ChromeDevTools(std::shared_ptr<Impl> impl);
        std::shared_ptr<Impl> m_impl;
    };
}
