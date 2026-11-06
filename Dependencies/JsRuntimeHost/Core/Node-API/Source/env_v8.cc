#include <napi/env.h>
#include <napi/js_native_api_types.h>
#include "js_native_api_v8.h"

#include <chrono>

namespace Napi
{
  namespace
  {
    constexpr auto FinalizerDrainBudget{std::chrono::milliseconds{8}};
  }

  Env Attach(v8::Local<v8::Context> isolate)
  {
    // second argument is module version
    return {new napi_env__(isolate, NAPI_VERSION)};
  }

  void Detach(Env env)
  {
    napi_env env_ptr{env};
    env_ptr->DeleteMe();
  }

  void DrainFinalizers(Env env)
  {
    napi_env env_ptr{env};
    const auto start = std::chrono::steady_clock::now();

    // Finalizers can mutate this queue. Bound each dispatcher turn so a large
    // batch cannot monopolize the runtime thread.
    while (!env_ptr->pending_finalizers.empty())
    {
      auto* finalizer = *env_ptr->pending_finalizers.begin();
      env_ptr->pending_finalizers.erase(finalizer);
      finalizer->Finalize();

      if (std::chrono::steady_clock::now() - start >= FinalizerDrainBudget)
      {
        break;
      }
    }
  }

  v8::Local<v8::Context> GetContext(Env env)
  {
    napi_env env_ptr{env};
    return env_ptr->context();
  }
}
