#include "js_runtime_api.h"

#include <napi/js_native_api.h>
#include <napi/js_native_api_types.h>

#include <exception>
#include <string>

#if defined(JSR_NAPI_ENGINE_JAVASCRIPTCORE)
#include <JavaScriptCore/JavaScript.h>
#include "js_native_api_javascriptcore.h"
#elif defined(JSR_NAPI_ENGINE_V8)
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
#include "js_native_api_quickjs.h"
#elif defined(JSR_NAPI_ENGINE_CHAKRA)
#define USE_EDGEMODE_JSRT
#include <jsrt.h>

STDAPI_(JsErrorCode) JsCollectGarbage(JsRuntimeHandle runtime);
STDAPI_(JsErrorCode)
JsGetRuntime(JsContextRef context, JsRuntimeHandle* runtime);
#elif defined(JSR_NAPI_ENGINE_HERMES)
#include <napi/env.h>
#endif

struct jsr_napi_env_scope_s {
  napi_env env{nullptr};
#if defined(JSR_NAPI_ENGINE_V8)
  v8::Global<v8::Context> context;
#endif
};

napi_status jsr_open_napi_env_scope(napi_env env,
                                    jsr_napi_env_scope* scope) {
  if (scope == nullptr) {
    return napi_invalid_arg;
  }

  auto* scope_impl = new jsr_napi_env_scope_s{};
  scope_impl->env = env;
#if defined(JSR_NAPI_ENGINE_V8)
  // node_lite calls Node-API outside any napi callback, so V8 has no *current context*. The env
  // holder already holds a Locker + Isolate::Scope; enter the env's context here (exited in
  // jsr_close_napi_env_scope) so calls such as napi_create_object -> v8::Object::New(isolate),
  // which use the isolate's current context, don't segfault. (On JSC this scope is a no-op -- the
  // env carries its context explicitly.)
  if (env != nullptr) {
    v8::Isolate* isolate = env->isolate;
    v8::HandleScope handle_scope(isolate);
    v8::Local<v8::Context> context = env->context();
    scope_impl->context.Reset(isolate, context);
    context->Enter();
  }
#endif
  *scope = scope_impl;
  return napi_ok;
}

napi_status jsr_close_napi_env_scope(napi_env /*env*/,
                                     jsr_napi_env_scope scope) {
  if (scope == nullptr) {
    return napi_invalid_arg;
  }

#if defined(JSR_NAPI_ENGINE_V8)
  if (scope->env != nullptr) {
    v8::Isolate* isolate = scope->env->isolate;
    v8::HandleScope handle_scope(isolate);
    v8::Local<v8::Context> context = scope->context.Get(isolate);
    context->Exit();
    scope->context.Reset();
  }
#endif
  delete scope;
  return napi_ok;
}

napi_status jsr_run_script(napi_env env,
                           napi_value source,
                           const char* source_url,
                           napi_value* result) {
#if defined(JSR_NAPI_ENGINE_HERMES)
  if (env == nullptr || source == nullptr || result == nullptr) {
    return napi_invalid_arg;
  }

  size_t length{};
  napi_status status =
      napi_get_value_string_utf8(env, source, nullptr, 0, &length);
  if (status != napi_ok) {
    return status;
  }

  std::string script(length + 1, '\0');
  size_t written{};
  status = napi_get_value_string_utf8(
      env, source, script.data(), script.size(), &written);
  if (status != napi_ok) {
    return status;
  }
  script.resize(written);

  try {
    *result = Napi::Eval(
        Napi::Env{env}, script.c_str(), source_url == nullptr ? "" : source_url);
    return napi_ok;
  } catch (const Napi::Error& error) {
    error.ThrowAsJavaScriptException();
    return napi_pending_exception;
  } catch (const std::exception& error) {
    napi_throw_error(env, nullptr, error.what());
    return napi_pending_exception;
  }
#else
  return napi_run_script(env, source, source_url, result);
#endif
}

napi_status jsr_collect_garbage(napi_env env) {
#if defined(JSR_NAPI_ENGINE_JAVASCRIPTCORE)
  if (env == nullptr) {
    return napi_invalid_arg;
  }

  JSGlobalContextRef context = env->context;
  if (context == nullptr) {
    return napi_invalid_arg;
  }

  JSGarbageCollect(context);
  return napi_ok;
#elif defined(JSR_NAPI_ENGINE_V8)
  if (env == nullptr) {
    return napi_invalid_arg;
  }

  v8::Isolate* isolate = env->isolate;
  if (isolate == nullptr) {
    return napi_invalid_arg;
  }

  isolate->RequestGarbageCollectionForTesting(
      v8::Isolate::kFullGarbageCollection);
  while (!env->pending_finalizers.empty()) {
    (*env->pending_finalizers.begin())->Finalize();
  }
  return napi_ok;
#elif defined(JSR_NAPI_ENGINE_QUICKJS)
  if (env == nullptr || env->context == nullptr) {
    return napi_invalid_arg;
  }
  JS_RunGC(JS_GetRuntime(env->context));
  return napi_ok;
#elif defined(JSR_NAPI_ENGINE_CHAKRA)
  JsContextRef context{};
  JsRuntimeHandle runtime{};
  if (JsGetCurrentContext(&context) != JsNoError ||
      JsGetRuntime(context, &runtime) != JsNoError ||
      JsCollectGarbage(runtime) != JsNoError) {
    return napi_generic_failure;
  }
  return napi_ok;
#elif defined(JSR_NAPI_ENGINE_HERMES)
  if (env == nullptr) {
    return napi_invalid_arg;
  }
  Napi::CollectGarbage(Napi::Env{env});
  return napi_ok;
#else
  (void)env;
  return napi_generic_failure;
#endif
}

napi_status jsr_drain_microtasks(napi_env env,
                                 int32_t max_count_hint,
                                 bool* result) {
  if (env == nullptr || result == nullptr) {
    return napi_invalid_arg;
  }

#if defined(JSR_NAPI_ENGINE_QUICKJS)
  if (env->context == nullptr) {
    return napi_invalid_arg;
  }

  JSRuntime* runtime = JS_GetRuntime(env->context);
  JSContext* pending_context = nullptr;
  int32_t count = 0;
  while (max_count_hint <= 0 || count < max_count_hint) {
    int status = JS_ExecutePendingJob(runtime, &pending_context);
    if (status < 0) {
      return napi_pending_exception;
    }
    if (status == 0) {
      *result = true;
      return napi_ok;
    }
    ++count;
  }

  *result = !JS_IsJobPending(runtime);
  return napi_ok;
#elif defined(JSR_NAPI_ENGINE_V8)
  if (env->isolate == nullptr) {
    return napi_invalid_arg;
  }
  env->isolate->PerformMicrotaskCheckpoint();
  *result = true;
  return napi_ok;
#elif defined(JSR_NAPI_ENGINE_HERMES)
  (void)max_count_hint;
  Napi::DrainJobs(Napi::Env{env});
  *result = true;
  return napi_ok;
#elif defined(JSR_NAPI_ENGINE_JAVASCRIPTCORE) || \
    defined(JSR_NAPI_ENGINE_CHAKRA)
  (void)max_count_hint;
  *result = true;
  return napi_ok;
#else
  (void)max_count_hint;
  *result = false;
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
