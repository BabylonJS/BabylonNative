#include "node_lite.h"

#include <napi/env.h>
#include <napi/js_native_api.h>

#include <stdexcept>
#include <memory>
#include <string>
#include <utility>

#if defined(JSR_NAPI_ENGINE_JAVASCRIPTCORE)
#include <JavaScriptCore/JavaScript.h>
#include "js_native_api_javascriptcore.h"
#elif defined(JSR_NAPI_ENGINE_V8)
#if !defined(__ANDROID__)
#include <libplatform/libplatform.h>
#endif
#include <v8.h>
#include "js_native_api_v8.h"
#elif defined(JSR_NAPI_ENGINE_QUICKJS)
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshorten-64-to-32"
#endif
#include <quickjs.h>
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
#elif defined(JSR_NAPI_ENGINE_CHAKRA)
#define USE_EDGEMODE_JSRT
#include <jsrt.h>
#endif

namespace node_api_tests {

namespace {

#if defined(JSR_NAPI_ENGINE_CHAKRA)
void ThrowIfChakraFailed(JsErrorCode error) {
  if (error != JsNoError) {
    throw std::runtime_error(
        "Chakra call failed with error code " +
        std::to_string(static_cast<int>(error)));
  }
}
#endif

class JsRuntimeHostEnvHolder : public IEnvHolder {
 public:
  JsRuntimeHostEnvHolder(
      std::shared_ptr<NodeLiteTaskRunner> taskRunner,
      std::function<void(napi_env, napi_value)> onUnhandledError,
      const std::string& executablePath)
      : task_runner_(std::move(taskRunner)),
        onUnhandledError_(std::move(onUnhandledError)) {
#if defined(JSR_NAPI_ENGINE_JAVASCRIPTCORE)
    (void)executablePath;
    context_ = JSGlobalContextCreateInGroup(nullptr, nullptr);
    env_ = Napi::Attach(context_);
#elif defined(JSR_NAPI_ENGINE_V8)
#if !defined(__ANDROID__)
    v8::V8::InitializeICUDefaultLocation(executablePath.c_str());
    v8::V8::InitializeExternalStartupData(executablePath.c_str());
    v8::V8::SetFlagsFromString("--expose-gc");
    platform_ = v8::platform::NewDefaultPlatform();
    v8::V8::InitializePlatform(platform_.get());
    if (!v8::V8::Initialize()) {
      v8::V8::DisposePlatform();
      platform_.reset();
      throw std::runtime_error("Unable to initialize V8");
    }
#endif

#if defined(__ANDROID__)
    // Android runs node_lite in-process after AppRuntime initializes V8, so
    // reuse that process-global platform instead of initializing it twice.
#endif
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
#elif defined(JSR_NAPI_ENGINE_QUICKJS)
    (void)executablePath;
    runtime_ = JS_NewRuntime();
    if (runtime_ == nullptr) {
      throw std::runtime_error("Unable to create QuickJS runtime");
    }
    context_ = JS_NewContext(runtime_);
    if (context_ == nullptr) {
      JS_FreeRuntime(runtime_);
      runtime_ = nullptr;
      throw std::runtime_error("Unable to create QuickJS context");
    }
    env_ = Napi::Attach(context_);
#elif defined(JSR_NAPI_ENGINE_CHAKRA)
    (void)executablePath;
    ThrowIfChakraFailed(
        JsCreateRuntime(JsRuntimeAttributeNone, nullptr, &runtime_));
    try {
      ThrowIfChakraFailed(JsCreateContext(runtime_, &context_));
      ThrowIfChakraFailed(JsSetCurrentContext(context_));
      ThrowIfChakraFailed(JsSetPromiseContinuationCallback(
          [](JsValueRef task, void* callbackState) {
            ThrowIfChakraFailed(JsAddRef(task, nullptr));
            static_cast<NodeLiteTaskRunner*>(callbackState)->PostTask(
                [task]() {
                  JsValueRef undefined{};
                  ThrowIfChakraFailed(JsGetUndefinedValue(&undefined));
                  ThrowIfChakraFailed(
                      JsCallFunction(task, &undefined, 1, nullptr));
                  ThrowIfChakraFailed(JsRelease(task, nullptr));
                });
          },
          task_runner_.get()));
      env_ = Napi::Attach();
    } catch (...) {
      JsSetCurrentContext(JS_INVALID_REFERENCE);
      JsDisposeRuntime(runtime_);
      runtime_ = JS_INVALID_RUNTIME_HANDLE;
      throw;
    }
#elif defined(JSR_NAPI_ENGINE_HERMES)
    (void)executablePath;
    env_ = Napi::Attach();
#else
    (void)executablePath;
    (void)onUnhandledError_;
    throw std::runtime_error(
        "node_lite is not implemented for the selected JavaScript engine.");
#endif
  }

