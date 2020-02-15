#include "AppRuntimeImpl.h"

namespace Babylon
{
    AppRuntime::Impl::Impl(AppRuntime& runtime, const char* rootUrl)
        : Runtime{runtime}
        , RootUrl{rootUrl}
        , TaskChain{std::make_unique<Babylon::TaskChain>([this] { CreateEnvironmentAndRun(); })}
    {
    }

    void AppRuntime::Impl::Run(Napi::Env env)
    {
        Runtime.AddJavaScriptReference(env, false);

        TaskChain->RunTaskChain(env);
        TaskChain.reset();
    }
}
