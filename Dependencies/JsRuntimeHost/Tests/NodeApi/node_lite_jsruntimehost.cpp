#include "node_lite.h"

#include <napi/env.h>
#include <napi/js_native_api.h>

#include <algorithm>
#include <stdexcept>
#include <memory>
#include <string>
#include <utility>
#include <vector>

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

    v8_rejection_holder_ = this;
    isolate_->SetPromiseRejectCallback(&OnV8PromiseReject);
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
    JS_SetHostPromiseRejectionTracker(
        runtime_, &OnQuickJsPromiseRejection, this);
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
    InstallHermesRejectionTracker();
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

    if (isolate_ != nullptr) {
      isolate_->SetPromiseRejectCallback(nullptr);
    }
    v8_rejections_.clear();
    if (v8_rejection_holder_ == this) {
      v8_rejection_holder_ = nullptr;
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
    if (runtime_ != nullptr) {
      JS_SetHostPromiseRejectionTracker(runtime_, nullptr, nullptr);
    }
    FreeQuickJsRejections();
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
      if (hermes_take_rejections_ != nullptr) {
        napi_delete_reference(env_, hermes_take_rejections_);
        hermes_take_rejections_ = nullptr;
      }
      Napi::Detach(Napi::Env{env_});
      env_ = nullptr;
    }
#endif
  }

  napi_env getEnv() override { return env_; }

  bool TakeUnhandledRejection(napi_value* reason) override {
#if defined(JSR_NAPI_ENGINE_V8)
    if (v8_rejections_.empty()) {
      return false;
    }
    *reason = v8impl::JsValueFromV8LocalValue(
        v8_rejections_.front().reason.Get(isolate_));
    v8_rejections_.clear();
    return true;
#elif defined(JSR_NAPI_ENGINE_QUICKJS)
    if (quickjs_rejections_.empty()) {
      return false;
    }
    // There is no public JSValue-to-napi_value conversion, so hand the reason over through a
    // temporary global property.
    constexpr const char* kReasonProperty = "__nodeLiteUnhandledRejection";
    JSValue global = JS_GetGlobalObject(context_);
    JS_SetPropertyStr(context_, global, kReasonProperty,
                      JS_DupValue(context_, quickjs_rejections_.front().reason));
    JS_FreeValue(context_, global);
    FreeQuickJsRejections();
    napi_value napiGlobal = NodeApi::GetGlobal(env_);
    *reason = NodeApi::GetProperty(env_, napiGlobal, kReasonProperty);
    NodeApi::DeleteProperty(env_, napiGlobal, kReasonProperty);
    return true;
#elif defined(JSR_NAPI_ENGINE_HERMES)
    napi_value rejections = NodeApi::CallFunction(
        env_, NodeApi::GetReferenceValue(env_, hermes_take_rejections_), {});
    napi_env env = env_;
    uint32_t count{};
    NODE_LITE_CALL(napi_get_array_length(env, rejections, &count));
    if (count == 0) {
      return false;
    }
    NODE_LITE_CALL(napi_get_element(env, rejections, 0, reason));
    return true;
#else
    // The in-box Chakra (chakra.dll) has no promise rejection tracking API, and JavaScriptCore
    // only has private SPI for it, so unhandled rejections are not detected on these engines.
    (void)reason;
    return false;
#endif
  }

 private:
#if defined(JSR_NAPI_ENGINE_V8)
  static void OnV8PromiseReject(v8::PromiseRejectMessage message) {
    JsRuntimeHostEnvHolder* holder = v8_rejection_holder_;
    if (holder == nullptr) {
      return;
    }
    v8::Local<v8::Promise> promise = message.GetPromise();
    switch (message.GetEvent()) {
      case v8::kPromiseRejectWithNoHandler:
        holder->v8_rejections_.push_back(
            V8Rejection{v8::Global<v8::Promise>(holder->isolate_, promise),
                        v8::Global<v8::Value>(holder->isolate_,
                                              message.GetValue())});
        break;
      case v8::kPromiseHandlerAddedAfterReject:
        holder->v8_rejections_.erase(
            std::remove_if(holder->v8_rejections_.begin(),
                           holder->v8_rejections_.end(),
                           [&promise](const V8Rejection& rejection) {
                             return rejection.promise == promise;
                           }),
            holder->v8_rejections_.end());
        break;
      default:
        break;
    }
  }
