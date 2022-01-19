#include <Babylon/Plugins/NativeTracing.h>
#include <Babylon/JsRuntime.h>
#include <napi/napi_pointer.h>
#include <arcana/tracing/trace_region.h>
#include <optional>

namespace
{
    Napi::Value StartPerformanceCounter(const Napi::CallbackInfo& info)
    {
        const std::string name{info[0].As<Napi::String>().Utf8Value()};
        auto* traceRegion = new std::optional<arcana::trace_region>(name.c_str());
        return Napi::Pointer<std::optional<arcana::trace_region>>::Create(info.Env(), traceRegion, Napi::NapiPointerDeleter(traceRegion));
    }

    void EndPerformanceCounter(const Napi::CallbackInfo& info)
    {
        info[0].As<Napi::Pointer<std::optional<arcana::trace_region>>>().Get()->reset();
    }

    void EnablePerformanceTracing(const Napi::CallbackInfo&)
    {
        arcana::trace_region::enable();
    }

    void DisablePerformanceTracing(const Napi::CallbackInfo&)
    {
        arcana::trace_region::disable();
    }
}

namespace Babylon::Plugins::NativeTracing
{
    void Initialize(Napi::Env env)
    {
        auto nativeObject{JsRuntime::NativeObject::GetFromJavaScript(env)};
        nativeObject.Set("startPerformanceCounter", Napi::Function::New(env, StartPerformanceCounter, "startPerformanceCounter"));
        nativeObject.Set("endPerformanceCounter", Napi::Function::New(env, EndPerformanceCounter, "endPerformanceCounter"));
        nativeObject.Set("enablePerformanceLogging", Napi::Function::New(env, EnablePerformanceTracing, "enablePerformanceLogging"));
        nativeObject.Set("disablePerformanceLogging", Napi::Function::New(env, DisablePerformanceTracing, "disablePerformanceLogging"));
    }
}
