#include <js_native_api.h>
#include "../common.h"
#include "../entry_point.h"

// Feature-detection fallback for the standard test_bigint. Engines without BigInt (jsc-android ~2020,
// whose parser even rejects `0n` literals, and the Win10 OS Chakra) must report the capability gap via
// a JS-catchable ENOTSUP exception from the BigInt create API rather than failing silently or crashing.
static napi_value CreateBigIntExpectThrow(napi_env env, napi_callback_info info) {
  (void)info;
  napi_value result = NULL;
  // On a BigInt-less engine this throws ENOTSUP (a pending exception); let it propagate to JS land,
  // where test.js asserts on the error code. (No `0n` literal here, so the script itself parses.)
  napi_create_bigint_int64(env, 42, &result);
  (void)result;
  return NULL;
}

EXTERN_C_START
napi_value Init(napi_env env, napi_value exports) {
  napi_property_descriptor properties[] = {
    DECLARE_NODE_API_PROPERTY("CreateBigIntExpectThrow", CreateBigIntExpectThrow),
  };

  NODE_API_CALL(env,
      napi_define_properties(
          env, exports, sizeof(properties) / sizeof(*properties), properties));

  return exports;
}
EXTERN_C_END
