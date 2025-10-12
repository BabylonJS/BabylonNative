#include "js_runtime_api.h"

#include <napi/js_native_api.h>
#include <napi/js_native_api_types.h>

#if defined(__APPLE__)
#include <JavaScriptCore/JavaScript.h>
#include "js_native_api_javascriptcore.h"
#elif defined(__ANDROID__)
#include <v8.h>
#include "js_native_api_v8.h"
#endif

struct jsr_napi_env_scope_s {
  napi_env env{nullptr};
};

napi_status jsr_open_napi_env_scope(napi_env env,
                                    jsr_napi_env_scope* scope) {
  if (scope == nullptr) {
    return napi_invalid_arg;
  }

  auto* scope_impl = new jsr_napi_env_scope_s{};
  scope_impl->env = env;
  *scope = scope_impl;
  return napi_ok;
}

napi_status jsr_close_napi_env_scope(napi_env /*env*/,
                                     jsr_napi_env_scope scope) {
  if (scope == nullptr) {
    return napi_invalid_arg;
  }

  delete scope;
  return napi_ok;
}

napi_status jsr_run_script(napi_env env,
                           napi_value source,
                           const char* source_url,
                           napi_value* result) {
  return napi_run_script(env, source, source_url, result);
}

napi_status jsr_collect_garbage(napi_env env) {
#if defined(__APPLE__)
  if (env == nullptr) {
    return napi_invalid_arg;
  }

  JSGlobalContextRef context = env->context;
  if (context == nullptr) {
    return napi_invalid_arg;
  }

  JSGarbageCollect(context);
  return napi_ok;
#elif defined(__ANDROID__)
  if (env == nullptr) {
    return napi_invalid_arg;
  }

  v8::Isolate* isolate = env->isolate;
  if (isolate == nullptr) {
    return napi_invalid_arg;
  }

  isolate->RequestGarbageCollectionForTesting(
      v8::Isolate::kFullGarbageCollection);
  return napi_ok;
#else
  (void)env;
  return napi_generic_failure;
#endif
}

napi_status jsr_initialize_native_module(
    napi_env env,
    napi_addon_register_func register_module,
    int32_t /*api_version*/,
    napi_value* exports) {
  if (env == nullptr || register_module == nullptr || exports == nullptr) {
    return napi_invalid_arg;
  }

  napi_value module_exports{};
  napi_status status = napi_create_object(env, &module_exports);
  if (status != napi_ok) {
    return status;
  }

  napi_value returned_exports = register_module(env, module_exports);

  bool has_exception = false;
  status = napi_is_exception_pending(env, &has_exception);
  if (status != napi_ok) {
    return status;
  }

  if (has_exception) {
    return napi_pending_exception;
  }

  if (returned_exports != nullptr && returned_exports != module_exports) {
    module_exports = returned_exports;
  }

  *exports = module_exports;
  return napi_ok;
}
