#include <napi/env.h>
#include <napi/js_native_api_types.h>
#include "js_native_api_v8.h"

#include <utility>

namespace
{
  // Runs finalizers queued by garbage collection on a posted task, as Node.js's node_napi_env__
  // does with SetImmediate.
  struct scheduled_napi_env__ final : napi_env__
  {
    scheduled_napi_env__(v8::Local<v8::Context> context, std::function<void(std::function<void()>)> postTask)
      : napi_env__(context, NAPI_VERSION)
      , post_task{std::move(postTask)}
    {
    }

    void EnqueueFinalizer(v8impl::RefTracker* finalizer) override
    {
      napi_env__::EnqueueFinalizer(finalizer);
      if (!finalization_scheduled && !destructing)
      {
        finalization_scheduled = true;
        post_task([this]() {
          finalization_scheduled = false;
          DrainFinalizerQueue();
        });
      }
    }

    void DeleteMe() override
    {
      destructing = true;
      DrainFinalizerQueue();
      napi_env__::DeleteMe();
    }

  private:
    void DrainFinalizerQueue()
    {
      // A finalizer can queue or delete other finalizers, so take them one at a time.
      while (!pending_finalizers.empty())
      {
        v8impl::RefTracker* finalizer = *pending_finalizers.begin();
        pending_finalizers.erase(finalizer);
        finalizer->Finalize();
      }
    }

    std::function<void(std::function<void()>)> post_task;
    bool finalization_scheduled = false;
    bool destructing = false;
  };
}

namespace Napi
{
  Env Attach(v8::Local<v8::Context> context, std::function<void(std::function<void()>)> postTask)
  {
    if (!postTask)
    {
      // second argument is module version
      return {new napi_env__(context, NAPI_VERSION)};
    }

    return {new scheduled_napi_env__(context, std::move(postTask))};
  }

  void Detach(Env env)
  {
    napi_env env_ptr{env};
    env_ptr->DeleteMe();
  }

  v8::Local<v8::Context> GetContext(Env env)
  {
    napi_env env_ptr{env};
    return env_ptr->context();
  }
}
