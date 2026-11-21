#pragma once

#define USE_EDGEMODE_JSRT
#define NOMINMAX

#include <jsrt.h>
#include <napi/js_native_api_types.h>
#include "js_native_api_shared.h"
#include <thread>
#include <cassert>
#include <map>

namespace napi_chakra_internal {
  void DiscardReferenceAfterRuntimeDisposal(napi_ref ref);
}

struct napi_env__ {
  JsSourceContext source_context = JS_SOURCE_CONTEXT_NONE;
  napi_extended_error_info last_error{ nullptr, nullptr, 0, napi_ok };
  JsValueRef has_own_property_function = JS_INVALID_REFERENCE;
  napi_ref has_own_property_reference{};
  napi_shared::PropertyNameIntrinsics property_name_intrinsics{};

  JsPropertyIdRef wrap_property_id = JS_INVALID_REFERENCE;
  napi_ref wrap_symbol_reference{};

  // Escapable scope bookkeeping: token -> whether that scope has escaped. Values
  // are rooted by the engine rather than by a scope here, so this exists only to
  // honour the one-escape-per-scope rule and to reject tokens that are not open.
  // The token is a monotonic counter, never an index into anything, so two scopes
  // can never share one.
  size_t next_escapable_scope_token = 0;
  std::map<size_t, bool> open_escapable_scopes;

  // napi_set_instance_data / napi_get_instance_data (N-API v6).
  void* instance_data = nullptr;
  napi_finalize instance_data_finalize_cb = nullptr;
  void* instance_data_finalize_hint = nullptr;

  const std::thread::id thread_id{std::this_thread::get_id()};

  ~napi_env__() {
    // Run the instance-data finalizer at env teardown (env_chakra.cc deletes the env), matching V8/JSC.
    if (instance_data_finalize_cb != nullptr) {
      instance_data_finalize_cb(this, instance_data, instance_data_finalize_hint);
    }
  }
};

#define RETURN_STATUS_IF_FALSE(env, condition, status) \
  do {                                                 \
    if (!(condition)) {                                \
      return napi_set_last_error((env), (status));     \
    }                                                  \
  } while (0)

#define CHECK_ENV(env)                                    \
  do {                                                    \
    if ((env) == nullptr) {                               \
      return napi_invalid_arg;                            \
    }                                                     \
    assert(env->thread_id == std::this_thread::get_id()); \
  } while (0)

#define CHECK_ARG(env, arg) \
  RETURN_STATUS_IF_FALSE((env), ((arg) != nullptr), napi_invalid_arg)

#define CHECK_JSRT(env, expr)                                   \
  do {                                                          \
    JsErrorCode err = (expr);                                   \
    if (err != JsNoError) return napi_set_last_error(env, err); \
  } while (0)

#define CHECK_JSRT_EXPECTED(env, expr, expected)                \
  do {                                                          \
    JsErrorCode err = (expr);                                   \
    if (err == JsErrorInvalidArgument)                          \
      return napi_set_last_error(env, expected);                \
    if (err != JsNoError) return napi_set_last_error(env, err); \
  } while (0)

#define CHECK_JSRT_ERROR_CODE(operation)                 \
  do {                                                   \
    auto result = operation;                             \
    if (result != JsErrorCode::JsNoError) return result; \
  } while (0)

// This does not call napi_set_last_error because the expression
// is assumed to be a NAPI function call that already did.
#define CHECK_NAPI(expr)                  \
  do {                                    \
    napi_status status = (expr);          \
    if (status != napi_ok) return status; \
  } while (0)

// utf8 multibyte codepoint start check
#define UTF8_MULTIBYTE_START(c) (((c) & 0xC0) == 0xC0)

#define STR_AND_LENGTH(str) str, sizeof(str) - 1

static void napi_clear_last_error(napi_env env) {
  env->last_error.error_code = napi_ok;
  env->last_error.engine_error_code = 0;
  env->last_error.engine_reserved = nullptr;
}

static napi_status napi_set_last_error(napi_env env, napi_status error_code, uint32_t engine_error_code = 0, void* engine_reserved = nullptr) {
  env->last_error.error_code = error_code;
  env->last_error.engine_error_code = engine_error_code;
  env->last_error.engine_reserved = engine_reserved;

  return error_code;
}

static napi_status napi_set_last_error(napi_env env, JsErrorCode jsError, void* engine_reserved = nullptr) {
  napi_status status;
  switch (jsError) {
    case JsNoError: status = napi_ok; break;
    case JsErrorNullArgument:
    case JsErrorInvalidArgument: status = napi_invalid_arg; break;
    case JsErrorPropertyNotString: status = napi_string_expected; break;
    case JsErrorArgumentNotObject: status = napi_object_expected; break;
    case JsErrorScriptException:
    case JsErrorInExceptionState: status = napi_pending_exception; break;
    default: status = napi_generic_failure; break;
  }

  env->last_error.error_code = status;
  env->last_error.engine_error_code = jsError;
  env->last_error.engine_reserved = engine_reserved;
  return status;
}
