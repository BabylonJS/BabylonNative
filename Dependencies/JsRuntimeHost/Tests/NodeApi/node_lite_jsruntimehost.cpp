#include "node_lite.h"

#include <napi/env.h>
#include <napi/js_native_api.h>

#include <stdexcept>
#include <memory>
#include <mutex>
#include <utility>

#if defined(__APPLE__)
#include <JavaScriptCore/JavaScript.h>
#include "js_native_api_javascriptcore.h"
#elif defined(__ANDROID__)
#include <v8.h>
#include "js_native_api_v8.h"
#include <libplatform/libplatform.h>
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
    V8Platform::EnsureInitialized();

    allocator_.reset(v8::ArrayBuffer::Allocator::NewDefaultAllocator());
    v8::Isolate::CreateParams create_params;
    create_params.array_buffer_allocator = allocator_.get();
    isolate_ = v8::Isolate::New(create_params);

    v8::Locker locker(isolate_);
    v8::Isolate::Scope isolate_scope(isolate_);
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
      v8::Locker locker(isolate_);
      v8::Isolate::Scope isolate_scope(isolate_);
      v8::HandleScope handle_scope(isolate_);
      v8::Local<v8::Context> context = context_.Get(isolate_);
      v8::Context::Scope context_scope(context);

      if (onUnhandledError_) {
        bool hasPending = false;
        if (napi_is_exception_pending(env_, &hasPending) == napi_ok && hasPending) {
          napi_value error{};
          if (napi_get_and_clear_last_exception(env_, &error) == napi_ok) {
            onUnhandledError_(env_, error);
          }
        }
      }

      Napi::Env napiEnv{env_};
      Napi::Detach(napiEnv);
      env_ = nullptr;
    }

    context_.Reset();

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
  class V8Platform {
   public:
    static void EnsureInitialized() {
      std::call_once(init_flag_, []() {
        platform_ = v8::platform::NewDefaultPlatform();
        v8::V8::InitializePlatform(platform_.get());
        v8::V8::Initialize();
      });
    }

   private:
    static std::once_flag init_flag_;
    static std::unique_ptr<v8::Platform> platform_;
  };

  v8::Isolate* isolate_{nullptr};
  v8::Global<v8::Context> context_;
  std::unique_ptr<v8::ArrayBuffer::Allocator> allocator_{};
#endif
  napi_env env_{};
  std::function<void(napi_env, napi_value)> onUnhandledError_{};
};

#if defined(__ANDROID__)
std::once_flag JsRuntimeHostEnvHolder::V8Platform::init_flag_{};
std::unique_ptr<v8::Platform> JsRuntimeHostEnvHolder::V8Platform::platform_{};
#endif

}  // namespace

std::unique_ptr<IEnvHolder> CreateEnvHolder(
    std::shared_ptr<NodeLiteTaskRunner> taskRunner,
    std::function<void(napi_env, napi_value)> onUnhandledError) {
  return std::make_unique<JsRuntimeHostEnvHolder>(
      std::move(taskRunner), std::move(onUnhandledError));
}

}  // namespace node_api_tests
