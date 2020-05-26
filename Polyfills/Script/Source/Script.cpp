#include "Script.h"
#include <Babylon/JsRuntime.h>
#include <functional>
#include <sstream>

#include <arcana/threading/task.h>
#include <arcana/threading/task_schedulers.h>


namespace Babylon::Polyfills::Internal
{
    void Script::CreateInstance(Napi::Env env, DispatchFunctionT dispatchFunction)
    {
        Napi::HandleScope scope{env};

        Napi::Function func = ParentT::DefineClass(
            env,
            "Script",
            {
                ParentT::InstanceMethod("loadScript", &Script::LoadScript),
            });

        Napi::Object script = func.New({ Napi::External<Babylon::Polyfills::Script::DispatchFunctionT>::New(env, new Babylon::Polyfills::Script::DispatchFunctionT(std::move(dispatchFunction))) });
        env.Global().Set(JS_INSTANCE_NAME, script);
    }

    Script::Script(const Napi::CallbackInfo& info)
        : ParentT{info}
        , m_dispatchFunction{ *info[0].As<Napi::External<Babylon::Polyfills::Script::DispatchFunctionT>>().Data() }
        , m_loader{ m_dispatchFunction }
        , m_runtimeScheduler{ JsRuntime::GetFromJavaScript(info.Env()) }
    {
    }

    void Script::LoadScript(const Napi::CallbackInfo& info)
    {
        const auto url = info[0].As<Napi::String>();
        const auto onSuccess = info[1].As<Napi::Function>();
        const auto onError = info[2].As<Napi::Function>();
        //const auto scriptId = info[3].As<Napi::String>();

        m_loader.LoadScript(url, new Napi::FunctionReference(Napi::Persistent(onSuccess)), new Napi::FunctionReference(Napi::Persistent(onError)));
        /*
            std::make_shared<std::function<void()>>( [onSuccessRef = Napi::Persistent(onSuccess)]()  {
                onSuccessRef.Call({});
            }));*/
    }
}

namespace Babylon::Polyfills::Script
{
    void Initialize(Napi::Env env, DispatchFunctionT dispatchFunction)
    {
        Internal::Script::CreateInstance(env, dispatchFunction);
    }
}
