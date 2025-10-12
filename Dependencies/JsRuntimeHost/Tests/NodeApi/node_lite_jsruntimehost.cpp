#include "node_lite.h"

#include <napi/env.h>
#include <napi/js_native_api.h>

#include <stdexcept>
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
    // TODO: Implement a dedicated V8 environment for Android Node-API tests.
    // For now we surface a clear failure so we remember to provide a proper
    // implementation before enabling Android execution.
    (void)onUnhandledError_;
    throw std::runtime_error(
        "Android Node-API tests are not yet implemented for node_lite.");
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

      Napi::Detach(napiEnv);
      env_ = nullptr;
    }

    if (context_ != nullptr) {
      JSGlobalContextRelease(context_);
      context_ = nullptr;
    }
#endif
  }

  napi_env getEnv() override { return env_; }

 private:
#if defined(__APPLE__)
  JSGlobalContextRef context_{};
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
