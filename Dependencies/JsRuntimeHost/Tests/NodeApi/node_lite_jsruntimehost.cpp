#include "node_lite.h"

#include <napi/env.h>
#include <napi/js_native_api.h>

#include <stdexcept>
#include <memory>
#include <utility>

#if defined(__APPLE__)
#include <JavaScriptCore/JavaScript.h>
#include "js_native_api_javascriptcore.h"
#elif defined(__ANDROID__)
#include <v8.h>
#include "js_native_api_v8.h"
#endif

namespace node_api_tests {

namespace {

class JsRuntimeHostEnvHolder : public IEnvHolder {
 public:
  JsRuntimeHostEnvHolder(
      std::shared_ptr<NodeLiteTaskRunner> /*taskRunner*/,
      std::function<void(napi_env, napi_value)> onUnhandledError)
      : onUnhandledError_(std::move(onUnhandledError)) {
#if defined(__APPLE__)
    context_ = JSGlobalContextCreateInGroup(nullptr, nullptr);
    env_ = Napi::Attach(context_);
#elif defined(__ANDROID__)
    // V8's platform is process-global and is already initialized by JsRuntimeHost -- the host
    // AppRuntime that UnitTestsJNI links and that runs (via the regular V8 unit tests) before these
    // in-process Node-API tests. Initializing it a second time aborts V8 with "Wrong initialization
    // order", so reuse the host's platform and only create our own isolate/context below.
    allocator_.reset(v8::ArrayBuffer::Allocator::NewDefaultAllocator());
    v8::Isolate::CreateParams create_params;
    create_params.array_buffer_allocator = allocator_.get();
    isolate_ = v8::Isolate::New(create_params);

    // The host runs its own V8 isolate in this process, so V8 enforces multi-isolate locking.
    // Hold a Locker + Isolate::Scope for this holder's entire lifetime so all subsequent
    // Node-API/V8 access on this thread (running the test script, native callbacks, teardown) is
    // properly locked and scoped to our isolate -- otherwise V8 aborts with "Entering the V8 API
    // without proper locking in place".
    locker_ = std::make_unique<v8::Locker>(isolate_);
    isolate_scope_ = std::make_unique<v8::Isolate::Scope>(isolate_);

    v8::HandleScope handle_scope(isolate_);
    v8::Local<v8::Context> context = v8::Context::New(isolate_);
    context_.Reset(isolate_, context);
    v8::Context::Scope context_scope(context);
    env_ = Napi::Attach(context);
#else
    (void)onUnhandledError_;
    throw std::runtime_error(
        "node_lite is only implemented for Apple platforms in this port.");
#endif
  }

  ~JsRuntimeHostEnvHolder() override {
#if defined(__APPLE__)
    if (env_ != nullptr) {
      Napi::Env napiEnv{env_};

      if (onUnhandledError_) {
        bool hasPending = false;
        if (napi_is_exception_pending(env_, &hasPending) == napi_ok &&
            hasPending) {
          napi_value error{};
          if (napi_get_and_clear_last_exception(env_, &error) == napi_ok) {
            onUnhandledError_(env_, error);
          }
        }
      }

      if (context_ != nullptr) {
        JSGlobalContextRelease(context_);
        context_ = nullptr;
      }

      Napi::Detach(napiEnv);
      env_ = nullptr;
    } else if (context_ != nullptr) {
      JSGlobalContextRelease(context_);
      context_ = nullptr;
    }
#elif defined(__ANDROID__)
    if (env_ != nullptr && isolate_ != nullptr) {
      // Still locked + isolate-scoped on this thread via locker_/isolate_scope_ (held members).
      v8::HandleScope handle_scope(isolate_);
      v8::Local<v8::Context> context = context_.Get(isolate_);
      v8::Context::Scope context_scope(context);

      if (onUnhandledError_) {
        bool hasPending = false;
        if (napi_is_exception_pending(env_, &hasPending) == napi_ok && hasPending) {
          napi_value error{};
          if (napi_get_and_clear_last_exception(env_, &error) == napi_ok) {
            // onUnhandledError_ may invoke the in-process fatal handler, which throws
            // NodeLiteFatalError. A destructor must not let that escape (std::terminate). Real
            // test errors are reported synchronously via ExitOnException; this is a best-effort
            // fallback for anything still pending at teardown.
            try {
              onUnhandledError_(env_, error);
            } catch (...) {
            }
          }
        }
      }

      Napi::Env napiEnv{env_};
      Napi::Detach(napiEnv);
      env_ = nullptr;
    }

    context_.Reset();

    isolate_scope_.reset();
    locker_.reset();

    if (isolate_ != nullptr) {
      isolate_->Dispose();
      isolate_ = nullptr;
    }

    allocator_.reset();
#endif
  }

  napi_env getEnv() override { return env_; }

 private:
#if defined(__APPLE__)
  JSGlobalContextRef context_{};
#elif defined(__ANDROID__)
  v8::Isolate* isolate_{nullptr};
  std::unique_ptr<v8::Locker> locker_{};
  std::unique_ptr<v8::Isolate::Scope> isolate_scope_{};
  v8::Global<v8::Context> context_;
  std::unique_ptr<v8::ArrayBuffer::Allocator> allocator_{};
#endif
  napi_env env_{};
  std::function<void(napi_env, napi_value)> onUnhandledError_{};
};

}  // namespace

std::unique_ptr<IEnvHolder> CreateEnvHolder(
    std::shared_ptr<NodeLiteTaskRunner> taskRunner,
    std::function<void(napi_env, napi_value)> onUnhandledError) {
  return std::make_unique<JsRuntimeHostEnvHolder>(
      std::move(taskRunner), std::move(onUnhandledError));
}

}  // namespace node_api_tests
