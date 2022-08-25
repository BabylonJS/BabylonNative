#include "AppRuntime.h"
#include <exception>
#include <sstream>

namespace Babylon
{
    void AppRuntime::RunEnvironmentTier(const char*)
    {
        Napi::Env env = Napi::Attach();
        Run(env);
        Napi::Detach(env);
    }
}