  ~JsRuntimeHostEnvHolder() override {
#if defined(JSR_NAPI_ENGINE_JAVASCRIPTCORE)
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
#elif defined(JSR_NAPI_ENGINE_V8)
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
#if !defined(__ANDROID__)
    if (platform_ != nullptr) {
      v8::V8::Dispose();
      v8::V8::DisposePlatform();
      platform_.reset();
    }
#endif
#elif defined(JSR_NAPI_ENGINE_QUICKJS)
    if (env_ != nullptr) {
      if (onUnhandledError_) {
        bool hasPending = false;
        if (napi_is_exception_pending(env_, &hasPending) == napi_ok &&
            hasPending) {
          napi_value error{};
          if (napi_get_and_clear_last_exception(env_, &error) == napi_ok) {
            try {
              onUnhandledError_(env_, error);
            } catch (...) {
            }
          }
        }
      }
      Napi::Detach(Napi::Env{env_});
      env_ = nullptr;
    }
    if (context_ != nullptr) {
      JS_FreeContext(context_);
      context_ = nullptr;
    }
    if (runtime_ != nullptr) {
      JS_FreeRuntime(runtime_);
      runtime_ = nullptr;
    }
#elif defined(JSR_NAPI_ENGINE_CHAKRA)
    if (env_ != nullptr && onUnhandledError_) {
      bool hasPending = false;
      if (napi_is_exception_pending(env_, &hasPending) == napi_ok &&
          hasPending) {
        napi_value error{};
        if (napi_get_and_clear_last_exception(env_, &error) == napi_ok) {
          try {
            onUnhandledError_(env_, error);
          } catch (...) {
          }
        }
      }
    }
    JsSetCurrentContext(JS_INVALID_REFERENCE);
    if (runtime_ != JS_INVALID_RUNTIME_HANDLE) {
      JsDisposeRuntime(runtime_);
      runtime_ = JS_INVALID_RUNTIME_HANDLE;
    }
    if (env_ != nullptr) {
      Napi::Detach(Napi::Env{env_});
      env_ = nullptr;
    }
#elif defined(JSR_NAPI_ENGINE_HERMES)
    if (env_ != nullptr) {
      if (onUnhandledError_) {
        bool hasPending = false;
        if (napi_is_exception_pending(env_, &hasPending) == napi_ok &&
            hasPending) {
          napi_value error{};
          if (napi_get_and_clear_last_exception(env_, &error) == napi_ok) {
            try {
              onUnhandledError_(env_, error);
            } catch (...) {
            }
          }
        }
      }
      Napi::Detach(Napi::Env{env_});
      env_ = nullptr;
    }
#endif
  }

  napi_env getEnv() override { return env_; }

 private:
#if defined(JSR_NAPI_ENGINE_JAVASCRIPTCORE)
  JSGlobalContextRef context_{};
#elif defined(JSR_NAPI_ENGINE_V8)
  v8::Isolate* isolate_{nullptr};
  std::unique_ptr<v8::Locker> locker_{};
  std::unique_ptr<v8::Isolate::Scope> isolate_scope_{};
  v8::Global<v8::Context> context_;
  std::unique_ptr<v8::ArrayBuffer::Allocator> allocator_{};
#if !defined(__ANDROID__)
  std::unique_ptr<v8::Platform> platform_{};
#endif
#elif defined(JSR_NAPI_ENGINE_QUICKJS)
  JSRuntime* runtime_{};
  JSContext* context_{};
#elif defined(JSR_NAPI_ENGINE_CHAKRA)
  JsRuntimeHandle runtime_{JS_INVALID_RUNTIME_HANDLE};
  JsContextRef context_{JS_INVALID_REFERENCE};
#endif
  std::shared_ptr<NodeLiteTaskRunner> task_runner_{};
  napi_env env_{};
  std::function<void(napi_env, napi_value)> onUnhandledError_{};
};

}  // namespace

std::unique_ptr<IEnvHolder> CreateEnvHolder(
    std::shared_ptr<NodeLiteTaskRunner> taskRunner,
    std::function<void(napi_env, napi_value)> onUnhandledError,
    std::string executablePath) {
  return std::make_unique<JsRuntimeHostEnvHolder>(
      std::move(taskRunner),
      std::move(onUnhandledError),
      executablePath);
}

}  // namespace node_api_tests
