#include <Babylon/Plugins/NativeTracing.h>
#include <Babylon/JsRuntime.h>
#include <Babylon/PerfTrace.h>

namespace
{
    Napi::Value StartPerformanceCounter(const Napi::CallbackInfo& info)
    {
        const std::string name{info[0].As<Napi::String>().Utf8Value()};
        return Babylon::PerfTrace::Handle::ToNapi(info.Env(), Babylon::PerfTrace::Trace(name.c_str()));
    }

    void EndPerformanceCounter(const Napi::CallbackInfo& info)
    {
        Babylon::PerfTrace::Handle::FromNapi(info[0]);
    }

    void EnablePerformanceTracing(const Napi::CallbackInfo& info)
    {
        const auto level = (info.Length() > 0 && info[0].As<Napi::Number>().Int32Value()) ? Babylon::PerfTrace::Level::Log : Babylon::PerfTrace::Level::Mark;
        Babylon::PerfTrace::SetLevel(level);
    }

    void DisablePerformanceTracing(const Napi::CallbackInfo&)
    {
        Babylon::PerfTrace::SetLevel(Babylon::PerfTrace::Level::None);
    }
}

namespace Babylon::Plugins::NativeTracing
{
    void BABYLON_API Initialize(Napi::Env env)
    {
        auto nativeObject{JsRuntime::NativeObject::GetFromJavaScript(env)};
        nativeObject.Set("startPerformanceCounter", Napi::Function::New(env, StartPerformanceCounter, "startPerformanceCounter"));
        nativeObject.Set("endPerformanceCounter", Napi::Function::New(env, EndPerformanceCounter, "endPerformanceCounter"));
        nativeObject.Set("enablePerformanceLogging", Napi::Function::New(env, EnablePerformanceTracing, "enablePerformanceLogging"));
        nativeObject.Set("disablePerformanceLogging", Napi::Function::New(env, DisablePerformanceTracing, "disablePerformanceLogging"));
    }
}
