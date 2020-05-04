#pragma once

#include <napi/js_native_api.h>
#include <napi/js_native_api_types.h>
#include <JavaScriptCore/JavaScript.h>

struct napi_env__ {
  JSContextRef context;
  JSValueRef last_exception;
  napi_extended_error_info last_error;
};

#define RETURN_STATUS_IF_FALSE(env, condition, status)                  \
  do {                                                                  \
    if (!(condition)) {                                                 \
      return napi_set_last_error((env), (status));                      \
    }                                                                   \
  } while (0)

#define CHECK_ENV(env)          \
  do {                          \
    if ((env) == nullptr) {     \
      return napi_invalid_arg;  \
    }                           \
  } while (0)

#define CHECK_ARG(env, arg) \
  RETURN_STATUS_IF_FALSE((env), ((arg) != nullptr), napi_invalid_arg)

#define CHECK_JSC(env, exception)                \
  do {                                           \
    if ((exception) != nullptr) {                \
      return napi_set_exception(env, exception); \
    }                                            \
  } while (0)

// This does not call napi_set_last_error because the expression
// is assumed to be a NAPI function call that already did.
#define CHECK_NAPI(expr)                  \
  do {                                    \
    napi_status status = (expr);          \
    if (status != napi_ok) return status; \
  } while (0)
