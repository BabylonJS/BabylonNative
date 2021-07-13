#include "ChromeDevTools.h"

#include <V8JsiRuntime.h>
#include <Babylon/JsRuntime.h>

namespace Babylon::Plugins
{
    class ChromeDevTools::Impl final : public std::enable_shared_from_this<ChromeDevTools::Impl>
    {
        public:
            explicit Impl(Napi::Env env) 
                : m_env(env)
            {
                JsRuntime::GetFromJavaScript(env).Dispatch([this](Napi::Env env) {
                    m_runtime = JsRuntime::NativeObject::GetFromJavaScript(env).Get("_JSIRuntime").As<Napi::External<facebook::jsi::Runtime>>().Data();
                });
            }

            ~Impl()
            {
            }

            bool SupportsInspector()
            {
                return true;
            }

            void StartInspector(const unsigned short, const std::string&)
            {
                JsRuntime::GetFromJavaScript(m_env).Dispatch([this](Napi::Env) {
                    if (m_runtime != nullptr)
                    {
                        v8runtime::openInspector(*m_runtime);
                    }
                });
            }

            void StopInspector()
            {
            }

        private:
            facebook::jsi::Runtime* m_runtime;
            Napi::Env m_env;
    };

    ChromeDevTools ChromeDevTools::Initialize(Napi::Env env)
    {
        return {std::make_shared<ChromeDevTools::Impl>(env)};
    }

    ChromeDevTools::ChromeDevTools(std::shared_ptr<ChromeDevTools::Impl> impl)
        : m_impl{std::move(impl)}
    {
    }

    bool ChromeDevTools::SupportsInspector() const
    {
        return m_impl->SupportsInspector();
    }

    /*
        Note: V8JSI doesn't currently support setting the port or appName at runtime.
        For now the port is set to 4653 in AppRuntimeJSI.cpp.
    */
    void ChromeDevTools::StartInspector(const unsigned short port, const std::string& appName) const
    {
        m_impl->StartInspector(port, appName);
    }

    /* Note: V8JSI doesn't currently have a method for stopping the inspector at runtime. */
    void ChromeDevTools::StopInspector() const
    {
        m_impl->StopInspector();
    }
}