#elif defined(JSR_NAPI_ENGINE_QUICKJS)
  static void OnQuickJsPromiseRejection(JSContext* ctx,
                                        JSValueConst promise,
                                        JSValueConst reason,
                                        bool is_handled,
                                        void* opaque) {
    auto& rejections =
        static_cast<JsRuntimeHostEnvHolder*>(opaque)->quickjs_rejections_;
    if (!is_handled) {
      rejections.push_back(
          QuickJsRejection{JS_DupValue(ctx, promise), JS_DupValue(ctx, reason)});
      return;
    }
    auto it = std::find_if(
        rejections.begin(), rejections.end(),
        [&promise](const QuickJsRejection& rejection) {
          return JS_VALUE_GET_PTR(rejection.promise) == JS_VALUE_GET_PTR(promise);
        });
    if (it != rejections.end()) {
      JS_FreeValue(ctx, it->promise);
      JS_FreeValue(ctx, it->reason);
      rejections.erase(it);
    }
  }

  void FreeQuickJsRejections() {
    for (QuickJsRejection& rejection : quickjs_rejections_) {
      JS_FreeValueRT(runtime_, rejection.promise);
      JS_FreeValueRT(runtime_, rejection.reason);
    }
    quickjs_rejections_.clear();
  }
#elif defined(JSR_NAPI_ENGINE_HERMES)
  void InstallHermesRejectionTracker() {
    // The tracker reports a rejection as unhandled from a setTimeout callback, and reports it as
    // handled again if a handler is attached later. allRejections is needed because by default
    // it only tracks ReferenceError and TypeError.
    constexpr const char* kInstallTracker = R"(
(function () {
  const pending = new Map();
  HermesInternal.enablePromiseRejectionTracker({
    allRejections: true,
    onUnhandled: (id, error) => { pending.set(id, error); },
    onHandled: (id) => { pending.delete(id); },
  });
  return () => {
    const reasons = Array.from(pending.values());
    pending.clear();
    return reasons;
  };
})())";
    NodeApiEnvScope env_scope{env_};
    NodeApiHandleScope handle_scope{env_};
    napi_env env = env_;
    // Pass a source URL so this uses jsr_run_script: Hermes exports the 3-argument
    // napi_run_script, not the 4-argument one the shared header declares.
    napi_value take = NodeApi::RunScript(env, kInstallTracker,
                                         "node_lite_rejection_tracker.js");
    NODE_LITE_CALL(napi_create_reference(env, take, 1, &hermes_take_rejections_));
  }
#endif

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
  struct V8Rejection {
    v8::Global<v8::Promise> promise;
    v8::Global<v8::Value> reason;
  };
  std::vector<V8Rejection> v8_rejections_{};
  // SetPromiseRejectCallback takes no user data, so the callback finds its holder through this.
  static inline thread_local JsRuntimeHostEnvHolder* v8_rejection_holder_{};
#elif defined(JSR_NAPI_ENGINE_QUICKJS)
  JSRuntime* runtime_{};
  JSContext* context_{};
  struct QuickJsRejection {
    JSValue promise;
    JSValue reason;
  };
  std::vector<QuickJsRejection> quickjs_rejections_{};
#elif defined(JSR_NAPI_ENGINE_CHAKRA)
  JsRuntimeHandle runtime_{JS_INVALID_RUNTIME_HANDLE};
  JsContextRef context_{JS_INVALID_REFERENCE};
#elif defined(JSR_NAPI_ENGINE_HERMES)
  napi_ref hermes_take_rejections_{};
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
