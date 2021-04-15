#include <napi/env.h>

namespace Babylon::Plugins::Internal
{
    class ChromeDevTools : public Napi::ObjectWrap<ChromeDevTools>
    {
        class Impl;

    public:
        static void Initialize(Napi::Env env);
        ChromeDevTools(const Napi::CallbackInfo& info);
        static constexpr auto JS_INSTANCE_NAME = "chromeDevTools";
        Napi::Value SupportsInspector(const Napi::CallbackInfo& info);
        void StartInspector(const Napi::CallbackInfo &info);
        void StopInspector(const Napi::CallbackInfo& info);

    private:
        std::unique_ptr<Impl> m_impl;
    };
}