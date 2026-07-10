#include "js_native_api_quickjs.h"
#include <napi/js_native_api.h>
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshorten-64-to-32"
#endif
#include <quickjs.h>
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
#include <array>
#include <cassert>
#include <cmath>
#include <vector>
#include <string>
#include <stdexcept>
#include <cstring>
#include <algorithm>

namespace {

// Custom class IDs for external objects
static JSClassID js_external_class_id = 0;
static JSClassID js_callback_class_id = 0;
static JSClassID js_wrap_class_id = 0;
static bool class_ids_allocated = false;

// Adapter for external data + finalize callback
class ExternalData {
 public:
  ExternalData(napi_env env, void* data, napi_finalize finalize_cb, void* hint)
    : _env(env), _data(data), _cb(finalize_cb), _hint(hint) {
    // Keep the env alive for as long as this adapter exists. The adapter may be
    // finalized during the engine's teardown cascade (JS_FreeContext), i.e.
    // after Napi::Detach has run, and RunCallback() then reaches back into the
    // env (e.g. an ObjectWrap destructor calling napi_delete_reference). This
    // ref guarantees the env outlives that final use.
    if (_env != nullptr) {
      _env->Ref();
    }
  }

  ~ExternalData() {
    if (_env != nullptr) {
      _env->Unref();
    }
  }

  void* Data() { return _data; }

  static void RunCallback(ExternalData* externalData) {
    if (externalData && externalData->_cb) {
      externalData->_cb(externalData->_env, externalData->_data, externalData->_hint);
    }
  }

  static void Finalize(JSRuntime *rt, JSValue val) {
    void* opaque = JS_GetOpaque(val, js_external_class_id);
    ExternalData* externalData = reinterpret_cast<ExternalData*>(opaque);
    if (externalData != nullptr) {
      RunCallback(externalData);
      delete externalData;
    }
  }

  static void FinalizeWrap(JSRuntime *rt, JSValue val) {
    void* opaque = JS_GetOpaque(val, js_wrap_class_id);
    ExternalData* externalData = reinterpret_cast<ExternalData*>(opaque);
    if (externalData != nullptr) {
      RunCallback(externalData);
      delete externalData;
    }
  }

 private:
  napi_env _env;
  void* _data;
  napi_finalize _cb;
  void* _hint;
};

// CallbackInfo struct
struct CallbackInfo {
  napi_value newTarget;
  napi_value thisArg;
  napi_value* argv;
  void* data;
  uint16_t argc;
  bool isConstructCall;
};

// Adapter for external callback + callback data
class ExternalCallback {
 public:
  ExternalCallback(napi_env env, napi_callback cb, void* data)
    : _env(env), _cb(cb), _data(data), newTarget(JS_UNDEFINED) {}

static JSValue Callback(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv, int magic, JSValue *func_data) {
  ExternalCallback* externalCallback = reinterpret_cast<ExternalCallback*>(JS_GetOpaque(func_data[0], js_callback_class_id));
  if (!externalCallback || !externalCallback->_cb) {
    return JS_UNDEFINED;
  }
  
  // SAVE AND SET current execution context
  JSContext* savedCtx = externalCallback->_env->current_context;
  externalCallback->_env->current_context = ctx;
  
  napi_clear_last_error(externalCallback->_env);

  // Build argv array
  std::vector<napi_value> args;
  args.reserve(argc);
  for (int i = 0; i < argc; i++) {
    args.push_back(reinterpret_cast<napi_value>(const_cast<JSValue*>(&argv[i])));
  }

  JSValue actualThis = JS_UNDEFINED;
  bool isConstructCall = (magic == 1);
  
  // Handle constructor call
  if (isConstructCall) {
    JSValue prototypeProperty = JS_GetPropertyStr(ctx, externalCallback->newTarget, "prototype");
    
    if (JS_IsException(prototypeProperty)) {
      externalCallback->_env->current_context = savedCtx; // RESTORE
      return JS_EXCEPTION;
    }
    
    // Create the instance with the wrap class id so it owns an opaque slot and
    // the NapiWrap finalizer directly. napi_wrap() then stores the native
    // pointer on the instance itself (via JS_SetOpaque) instead of inserting a
    // separate wrapper object into the prototype chain. This keeps the native
    // data completely invisible to JS: Object.getPrototypeOf(instance) is the
    // class prototype and no extra own-property is added (regression #172).
    actualThis = JS_NewObjectProtoClass(ctx, prototypeProperty, js_wrap_class_id);
    JS_FreeValue(ctx, prototypeProperty);
    
    if (JS_IsException(actualThis)) {
      externalCallback->_env->current_context = savedCtx; // RESTORE
      return JS_EXCEPTION;
    }
  } else {
    actualThis = JS_DupValue(ctx, this_val);
  }

  CallbackInfo cbInfo;
  cbInfo.thisArg = reinterpret_cast<napi_value>(&actualThis);
  cbInfo.newTarget = reinterpret_cast<napi_value>(&externalCallback->newTarget);
  cbInfo.isConstructCall = isConstructCall;
  cbInfo.argc = argc;
  cbInfo.argv = args.empty() ? nullptr : args.data();
  cbInfo.data = externalCallback->_data;

  napi_value callbackResult = nullptr;

  // Wrap callback invocation in try-catch to handle C++ exceptions
  try {
    callbackResult = externalCallback->_cb(externalCallback->_env, reinterpret_cast<napi_callback_info>(&cbInfo));
  } catch (const std::exception& e) {
    // C++ exception was thrown - convert to JavaScript exception
    // The exception should already be set via ThrowAsJavaScriptException()
    // but if not, create a generic error
    if (!JS_HasException(ctx)) {
      JS_ThrowInternalError(ctx, "Uncaught C++ exception: %s", e.what());
    }
    // See the isConstructCall note below: detach any wrap finalizer so a
    // throwing constructor does not leave a finalizer attached to the freed
    // native instance.
    if (isConstructCall) {
      napi_remove_wrap(externalCallback->_env, reinterpret_cast<napi_value>(&actualThis), nullptr);
    }
    JS_FreeValue(ctx, actualThis);
    externalCallback->_env->current_context = savedCtx;
    return JS_EXCEPTION;
  }

  // Check for exception in the CURRENT execution context
  if (JS_HasException(ctx)) {
    // If a constructor left a pending exception, node-addon-api has already
    // destroyed the C++ ObjectWrap instance during stack unwinding, but the
    // wrap finalizer registered by napi_wrap() is still attached to
    // `actualThis`. Detach it now so a later GC does not run the finalizer on
    // the freed native instance (use-after-free / heap corruption). The
    // pending JS exception is preserved across this cleanup.
    if (isConstructCall) {
      napi_remove_wrap(externalCallback->_env, reinterpret_cast<napi_value>(&actualThis), nullptr);
    }
    JS_FreeValue(ctx, actualThis);
    externalCallback->_env->current_context = savedCtx; // RESTORE
    return JS_EXCEPTION;
  }
  
  // Now handle the normal return value
  JSValue returnValue;
  
  if (callbackResult == nullptr) {
    if (isConstructCall) {
      // Constructor with no explicit return: the freshly created instance is
      // the result (ownership transfers to returnValue).
      returnValue = actualThis;
    } else {
      // Plain function returning void: actualThis was only a dup of `this_val`
      // used to build the CallbackInfo. Release it so `this` is not leaked on
      // every call (e.g. console.log, which pins the console object otherwise).
      returnValue = JS_UNDEFINED;
      JS_FreeValue(ctx, actualThis);
    }
  } else {
    JSValue cbResultValue = *reinterpret_cast<JSValue*>(callbackResult);
    
    if (isConstructCall) {
      if (JS_IsObject(cbResultValue)) {
        returnValue = JS_DupValue(ctx, cbResultValue);
        JS_FreeValue(ctx, actualThis);
      } else {
        returnValue = actualThis;
      }
    } else {
      returnValue = JS_DupValue(ctx, cbResultValue);
      JS_FreeValue(ctx, actualThis);
    }
  }
  
  externalCallback->_env->current_context = savedCtx; // RESTORE
  return returnValue;
}

  static void Finalize(JSRuntime *rt, JSValue val) {
    void* opaque = JS_GetOpaque(val, js_callback_class_id);
    ExternalCallback* externalCallback = reinterpret_cast<ExternalCallback*>(opaque);
    if (externalCallback != nullptr) {
      JS_FreeValueRT(rt, externalCallback->newTarget);
      delete externalCallback;
    }
  }

  // Expose newTarget (a strong JSValue held by this class) to QuickJS's cycle
  // collector. This is what allows the cycle
  //   func(cid=15) -> func_data[callbackData(cid=66)] -> opaque -> newTarget -> func
  // to be detected and broken at teardown.
  static void GcMark(JSRuntime *rt, JSValueConst val, JS_MarkFunc *mark_func) {
    void* opaque = JS_GetOpaque(val, js_callback_class_id);
    ExternalCallback* externalCallback = reinterpret_cast<ExternalCallback*>(opaque);
    if (externalCallback != nullptr) {
      JS_MarkValue(rt, externalCallback->newTarget, mark_func);
    }
  }

  JSValue newTarget;

 private:
  napi_env _env;
  napi_callback _cb;
  void* _data;
};

// Reference info for preventing GC - defined in js_native_api_quickjs.h.

// Initialize class IDs (allocate once globally, register per runtime)
void InitClassIds(JSRuntime* rt) {
  if (!class_ids_allocated) {
    JS_NewClassID(rt, &js_external_class_id);
    JS_NewClassID(rt, &js_callback_class_id);
    JS_NewClassID(rt, &js_wrap_class_id);
    class_ids_allocated = true;
  }

  // Register class defs per runtime (JS_NewClass is a no-op if already registered)
  JSClassDef external_class_def = {
    "NapiExternal",
    ExternalData::Finalize,
    nullptr,
    nullptr,
    nullptr
  };
  JS_NewClass(rt, js_external_class_id, &external_class_def);

  JSClassDef callback_class_def = {
    "NapiCallback",
    ExternalCallback::Finalize,
    ExternalCallback::GcMark,
    nullptr,
    nullptr
  };
  JS_NewClass(rt, js_callback_class_id, &callback_class_def);

  JSClassDef wrap_class_def = {
    "NapiWrap",
    ExternalData::FinalizeWrap,
    nullptr,
    nullptr,
    nullptr
  };
  JS_NewClass(rt, js_wrap_class_id, &wrap_class_def);
}

// Helper to convert napi_value to JSValue
inline JSValue ToJSValue(napi_value val) {
  if (val == nullptr) {
    return JS_UNDEFINED;
  }
  return *reinterpret_cast<JSValue*>(val);
}

// Helper to create napi_value from JSValue
// Note: This returns a pointer to a temporary. The caller must handle lifetime.
inline napi_value FromJSValue(napi_env env, JSValue val) {
  
    JSContext* ctx = env->current_context;
  // Store in current handle scope
  auto ptr = std::make_unique<JSValue>(val);
  napi_value result = reinterpret_cast<napi_value>(ptr.get());
  
  /*napi_value result;

  JSValue* ptr = new JSValue(val);
  result = reinterpret_cast<napi_value>(ptr);*/
  
  if (env /*&& !env->handle_scope_stack.empty()*/) {
    env->handle_scope_stack.push_back(std::move(ptr));
  }
  
  return result;
}

// Helper for property attributes
int ToQuickJSPropertyFlags(napi_property_attributes attributes) {
  int flags = 0;
  if (attributes & napi_writable)
    flags |= JS_PROP_WRITABLE;
  if (attributes & napi_enumerable)
    flags |= JS_PROP_ENUMERABLE;
  if (attributes & napi_configurable)
    flags |= JS_PROP_CONFIGURABLE;
  return flags;
}

// Error message table
static const char* error_messages[] = {
  nullptr,
  "Invalid argument",
  "An object was expected",
  "A string was expected",
  "A string or symbol was expected",
  "A function was expected",
  "A number was expected",
  "A boolean was expected",
  "An array was expected",
  "Unknown failure",
  "An exception is pending",
  "The async work item was cancelled",
  "napi_escape_handle already called on scope",
  "Invalid handle scope usage",
  "Invalid callback scope usage",
  "Thread-safe function queue is full",
  "Thread-safe function handle is closing",
  "A bigint was expected",
  "A date was expected",
  "An arraybuffer was expected",
  "A detachable arraybuffer was expected",
  "Would deadlock",
  "No external buffers allowed",
  "Cannot run JS",
};

} // anonymous namespace

// Get last error message
napi_status napi_get_last_error_info(napi_env env, const napi_extended_error_info** result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);
  
  static_assert(std::size(error_messages) == napi_cannot_run_js + 1,
    "Count of error messages must match count of error values");
  
  if (env->last_error.error_code < std::size(error_messages)) {
    env->last_error.error_message = error_messages[env->last_error.error_code];
  }
  
  *result = &env->last_error;
  return napi_ok;
}

// Get undefined
napi_status napi_get_undefined(napi_env env, napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);
  static JSValue undefined = JS_UNDEFINED;
  *result = reinterpret_cast<napi_value>(&undefined);
  napi_clear_last_error(env);
  return napi_ok;
}

// Get null
napi_status napi_get_null(napi_env env, napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);
  static JSValue null_val = JS_NULL;
  *result = reinterpret_cast<napi_value>(&null_val);
  napi_clear_last_error(env);
  return napi_ok;
}

// Get global
napi_status napi_get_global(napi_env env, napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);
  JSValue global = JS_GetGlobalObject(env->context);
  *result = FromJSValue(env, global);
  napi_clear_last_error(env);
  return napi_ok;
}

// Get boolean
napi_status napi_get_boolean(napi_env env, bool value, napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);
  static JSValue js_true = JS_TRUE;
  static JSValue js_false = JS_FALSE;
  *result = value ? reinterpret_cast<napi_value>(&js_true) : reinterpret_cast<napi_value>(&js_false);
  napi_clear_last_error(env);
  return napi_ok;
}

// Create number (double)
napi_status napi_create_double(napi_env env, double value, napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);
  *result = FromJSValue(env, JS_NewFloat64(env->context, value));
  napi_clear_last_error(env);
  return napi_ok;
}

// Create number (int32)
napi_status napi_create_int32(napi_env env, int32_t value, napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);
  *result = FromJSValue(env, JS_NewInt32(env->context, value));
  napi_clear_last_error(env);
  return napi_ok;
}

// Create number (uint32)
napi_status napi_create_uint32(napi_env env, uint32_t value, napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);
  *result = FromJSValue(env, JS_NewUint32(env->context, value));
  napi_clear_last_error(env);
  return napi_ok;
}

// Create number (int64)
napi_status napi_create_int64(napi_env env, int64_t value, napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);
  *result = FromJSValue(env, JS_NewInt64(env->context, value));
  napi_clear_last_error(env);
  return napi_ok;
}

// Create string UTF8
napi_status napi_create_string_utf8(napi_env env, const char* str, size_t length, napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);
  
  JSValue jsStr;
  if (length == NAPI_AUTO_LENGTH) {
    jsStr = JS_NewString(env->context, str);
  } else {
    jsStr = JS_NewStringLen(env->context, str, length);
  }
  
  if (JS_IsException(jsStr)) {
    return napi_set_last_error(env, napi_generic_failure);
  }
  
  *result = FromJSValue(env, jsStr);
  napi_clear_last_error(env);
  return napi_ok;
}

// Create string latin1
napi_status napi_create_string_latin1(napi_env env, const char* str, size_t length, napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);
  
  // QuickJS doesn't have native Latin-1 support, so we convert to UTF-8
  // For now, just treat as UTF-8 (this may not be fully correct for high bytes)
  if (length == NAPI_AUTO_LENGTH) {
    length = strlen(str);
  }
  
  JSValue jsStr = JS_NewStringLen(env->context, str, length);
  if (JS_IsException(jsStr)) {
    return napi_set_last_error(env, napi_generic_failure);
  }
  
  *result = FromJSValue(env, jsStr);
  napi_clear_last_error(env);
  return napi_ok;
}

// Create string UTF16
napi_status napi_create_string_utf16(napi_env env, const char16_t* str, size_t length, napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);
  
  // Convert UTF-16 to UTF-8 for QuickJS
  if (length == NAPI_AUTO_LENGTH) {
    length = 0;
    while (str[length] != 0) length++;
  }
  
  // Simple UTF-16 to UTF-8 conversion
  std::string utf8;
  utf8.reserve(length * 3); // worst case
  
  for (size_t i = 0; i < length; i++) {
    uint32_t cp = str[i];
    
    // Handle surrogate pairs
    if (cp >= 0xD800 && cp <= 0xDBFF && i + 1 < length) {
      uint16_t low = str[i + 1];
      if (low >= 0xDC00 && low <= 0xDFFF) {
        cp = 0x10000 + ((cp - 0xD800) << 10) + (low - 0xDC00);
        i++;
      }
    }
    
    if (cp < 0x80) {
      utf8.push_back(static_cast<char>(cp));
    } else if (cp < 0x800) {
      utf8.push_back(static_cast<char>(0xC0 | (cp >> 6)));
      utf8.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    } else if (cp < 0x10000) {
      utf8.push_back(static_cast<char>(0xE0 | (cp >> 12)));
      utf8.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
      utf8.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    } else {
      utf8.push_back(static_cast<char>(0xF0 | (cp >> 18)));
      utf8.push_back(static_cast<char>(0x80 | ((cp >> 12) & 0x3F)));
      utf8.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
      utf8.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    }
  }
  
  JSValue jsStr = JS_NewStringLen(env->context, utf8.c_str(), utf8.length());
  if (JS_IsException(jsStr)) {
    return napi_set_last_error(env, napi_generic_failure);
  }
  
  *result = FromJSValue(env, jsStr);
  napi_clear_last_error(env);
  return napi_ok;
}

// Get value type
napi_status napi_typeof(napi_env env, napi_value value, napi_valuetype* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, value);
  CHECK_ARG(env, result);

  JSValue jsValue = ToJSValue(value);
  
  if (JS_IsNumber(jsValue)) {
    *result = napi_number;
  } else if (JS_IsBool(jsValue)) {
    *result = napi_boolean;
  } else if (JS_IsString(jsValue)) {
    *result = napi_string;
  } else if (JS_IsSymbol(jsValue)) {
    *result = napi_symbol;
  } else if (JS_IsObject(jsValue)) {
    if (JS_IsFunction(env->context, jsValue)) {
      *result = napi_function;
    } else {
      // Check if it's an external object
      JSClassID class_id = JS_GetClassID(jsValue);
      if (class_id == js_external_class_id) {
        *result = napi_external;
      } else {
        *result = napi_object;
      }
    }
  } else if (JS_IsNull(jsValue)) {
    *result = napi_null;
  } else if (JS_IsUndefined(jsValue)) {
    *result = napi_undefined;
  } else if (JS_IsBigInt(jsValue)) {
    *result = napi_bigint;
  } else {
    *result = napi_undefined;
  }

  napi_clear_last_error(env);
  return napi_ok;
}

// Get value double
napi_status napi_get_value_double(napi_env env, napi_value value, double* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, value);
  CHECK_ARG(env, result);

  JSValue jsValue = ToJSValue(value);
  if (JS_ToFloat64(env->context, result, jsValue) < 0) {
    return napi_set_last_error(env, napi_number_expected);
  }

  napi_clear_last_error(env);
  return napi_ok;
}

// Get value int32
napi_status napi_get_value_int32(napi_env env, napi_value value, int32_t* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, value);
  CHECK_ARG(env, result);

  JSValue jsValue = ToJSValue(value);
  if (JS_ToInt32(env->context, result, jsValue) < 0) {
    return napi_set_last_error(env, napi_number_expected);
  }

  napi_clear_last_error(env);
  return napi_ok;
}

// Get value uint32
napi_status napi_get_value_uint32(napi_env env, napi_value value, uint32_t* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, value);
  CHECK_ARG(env, result);

  JSValue jsValue = ToJSValue(value);
  if (JS_ToUint32(env->context, result, jsValue) < 0) {
    return napi_set_last_error(env, napi_number_expected);
  }

  napi_clear_last_error(env);
  return napi_ok;
}

// Get value int64
napi_status napi_get_value_int64(napi_env env, napi_value value, int64_t* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, value);
  CHECK_ARG(env, result);

  JSValue jsValue = ToJSValue(value);
  
  double d;
  if (JS_ToFloat64(env->context, &d, jsValue) < 0) {
    return napi_set_last_error(env, napi_number_expected);
  }
  
  if (std::isfinite(d)) {
    *result = static_cast<int64_t>(d);
  } else {
    *result = 0;
  }

  napi_clear_last_error(env);
  return napi_ok;
}

// Get value bool
napi_status napi_get_value_bool(napi_env env, napi_value value, bool* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, value);
  CHECK_ARG(env, result);

  JSValue jsValue = ToJSValue(value);
  
  if (!JS_IsBool(jsValue)) {
    return napi_set_last_error(env, napi_boolean_expected);
  }
  
  *result = JS_VALUE_GET_BOOL(jsValue) != 0;
  napi_clear_last_error(env);
  return napi_ok;
}

// Get value string UTF8
napi_status napi_get_value_string_utf8(napi_env env, napi_value value, char* buf, size_t bufsize, size_t* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, value);

  JSValue jsValue = ToJSValue(value);
  
  if (!JS_IsString(jsValue)) {
    return napi_set_last_error(env, napi_string_expected);
  }
  
  size_t len;
  const char* str = JS_ToCStringLen(env->context, &len, jsValue);
  if (!str) {
    return napi_set_last_error(env, napi_string_expected);
  }

  if (buf == nullptr) {
    if (result != nullptr) {
      *result = len;
    }
  } else if (bufsize > 0) {
    size_t copy_len = std::min(len, bufsize - 1);
    memcpy(buf, str, copy_len);
    buf[copy_len] = '\0';
    if (result != nullptr) {
      *result = copy_len;
    }
  } else if (result != nullptr) {
    // buf != nullptr but bufsize == 0: no room for the null terminator. Report
    // zero copied and write nothing, instead of evaluating bufsize - 1 (which
    // would underflow to SIZE_MAX).
    *result = 0;
  }

  JS_FreeCString(env->context, str);
  napi_clear_last_error(env);
  return napi_ok;
}

// Get value string latin1
napi_status napi_get_value_string_latin1(napi_env env, napi_value value, char* buf, size_t bufsize, size_t* result) {
  // For simplicity, treat same as UTF-8
  return napi_get_value_string_utf8(env, value, buf, bufsize, result);
}

// Get value string UTF16
napi_status napi_get_value_string_utf16(napi_env env, napi_value value, char16_t* buf, size_t bufsize, size_t* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, value);

  JSValue jsValue = ToJSValue(value);
  
  if (!JS_IsString(jsValue)) {
    return napi_set_last_error(env, napi_string_expected);
  }
  
  size_t len;
  const char* str = JS_ToCStringLen(env->context, &len, jsValue);
  if (!str) {
    return napi_set_last_error(env, napi_string_expected);
  }

  // Convert UTF-8 to UTF-16
  std::vector<char16_t> utf16;
  utf16.reserve(len);
  
  for (size_t i = 0; i < len; ) {
    uint32_t cp;
    unsigned char c = str[i];
    
    if ((c & 0x80) == 0) {
      cp = c;
      i++;
    } else if ((c & 0xE0) == 0xC0) {
      cp = (c & 0x1F) << 6;
      if (i + 1 < len) cp |= (str[i+1] & 0x3F);
      i += 2;
    } else if ((c & 0xF0) == 0xE0) {
      cp = (c & 0x0F) << 12;
      if (i + 1 < len) cp |= (str[i+1] & 0x3F) << 6;
      if (i + 2 < len) cp |= (str[i+2] & 0x3F);
      i += 3;
    } else if ((c & 0xF8) == 0xF0) {
      cp = (c & 0x07) << 18;
      if (i + 1 < len) cp |= (str[i+1] & 0x3F) << 12;
      if (i + 2 < len) cp |= (str[i+2] & 0x3F) << 6;
      if (i + 3 < len) cp |= (str[i+3] & 0x3F);
      i += 4;
    } else {
      cp = 0xFFFD; // replacement character
      i++;
    }
    
    if (cp < 0x10000) {
      utf16.push_back(static_cast<char16_t>(cp));
    } else {
      cp -= 0x10000;
      utf16.push_back(static_cast<char16_t>(0xD800 + (cp >> 10)));
      utf16.push_back(static_cast<char16_t>(0xDC00 + (cp & 0x3FF)));
    }
  }

  JS_FreeCString(env->context, str);

  if (buf == nullptr) {
    if (result != nullptr) {
      *result = utf16.size();
    }
  } else if (bufsize > 0) {
    size_t copy_len = std::min(utf16.size(), bufsize - 1);
    memcpy(buf, utf16.data(), copy_len * sizeof(char16_t));
    buf[copy_len] = 0;
    if (result != nullptr) {
      *result = copy_len;
    }
  } else if (result != nullptr) {
    // buf != nullptr but bufsize == 0: no room for any character or the null
    // terminator. Report zero copied and write nothing, instead of evaluating
    // bufsize - 1 (which would underflow to SIZE_MAX and copy the whole string
    // into the zero-length buffer).
    *result = 0;
  }

  napi_clear_last_error(env);
  return napi_ok;
}

// Coerce to bool
napi_status napi_coerce_to_bool(napi_env env, napi_value value, napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, value);
  CHECK_ARG(env, result);

  JSValue jsValue = ToJSValue(value);
  int boolResult = JS_ToBool(env->context, jsValue);
  if (boolResult < 0) {
    return napi_set_last_error(env, napi_generic_failure);
  }

  static JSValue js_true = JS_TRUE;
  static JSValue js_false = JS_FALSE;
  *result = boolResult ? reinterpret_cast<napi_value>(&js_true) : reinterpret_cast<napi_value>(&js_false);
  napi_clear_last_error(env);
  return napi_ok;
}

// Coerce to number
napi_status napi_coerce_to_number(napi_env env, napi_value value, napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, value);
  CHECK_ARG(env, result);

  JSValue jsValue = ToJSValue(value);
  double num;
  if (JS_ToFloat64(env->context, &num, jsValue) < 0) {
    return napi_set_last_error(env, napi_generic_failure);
  }

  *result = FromJSValue(env, JS_NewFloat64(env->context, num));
  napi_clear_last_error(env);
  return napi_ok;
}

// Coerce to string
napi_status napi_coerce_to_string(napi_env env, napi_value value, napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, value);
  CHECK_ARG(env, result);

  JSValue jsValue = ToJSValue(value);
  JSValue jsString = JS_ToString(env->context, jsValue);
  
  if (JS_IsException(jsString)) {
    return napi_set_last_error(env, napi_generic_failure);
  }

  *result = FromJSValue(env, jsString);
  napi_clear_last_error(env);
  return napi_ok;
}

// Coerce to object
napi_status napi_coerce_to_object(napi_env env, napi_value value, napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, value);
  CHECK_ARG(env, result);

  JSValue jsValue = ToJSValue(value);
  
  JSValue global = JS_GetGlobalObject(env->context);
  JSValue objectCtor = JS_GetPropertyStr(env->context, global, "Object");
  JSValueConst args[1] = { jsValue };
  JSValue jsObject = JS_Call(env->context, objectCtor, JS_UNDEFINED, 1, args);
  
  JS_FreeValue(env->context, objectCtor);
  JS_FreeValue(env->context, global);

  if (JS_IsException(jsObject)) {
    return napi_set_last_error(env, napi_generic_failure);
  }

  *result = FromJSValue(env, jsObject);
  napi_clear_last_error(env);
  return napi_ok;
}

// Get prototype
napi_status napi_get_prototype(napi_env env, napi_value object, napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, object);
  CHECK_ARG(env, result);

  JSValue jsObject = ToJSValue(object);
  JSValue prototype = JS_GetPrototype(env->context, jsObject);
  
  if (JS_IsException(prototype)) {
    return napi_set_last_error(env, napi_object_expected);
  }

  *result = FromJSValue(env, prototype);
  napi_clear_last_error(env);
  return napi_ok;
}

// Create object
napi_status napi_create_object(napi_env env, napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);

  JSValue obj = JS_NewObject(env->context);
  if (JS_IsException(obj)) {
    return napi_set_last_error(env, napi_generic_failure);
  }

  *result = FromJSValue(env, obj);
  napi_clear_last_error(env);
  return napi_ok;
}

// Create array
napi_status napi_create_array(napi_env env, napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);

  JSValue arr = JS_NewArray(env->context);
  if (JS_IsException(arr)) {
    return napi_set_last_error(env, napi_generic_failure);
  }

  *result = FromJSValue(env, arr);
  napi_clear_last_error(env);
  return napi_ok;
}

// Create array with length
napi_status napi_create_array_with_length(napi_env env, size_t length, napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);

  JSValue arr = JS_NewArray(env->context);
  if (JS_IsException(arr)) {
    return napi_set_last_error(env, napi_generic_failure);
  }

  JSAtom lengthAtom = JS_NewAtom(env->context, "length");
  JS_SetProperty(env->context, arr, lengthAtom, JS_NewUint32(env->context, static_cast<uint32_t>(length)));
  JS_FreeAtom(env->context, lengthAtom);

  *result = FromJSValue(env, arr);
  napi_clear_last_error(env);
  return napi_ok;
}

// Get array length
napi_status napi_get_array_length(napi_env env, napi_value value, uint32_t* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, value);
  CHECK_ARG(env, result);

  JSValue jsValue = ToJSValue(value);
  JSValue lenVal = JS_GetPropertyStr(env->context, jsValue, "length");
  
  if (JS_IsException(lenVal)) {
    return napi_set_last_error(env, napi_array_expected);
  }

  if (JS_ToUint32(env->context, result, lenVal) < 0) {
    JS_FreeValue(env->context, lenVal);
    return napi_set_last_error(env, napi_number_expected);
  }

  JS_FreeValue(env->context, lenVal);
  napi_clear_last_error(env);
  return napi_ok;
}

// Internal helper to create function with custom magic value
static napi_status create_function_internal(napi_env env, const char* utf8name, size_t length, napi_callback cb, void* data, int magic, napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, cb);
  CHECK_ARG(env, result);
  
  JSRuntime* rt = JS_GetRuntime(env->context);
  InitClassIds(rt);
  
  ExternalCallback* externalCallback = new ExternalCallback(env, cb, data);
  
  // Create opaque object to hold callback data
  JSValue callbackData = JS_NewObjectClass(env->context, js_callback_class_id);
  JS_SetOpaque(callbackData, externalCallback);
  
  // Create function with data - magic parameter determines if it's a constructor call
  JSValueConst funcDataArray[1] = { callbackData };
  JSValue func = JS_NewCFunctionData(env->context, ExternalCallback::Callback, 0, magic, 1, funcDataArray);
  
  if (JS_IsException(func)) {
    delete externalCallback;
    JS_FreeValue(env->context, callbackData);
    return napi_set_last_error(env, napi_generic_failure);
  }
  
  // Set function name if provided
  if (utf8name != nullptr) {
    size_t name_len = (length == NAPI_AUTO_LENGTH) ? strlen(utf8name) : length;
    JSAtom nameAtom = JS_NewAtomLen(env->context, utf8name, name_len);
    JSAtom nameProp = JS_NewAtom(env->context, "name");
    JS_DefinePropertyValue(env->context, func, nameProp, 
                          JS_AtomToString(env->context, nameAtom), 0);
    JS_FreeAtom(env->context, nameProp);
    JS_FreeAtom(env->context, nameAtom);
  }
  
  // Release our local ref on callbackData. JS_NewCFunctionData has already
  // dup'd it into func's data array, so it stays alive as long as func does.
  JS_FreeValue(env->context, callbackData);

  // newTarget is a strong dup of the function — but it's only read for
  // constructor calls (see Callback() above, isConstructCall-gated path).
  // For regular functions (magic=0) we'd be allocating a useless self-cycle
  //   func -> func_data[callbackData] -> opaque ExternalCallback -> newTarget -> func
  // for every Napi::Function::New call (lambdas, mocha callbacks, console.log,
  // etc.). Even with NapiCallback's gc_mark exposing newTarget to the cycle
  // collector, in practice many of these cycles end up externally pinned (e.g.
  // by closures captured into bytecode) and survive teardown, leaving entries
  // on rt->gc_obj_list and tripping the assert in JS_FreeRuntime.
  if (magic == 1) {
    externalCallback->newTarget = JS_DupValue(env->context, func);
  }
  
  *result = FromJSValue(env, func);
  napi_clear_last_error(env);
  return napi_ok;
}

// Create function (regular function with magic=0)
napi_status napi_create_function(napi_env env, const char* utf8name, size_t length, napi_callback cb, void* data, napi_value* result) {
  return create_function_internal(env, utf8name, length, cb, data, 0, result);
}

// Create function for use as constructor (magic=1)
napi_status napi_create_function_with_magic(napi_env env, const char* utf8name, size_t length, napi_callback cb, void* data, napi_value* result) {
  return create_function_internal(env, utf8name, length, cb, data, 1, result);
}

// Get cb info
napi_status napi_get_cb_info(napi_env env, napi_callback_info cbinfo, size_t* argc, napi_value* argv, napi_value* this_arg, void** data) {
  CHECK_ENV(env);
  CHECK_ARG(env, cbinfo);

  CallbackInfo* info = reinterpret_cast<CallbackInfo*>(cbinfo);

  if (argv != nullptr && argc != nullptr) {
    size_t i = 0;
    size_t min = std::min(*argc, static_cast<size_t>(info->argc));
    
    for (; i < min; i++) {
      argv[i] = info->argv[i];
    }
    
    // Fill remaining with undefined
    static JSValue undefined = JS_UNDEFINED;
    for (; i < *argc; i++) {
      argv[i] = reinterpret_cast<napi_value>(&undefined);
    }
  }

  if (argc != nullptr) {
    *argc = info->argc;
  }

  if (this_arg != nullptr) {
    *this_arg = info->thisArg;
  }

  if (data != nullptr) {
    *data = info->data;
  }

  napi_clear_last_error(env);
  return napi_ok;
}

// Get new target
napi_status napi_get_new_target(napi_env env, napi_callback_info cbinfo, napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, cbinfo);
  CHECK_ARG(env, result);

  CallbackInfo* info = reinterpret_cast<CallbackInfo*>(cbinfo);
  
  if (info->isConstructCall) {
    *result = info->newTarget;
  } else {
    static JSValue undefined = JS_UNDEFINED;
    *result = reinterpret_cast<napi_value>(&undefined);
  }

  napi_clear_last_error(env);
  return napi_ok;
}

// Property operations
napi_status napi_get_property(napi_env env, napi_value object, napi_value key, napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, object);
  CHECK_ARG(env, key);
  CHECK_ARG(env, result);

  JSValue jsObject = ToJSValue(object);
  JSValue jsKey = ToJSValue(key);
  
  JSAtom atom = JS_ValueToAtom(env->context, jsKey);
  if (atom == JS_ATOM_NULL) {
    return napi_set_last_error(env, napi_generic_failure);
  }

  JSValue jsResult = JS_GetProperty(env->context, jsObject, atom);
  JS_FreeAtom(env->context, atom);

  if (JS_IsException(jsResult)) {
    return napi_set_last_error(env, napi_generic_failure);
  }

  *result = FromJSValue(env, jsResult);
  napi_clear_last_error(env);
  return napi_ok;
}

napi_status napi_set_property(napi_env env, napi_value object, napi_value key, napi_value value) {
  CHECK_ENV(env);
  CHECK_ARG(env, object);
  CHECK_ARG(env, key);
  CHECK_ARG(env, value);

  JSValue jsObject = ToJSValue(object);
  JSValue jsKey = ToJSValue(key);
  JSValue jsValue = ToJSValue(value);
  
  JSAtom atom = JS_ValueToAtom(env->context, jsKey);
  if (atom == JS_ATOM_NULL) {
    return napi_set_last_error(env, napi_generic_failure);
  }

  int ret = JS_SetProperty(env->context, jsObject, atom, JS_DupValue(env->context, jsValue));
  JS_FreeAtom(env->context, atom);

  if (ret < 0) {
    return napi_set_last_error(env, napi_generic_failure);
  }

  napi_clear_last_error(env);
  return napi_ok;
}

napi_status napi_has_property(napi_env env, napi_value object, napi_value key, bool* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, object);
  CHECK_ARG(env, key);
  CHECK_ARG(env, result);

  JSValue jsObject = ToJSValue(object);
  JSValue jsKey = ToJSValue(key);
  
  JSAtom atom = JS_ValueToAtom(env->context, jsKey);
  if (atom == JS_ATOM_NULL) {
    return napi_set_last_error(env, napi_generic_failure);
  }

  int has = JS_HasProperty(env->context, jsObject, atom);
  JS_FreeAtom(env->context, atom);

  if (has < 0) {
    return napi_set_last_error(env, napi_generic_failure);
  }

  *result = (has != 0);
  napi_clear_last_error(env);
  return napi_ok;
}

napi_status napi_delete_property(napi_env env, napi_value object, napi_value key, bool* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, object);
  CHECK_ARG(env, key);

  JSValue jsObject = ToJSValue(object);
  JSValue jsKey = ToJSValue(key);
  
  JSAtom atom = JS_ValueToAtom(env->context, jsKey);
  if (atom == JS_ATOM_NULL) {
    return napi_set_last_error(env, napi_generic_failure);
  }

  int deleted = JS_DeleteProperty(env->context, jsObject, atom, 0);
  JS_FreeAtom(env->context, atom);

  if (deleted < 0) {
    return napi_set_last_error(env, napi_generic_failure);
  }

  if (result != nullptr) {
    *result = (deleted != 0);
  }

  napi_clear_last_error(env);
  return napi_ok;
}

napi_status napi_has_own_property(napi_env env, napi_value object, napi_value key, bool* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, object);
  CHECK_ARG(env, key);
  CHECK_ARG(env, result);
  
  JSValue jsObject = ToJSValue(object);
  JSValue jsKey = ToJSValue(key);
  
  // Use the cached hasOwnProperty function
  if (!JS_IsUndefined(env->has_own_property_function)) {
    JSValueConst args[1] = { jsKey };
    JSValue hasResult = JS_Call(env->context, env->has_own_property_function, jsObject, 1, args);
    
    if (JS_IsException(hasResult)) {
      return napi_set_last_error(env, napi_generic_failure);
    }
    
    *result = JS_ToBool(env->context, hasResult) != 0;
    JS_FreeValue(env->context, hasResult);
  } else {
    // Fallback: check if property exists and is own
    JSAtom atom = JS_ValueToAtom(env->context, jsKey);
    if (atom == JS_ATOM_NULL) {
      return napi_set_last_error(env, napi_generic_failure);
    }
    
    JSPropertyDescriptor desc;
    int ret = JS_GetOwnProperty(env->context, &desc, jsObject, atom);
    JS_FreeAtom(env->context, atom);
    
    if (ret < 0) {
      return napi_set_last_error(env, napi_generic_failure);
    }
    
    *result = (ret == 1);
    
    if (ret == 1) {
      JS_FreeValue(env->context, desc.value);
      JS_FreeValue(env->context, desc.getter);
      JS_FreeValue(env->context, desc.setter);
    }
  }
  
  napi_clear_last_error(env);
  return napi_ok;
}

napi_status napi_get_named_property(napi_env env, napi_value object, const char* utf8name, napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, object);
  CHECK_ARG(env, utf8name);
  CHECK_ARG(env, result);

  JSValue jsObject = ToJSValue(object);
  JSValue jsResult = JS_GetPropertyStr(env->context, jsObject, utf8name);

  if (JS_IsException(jsResult)) {
    return napi_set_last_error(env, napi_generic_failure);
  }

  *result = FromJSValue(env, jsResult);
  napi_clear_last_error(env);
  return napi_ok;
}

napi_status napi_set_named_property(napi_env env, napi_value object, const char* utf8name, napi_value value) {
  CHECK_ENV(env);
  CHECK_ARG(env, object);
  CHECK_ARG(env, utf8name);
  CHECK_ARG(env, value);

  JSValue jsObject = ToJSValue(object);
  JSValue jsValue = ToJSValue(value);

  if (JS_SetPropertyStr(env->context, jsObject, utf8name, JS_DupValue(env->context, jsValue)) < 0) {
    return napi_set_last_error(env, napi_generic_failure);
  }

  napi_clear_last_error(env);
  return napi_ok;
}

napi_status napi_has_named_property(napi_env env, napi_value object, const char* utf8name, bool* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, object);
  CHECK_ARG(env, utf8name);
  CHECK_ARG(env, result);

  JSValue jsObject = ToJSValue(object);
  JSAtom atom = JS_NewAtom(env->context, utf8name);
  
  int has = JS_HasProperty(env->context, jsObject, atom);
  JS_FreeAtom(env->context, atom);

  if (has < 0) {
    return napi_set_last_error(env, napi_generic_failure);
  }

  *result = (has != 0);
  napi_clear_last_error(env);
  return napi_ok;
}

napi_status napi_get_element(napi_env env, napi_value object, uint32_t index, napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, object);
  CHECK_ARG(env, result);

  JSValue jsObject = ToJSValue(object);
  JSValue jsResult = JS_GetPropertyUint32(env->context, jsObject, index);

  if (JS_IsException(jsResult)) {
    return napi_set_last_error(env, napi_generic_failure);
  }

  *result = FromJSValue(env, jsResult);
  napi_clear_last_error(env);
  return napi_ok;
}

napi_status napi_set_element(napi_env env, napi_value object, uint32_t index, napi_value value) {
  CHECK_ENV(env);
  CHECK_ARG(env, object);
  CHECK_ARG(env, value);

  JSValue jsObject = ToJSValue(object);
  JSValue jsValue = ToJSValue(value);

  if (JS_SetPropertyUint32(env->context, jsObject, index, JS_DupValue(env->context, jsValue)) < 0) {
    return napi_set_last_error(env, napi_generic_failure);
  }

  napi_clear_last_error(env);
  return napi_ok;
}

napi_status napi_has_element(napi_env env, napi_value object, uint32_t index, bool* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, object);
  CHECK_ARG(env, result);

  JSValue jsObject = ToJSValue(object);
  JSAtom atom = JS_NewAtomUInt32(env->context, index);
  
  int has = JS_HasProperty(env->context, jsObject, atom);
  JS_FreeAtom(env->context, atom);

  if (has < 0) {
    return napi_set_last_error(env, napi_generic_failure);
  }

  *result = (has != 0);
  napi_clear_last_error(env);
  return napi_ok;
}

napi_status napi_delete_element(napi_env env, napi_value object, uint32_t index, bool* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, object);

  JSValue jsObject = ToJSValue(object);
  JSAtom atom = JS_NewAtomUInt32(env->context, index);
  
  int deleted = JS_DeleteProperty(env->context, jsObject, atom, 0);
  JS_FreeAtom(env->context, atom);

  if (deleted < 0) {
    return napi_set_last_error(env, napi_generic_failure);
  }

  if (result != nullptr) {
    *result = (deleted != 0);
  }

  napi_clear_last_error(env);
  return napi_ok;
}

// Get property names
napi_status napi_get_property_names(napi_env env, napi_value object, napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, object);
  CHECK_ARG(env, result);
  
  JSValue jsObject = ToJSValue(object);
  
  JSPropertyEnum* ptab;
  uint32_t plen;
  
  if (JS_GetOwnPropertyNames(env->context, &ptab, &plen, jsObject, 
      JS_GPN_STRING_MASK | JS_GPN_ENUM_ONLY) < 0) {
    return napi_set_last_error(env, napi_generic_failure);
  }
  
  JSValue arr = JS_NewArray(env->context);
  
  for (uint32_t i = 0; i < plen; i++) {
    JSValue name = JS_AtomToString(env->context, ptab[i].atom);
    JS_SetPropertyUint32(env->context, arr, i, name);
  }
  
  JS_FreePropertyEnum(env->context, ptab, plen);
  
  *result = FromJSValue(env, arr);
  napi_clear_last_error(env);
  return napi_ok;
}

// Define properties
napi_status napi_define_properties(napi_env env, napi_value object, size_t property_count, const napi_property_descriptor* properties) {
  CHECK_ENV(env);
  if (property_count > 0) {
    CHECK_ARG(env, properties);
  }
  
  JSValue jsObject = ToJSValue(object);
  
  for (size_t i = 0; i < property_count; i++) {
    const napi_property_descriptor* p = &properties[i];
    
    JSAtom atom;
    if (p->utf8name != nullptr) {
      atom = JS_NewAtom(env->context, p->utf8name);
    } else {
      atom = JS_ValueToAtom(env->context, ToJSValue(p->name));
    }
    
    if (atom == JS_ATOM_NULL) {
      return napi_set_last_error(env, napi_name_expected);
    }
    
    int flags = ToQuickJSPropertyFlags(p->attributes);
    
    if (p->getter != nullptr || p->setter != nullptr) {
      // Accessor property
      JSValue getter = JS_UNDEFINED;
      JSValue setter = JS_UNDEFINED;
      
      if (p->getter != nullptr) {
        napi_value getterFunc;
        napi_status status = napi_create_function(env, nullptr, 0, p->getter, p->data, &getterFunc);
        if (status != napi_ok) {
          JS_FreeAtom(env->context, atom);
          return status;
        }
        getter = JS_DupValue(env->context, ToJSValue(getterFunc));
      }
      
      if (p->setter != nullptr) {
        napi_value setterFunc;
        napi_status status = napi_create_function(env, nullptr, 0, p->setter, p->data, &setterFunc);
        if (status != napi_ok) {
          JS_FreeValue(env->context, getter);
          JS_FreeAtom(env->context, atom);
          return status;
        }
        setter = JS_DupValue(env->context, ToJSValue(setterFunc));
      }
      
      flags |= JS_PROP_HAS_GET | JS_PROP_HAS_SET | JS_PROP_HAS_CONFIGURABLE | JS_PROP_HAS_ENUMERABLE;
      JS_DefinePropertyGetSet(env->context, jsObject, atom, getter, setter, flags);
    } else if (p->method != nullptr) {
      // Method
      napi_value methodFunc;
      napi_status status = napi_create_function(env, p->utf8name, NAPI_AUTO_LENGTH, p->method, p->data, &methodFunc);
      if (status != napi_ok) {
        JS_FreeAtom(env->context, atom);
        return status;
      }
      
      flags |= JS_PROP_HAS_VALUE | JS_PROP_HAS_WRITABLE | JS_PROP_HAS_CONFIGURABLE | JS_PROP_HAS_ENUMERABLE;
      JS_DefinePropertyValue(env->context, jsObject, atom, JS_DupValue(env->context, ToJSValue(methodFunc)), flags);
    } else if (p->value != nullptr) {
      // Data property
      flags |= JS_PROP_HAS_VALUE | JS_PROP_HAS_WRITABLE | JS_PROP_HAS_CONFIGURABLE | JS_PROP_HAS_ENUMERABLE;
      JS_DefinePropertyValue(env->context, jsObject, atom, JS_DupValue(env->context, ToJSValue(p->value)), flags);
    }
    
    JS_FreeAtom(env->context, atom);
  }
  
  napi_clear_last_error(env);
  return napi_ok;
}

// Call function
napi_status napi_call_function(napi_env env, napi_value recv, napi_value func, size_t argc, const napi_value* argv, napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, recv);
  CHECK_ARG(env, func);
  if (argc > 0) {
    CHECK_ARG(env, argv);
  }

  JSValue jsRecv = ToJSValue(recv);
  JSValue jsFunc = ToJSValue(func);
  
  std::vector<JSValueConst> args;
  args.reserve(argc);
  for (size_t i = 0; i < argc; i++) {
    args.push_back(ToJSValue(argv[i]));
  }

  JSValue jsResult = JS_Call(env->context, jsFunc, jsRecv, static_cast<int>(argc), args.data());

  if (JS_IsException(jsResult)) {
    if (result != nullptr) {
      static JSValue undefined = JS_UNDEFINED;
      *result = reinterpret_cast<napi_value>(&undefined);
    }
    return napi_set_last_error(env, napi_pending_exception);
  }

  if (result != nullptr) {
    *result = FromJSValue(env, jsResult);
  } else {
    JS_FreeValue(env->context, jsResult);
  }

  napi_clear_last_error(env);
  return napi_ok;
}

// New instance
napi_status napi_new_instance(napi_env env, napi_value constructor, size_t argc, const napi_value* argv, napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, constructor);
  if (argc > 0) {
    CHECK_ARG(env, argv);
  }
  CHECK_ARG(env, result);
  
  JSValue jsCtor = ToJSValue(constructor);
  
  std::vector<JSValueConst> args;
  args.reserve(argc);
  for (size_t i = 0; i < argc; i++) {
    args.push_back(ToJSValue(argv[i]));
  }
  
  JSValue jsResult = JS_CallConstructor(env->context, jsCtor, static_cast<int>(argc), args.data());
  
  if (JS_IsException(jsResult)) {
    return napi_set_last_error(env, napi_pending_exception);
  }
  
  *result = FromJSValue(env, jsResult);
  napi_clear_last_error(env);
  return napi_ok;
}

// Is array
napi_status napi_is_array(napi_env env, napi_value value, bool* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, value);
  CHECK_ARG(env, result);

  JSValue jsValue = ToJSValue(value);
  *result = JS_IsArray(jsValue) != 0;

  napi_clear_last_error(env);
  return napi_ok;
}

// Throw
napi_status napi_throw(napi_env env, napi_value error) {
  CHECK_ENV(env);
  CHECK_ARG(env, error);
  
  JSValue jsError = ToJSValue(error);
  
  // Throw into the CURRENT execution context, not the stored context
  JSContext* targetCtx = env->current_context ? env->current_context : env->context;
  JS_Throw(targetCtx, JS_DupValue(targetCtx, jsError));
  
  return napi_set_last_error(env, napi_pending_exception);
}

// Throw error
napi_status napi_throw_error(napi_env env, const char* code, const char* msg) {
  CHECK_ENV(env);

  JSValue error = JS_NewError(env->context);
  if (msg) {
    JS_SetPropertyStr(env->context, error, "message", JS_NewString(env->context, msg));
  }
  if (code) {
    JS_SetPropertyStr(env->context, error, "code", JS_NewString(env->context, code));
  }

  JS_Throw(env->context, error);
  return napi_set_last_error(env, napi_pending_exception);
}

// Throw type error
napi_status napi_throw_type_error(napi_env env, const char* code, const char* msg) {
  CHECK_ENV(env);

  JS_ThrowTypeError(env->context, "%s", msg ? msg : "");
  return napi_set_last_error(env, napi_pending_exception);
}

// Throw range error
napi_status napi_throw_range_error(napi_env env, const char* code, const char* msg) {
  CHECK_ENV(env);

  JS_ThrowRangeError(env->context, "%s", msg ? msg : "");
  return napi_set_last_error(env, napi_pending_exception);
}

// Create error
napi_status napi_create_error(napi_env env, napi_value code, napi_value msg, napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, msg);
  CHECK_ARG(env, result);
  
  JSValue error = JS_NewError(env->context);
  JS_SetPropertyStr(env->context, error, "message", JS_DupValue(env->context, ToJSValue(msg)));
  
  if (code != nullptr) {
    JS_SetPropertyStr(env->context, error, "code", JS_DupValue(env->context, ToJSValue(code)));
  }
  
  *result = FromJSValue(env, error);
  napi_clear_last_error(env);
  return napi_ok;
}

// Create type error
napi_status napi_create_type_error(napi_env env, napi_value code, napi_value msg, napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, msg);
  CHECK_ARG(env, result);
  
  JSValue global = JS_GetGlobalObject(env->context);
  JSValue typeErrorCtor = JS_GetPropertyStr(env->context, global, "TypeError");
  
  JSValueConst args[1] = { ToJSValue(msg) };
  JSValue error = JS_CallConstructor(env->context, typeErrorCtor, 1, args);
  
  if (code != nullptr) {
    JS_SetPropertyStr(env->context, error, "code", JS_DupValue(env->context, ToJSValue(code)));
  }
  
  JS_FreeValue(env->context, typeErrorCtor);
  JS_FreeValue(env->context, global);
  
  *result = FromJSValue(env, error);
  napi_clear_last_error(env);
  return napi_ok;
}

// Create range error
napi_status napi_create_range_error(napi_env env, napi_value code, napi_value msg, napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, msg);
  CHECK_ARG(env, result);
  
  JSValue global = JS_GetGlobalObject(env->context);
  JSValue rangeErrorCtor = JS_GetPropertyStr(env->context, global, "RangeError");
  
  JSValueConst args[1] = { ToJSValue(msg) };
  JSValue error = JS_CallConstructor(env->context, rangeErrorCtor, 1, args);
  
  if (code != nullptr) {
    JS_SetPropertyStr(env->context, error, "code", JS_DupValue(env->context, ToJSValue(code)));
  }
  
  JS_FreeValue(env->context, rangeErrorCtor);
  JS_FreeValue(env->context, global);
  
  *result = FromJSValue(env, error);
  napi_clear_last_error(env);
  return napi_ok;
}

// Is error
napi_status napi_is_error(napi_env env, napi_value value, bool* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, value);
  CHECK_ARG(env, result);

  JSValue jsValue = ToJSValue(value);
  *result = JS_IsError(jsValue) != 0;

  napi_clear_last_error(env);
  return napi_ok;
}

// Strict equals
napi_status napi_strict_equals(napi_env env, napi_value lhs, napi_value rhs, bool* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, lhs);
  CHECK_ARG(env, rhs);
  CHECK_ARG(env, result);

  JSValue jsLhs = ToJSValue(lhs);
  JSValue jsRhs = ToJSValue(rhs);

 // *result = JS_StrictEq(env->context, jsLhs, jsRhs) != 0; // TODO
  *result = (JS_VALUE_GET_TAG(jsLhs) == JS_VALUE_GET_TAG(jsRhs)) &&
            (JS_VALUE_GET_PTR(jsLhs) == JS_VALUE_GET_PTR(jsRhs));
  
  // For non-pointer types (numbers, bools), compare by value
  if (!*result) {
    if (JS_IsNumber(jsLhs) && JS_IsNumber(jsRhs)) {
      double a, b;
      JS_ToFloat64(env->context, &a, jsLhs);
      JS_ToFloat64(env->context, &b, jsRhs);
      *result = (a == b);
    } else if (JS_IsString(jsLhs) && JS_IsString(jsRhs)) {
      const char* a = JS_ToCString(env->context, jsLhs);
      const char* b = JS_ToCString(env->context, jsRhs);
      *result = (a && b && strcmp(a, b) == 0);
      JS_FreeCString(env->context, a);
      JS_FreeCString(env->context, b);
    }
  }

  napi_clear_last_error(env);
  return napi_ok;
}

// Create symbol
napi_status napi_create_symbol(napi_env env, napi_value description, napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);
  
  JSValue global = JS_GetGlobalObject(env->context);
  JSValue symbolCtor = JS_GetPropertyStr(env->context, global, "Symbol");
  
  JSValue symbol;
  if (description != nullptr) {
    JSValueConst args[1] = { ToJSValue(description) };
    symbol = JS_Call(env->context, symbolCtor, JS_UNDEFINED, 1, args);
  } else {
    symbol = JS_Call(env->context, symbolCtor, JS_UNDEFINED, 0, nullptr);
  }
  
  JS_FreeValue(env->context, symbolCtor);
  JS_FreeValue(env->context, global);
  
  if (JS_IsException(symbol)) {
    return napi_set_last_error(env, napi_generic_failure);
  }
  
  *result = FromJSValue(env, symbol);
  napi_clear_last_error(env);
  return napi_ok;
}

// Reference management
napi_status napi_create_reference(napi_env env, napi_value value, uint32_t initial_refcount, napi_ref* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, value);
  CHECK_ARG(env, result);

  JSValue jsValue = ToJSValue(value);
  // A reference with initial_refcount == 0 is a WEAK reference: it must not
  // keep the JS value alive. Only the count > 0 (strong) case dups. Otherwise
  // the reference self-pins the value (e.g. every ObjectWrap instance would
  // leak forever because napi_wrap creates a weak self-reference).
  JSValue stored = (initial_refcount == 0)
                       ? jsValue
                       : JS_DupValue(env->context, jsValue);
  RefInfo* info = new RefInfo{ stored, initial_refcount };

  // Track for env-scoped cleanup (see Napi::Detach).
  env->refs_list.push_back(info);

  *result = reinterpret_cast<napi_ref>(info);
  napi_clear_last_error(env);
  return napi_ok;
}

napi_status napi_delete_reference(napi_env env, napi_ref ref) {
  CHECK_ENV(env);
  CHECK_ARG(env, ref);

  RefInfo* info = reinterpret_cast<RefInfo*>(ref);
  if (env->detached) {
    // Detach already released the JS side and cleared the tracking
    // list; all we have to do is free the RefInfo allocation itself.
    delete info;
    return napi_ok;
  }
  // Only a strong reference (count > 0) owns a dup that must be freed.
  // A weak reference does not own the JS value; freeing it here would be a
  // spurious decrement and could even touch an already-finalized value.
  if (info->count > 0) {
    JS_FreeValue(env->context, info->value);
  }
  // Remove from env tracking.
  auto& list = env->refs_list;
  for (auto it = list.begin(); it != list.end(); ++it) {
    if (*it == info) {
      list.erase(it);
      break;
    }
  }
  delete info;

  napi_clear_last_error(env);
  return napi_ok;
}

napi_status napi_reference_ref(napi_env env, napi_ref ref, uint32_t* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, ref);

  RefInfo* info = reinterpret_cast<RefInfo*>(ref);
  // 0 -> 1 transition: promote to strong by taking a dup. The caller is
  // responsible for ensuring the underlying value is still alive at this
  // point (weak references do not keep the value alive).
  if (info->count == 0) {
    info->value = JS_DupValue(env->context, info->value);
  }
  info->count++;

  if (result != nullptr) {
    *result = info->count;
  }

  napi_clear_last_error(env);
  return napi_ok;
}

napi_status napi_reference_unref(napi_env env, napi_ref ref, uint32_t* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, ref);

  RefInfo* info = reinterpret_cast<RefInfo*>(ref);
  if (info->count > 0) {
    info->count--;
    // 1 -> 0 transition: demote to weak by releasing the dup we owned. The
    // raw JSValue is retained (unowned) so that a subsequent ref can dup it
    // again, but we no longer keep the value alive.
    if (info->count == 0) {
      JSValue v = info->value;
      JS_FreeValue(env->context, v);
      // Leave info->value as the (now unowned) raw tag/pointer; do not touch.
    }
  }

  if (result != nullptr) {
    *result = info->count;
  }

  napi_clear_last_error(env);
  return napi_ok;
}

napi_status napi_get_reference_value(napi_env env, napi_ref ref, napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, ref);
  CHECK_ARG(env, result);
  
  RefInfo* info = reinterpret_cast<RefInfo*>(ref);
  
  if (info->count == 0) {
    *result = nullptr;
  } else {
    *result = FromJSValue(env, JS_DupValue(env->context, info->value));
  }
  
  napi_clear_last_error(env);
  return napi_ok;
}

// Handle scopes - QuickJS uses reference counting, so these are mostly no-ops
napi_status napi_open_handle_scope(napi_env env, napi_handle_scope* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);
  
  env->current_scope_start = env->handle_scope_stack.size();
  *result = reinterpret_cast<napi_handle_scope>(env->current_scope_start + 1);
  
  napi_clear_last_error(env);
  return napi_ok;
}

napi_status napi_close_handle_scope(napi_env env, napi_handle_scope scope) {
  CHECK_ENV(env);
  CHECK_ARG(env, scope);
  
  // Free all JSValues created in this scope
  size_t scope_start = reinterpret_cast<size_t>(scope) - 1;
  
  // Call JS_FreeValue on all values in scope
  for (size_t i = scope_start; i < env->handle_scope_stack.size(); i++) {
    JS_FreeValue(env->context, *env->handle_scope_stack[i]);
  }
  
  // Remove from stack
  env->handle_scope_stack.resize(scope_start);
  env->current_scope_start = scope_start;
  
  napi_clear_last_error(env);
  return napi_ok;
}

// Escapeable handle scopes - similar to regular handle scopes for QuickJS
napi_status napi_open_escapable_handle_scope(napi_env env, napi_escapable_handle_scope* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);
  
  // Same as regular handle scope for QuickJS
  env->current_scope_start = env->handle_scope_stack.size();
  *result = reinterpret_cast<napi_escapable_handle_scope>(env->current_scope_start + 1);
  
  napi_clear_last_error(env);
  return napi_ok;
}

napi_status napi_close_escapable_handle_scope(napi_env env, napi_escapable_handle_scope scope) {
  CHECK_ENV(env);
  CHECK_ARG(env, scope);
  
  // Same cleanup as regular handle scope
  size_t scope_start = reinterpret_cast<size_t>(scope) - 1;
  
  for (size_t i = scope_start; i < env->handle_scope_stack.size(); i++) {
    JS_FreeValue(env->context, *env->handle_scope_stack[i]);
  }
  
  env->handle_scope_stack.resize(scope_start);
  env->current_scope_start = scope_start;
  
  napi_clear_last_error(env);
  return napi_ok;
}

napi_status napi_escape_handle(napi_env env, napi_escapable_handle_scope scope, napi_value escapee, napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, scope);
  CHECK_ARG(env, escapee);
  CHECK_ARG(env, result);
  
  // Get the scope start index
  size_t scope_start = reinterpret_cast<size_t>(scope) - 1;
  
  // Duplicate the JSValue to create a new handle that will outlive the current scope
  JSValue jsValue = ToJSValue(escapee);
  JSValue escapedValue = JS_DupValue(env->context, jsValue);
  
  // Store the escaped value in the parent scope (before scope_start)
  auto parentPtr = std::make_unique<JSValue>(escapedValue);
  napi_value parentHandle = reinterpret_cast<napi_value>(parentPtr.get());
  
  // Insert at parent scope position (before current scope)
  if (scope_start > 0) {
    env->handle_scope_stack.insert(
      env->handle_scope_stack.begin() + scope_start,
      std::move(parentPtr)
    );
    
    // Note: Inserting shifts indices, but since we're inserting at scope_start,
    // the current scope's start index is now scope_start + 1
    // We need to update current_scope_start if it was pointing to this scope
    if (env->current_scope_start == scope_start) {
      env->current_scope_start = scope_start + 1;
    }
  } else {
    // No parent scope - just add to the beginning
    env->handle_scope_stack.insert(
      env->handle_scope_stack.begin(),
      std::move(parentPtr)
    );
    
    if (env->current_scope_start == 0) {
      env->current_scope_start = 1;
    }
  }
  
  *result = parentHandle;
  napi_clear_last_error(env);
  return napi_ok;
}

// ArrayBuffer support
napi_status napi_is_arraybuffer(napi_env env, napi_value value, bool* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, value);
  CHECK_ARG(env, result);
  
  JSValue jsValue = ToJSValue(value);
  *result = JS_IsArrayBuffer(jsValue);
  
  napi_clear_last_error(env);
  return napi_ok;
}

napi_status napi_create_arraybuffer(napi_env env, size_t byte_length, void** data, napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);
  
  JSValue arrayBuffer = JS_NewArrayBufferCopy(env->context, nullptr, byte_length);
  
  if (JS_IsException(arrayBuffer)) {
    return napi_set_last_error(env, napi_generic_failure);
  }
  
  if (data != nullptr) {
    size_t size;
    *data = JS_GetArrayBuffer(env->context, &size, arrayBuffer);
  }
  
  *result = FromJSValue(env, arrayBuffer);
  napi_clear_last_error(env);
  return napi_ok;
}

namespace {
  void ArrayBufferFreeCallback(JSRuntime* rt, void* opaque, void* ptr) {
    ExternalData* externalData = reinterpret_cast<ExternalData*>(opaque);
    if (externalData != nullptr) {
      // Invoke via the class finalizer which calls _cb properly
      ExternalData::RunCallback(externalData);
      delete externalData;
    }
  }
}

napi_status napi_create_external_arraybuffer(napi_env env, void* external_data, size_t byte_length, napi_finalize finalize_cb, void* finalize_hint, napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);
  
  ExternalData* externalDataInfo = new ExternalData(env, external_data, finalize_cb, finalize_hint);
  
  JSValue arrayBuffer = JS_NewArrayBuffer(env->context, 
                                          reinterpret_cast<uint8_t*>(external_data), 
                                          byte_length, 
                                          ArrayBufferFreeCallback, 
                                          externalDataInfo, 
                                          0);
  
  if (JS_IsException(arrayBuffer)) {
    delete externalDataInfo;
    return napi_set_last_error(env, napi_generic_failure);
  }
  
  *result = FromJSValue(env, arrayBuffer);
  napi_clear_last_error(env);
  return napi_ok;
}

napi_status napi_get_arraybuffer_info(napi_env env, napi_value arraybuffer, void** data, size_t* byte_length) {
  CHECK_ENV(env);
  CHECK_ARG(env, arraybuffer);
  
  JSValue jsArrayBuffer = ToJSValue(arraybuffer);
  size_t size;
  uint8_t* bufferData = JS_GetArrayBuffer(env->context, &size, jsArrayBuffer);
  
  if (data != nullptr) {
    *data = bufferData;
  }
  
  if (byte_length != nullptr) {
    *byte_length = size;
  }
  
  napi_clear_last_error(env);
  return napi_ok;
}

// TypedArray support
napi_status napi_is_typedarray(napi_env env, napi_value value, bool* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, value);
  CHECK_ARG(env, result);
  
  JSValue jsValue = ToJSValue(value);
  *result = JS_GetTypedArrayType(jsValue) != -1;
  
  napi_clear_last_error(env);
  return napi_ok;
}

// TypedArray support
napi_status napi_create_typedarray(napi_env env, napi_typedarray_type type, size_t length, napi_value arraybuffer, size_t byte_offset, napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, arraybuffer);
  CHECK_ARG(env, result);
  
  JSTypedArrayEnum jsType;
  switch (type) {
    case napi_int8_array: jsType = JS_TYPED_ARRAY_INT8; break;
    case napi_uint8_array: jsType = JS_TYPED_ARRAY_UINT8; break;
    case napi_uint8_clamped_array: jsType = JS_TYPED_ARRAY_UINT8C; break;
    case napi_int16_array: jsType = JS_TYPED_ARRAY_INT16; break;
    case napi_uint16_array: jsType = JS_TYPED_ARRAY_UINT16; break;
    case napi_int32_array: jsType = JS_TYPED_ARRAY_INT32; break;
    case napi_uint32_array: jsType = JS_TYPED_ARRAY_UINT32; break;
    case napi_float32_array: jsType = JS_TYPED_ARRAY_FLOAT32; break;
    case napi_float64_array: jsType = JS_TYPED_ARRAY_FLOAT64; break;
    case napi_bigint64_array: jsType = JS_TYPED_ARRAY_BIG_INT64; break;
    case napi_biguint64_array: jsType = JS_TYPED_ARRAY_BIG_UINT64; break;
    default:
      return napi_set_last_error(env, napi_invalid_arg);
  }
  
  JSValue jsArrayBuffer = ToJSValue(arraybuffer);
  
  // Create temporary JSValue arguments
  JSValue offsetVal = JS_NewInt64(env->context, byte_offset);
  JSValue lengthVal = JS_NewInt64(env->context, length);
  
  JSValueConst args[3] = { jsArrayBuffer, offsetVal, lengthVal };
  JSValue typedArray = JS_NewTypedArray(env->context, 3, args, jsType);
  
  // Free temporary values
  JS_FreeValue(env->context, offsetVal);
  JS_FreeValue(env->context, lengthVal);
  
  if (JS_IsException(typedArray)) {
    return napi_set_last_error(env, napi_generic_failure);
  }
  
  *result = FromJSValue(env, typedArray);
  napi_clear_last_error(env);
  return napi_ok;
}

napi_status napi_create_dataview(napi_env env, size_t byte_length, napi_value arraybuffer, size_t byte_offset, napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, arraybuffer);
  CHECK_ARG(env, result);
  
  JSValue global = JS_GetGlobalObject(env->context);
  JSValue dataViewCtor = JS_GetPropertyStr(env->context, global, "DataView");
  
  JSValue jsArrayBuffer = ToJSValue(arraybuffer);
  
  // Create temporary JSValue arguments
  JSValue offsetVal = JS_NewInt64(env->context, byte_offset);
  JSValue lengthVal = JS_NewInt64(env->context, byte_length);
  
  JSValueConst args[3] = { jsArrayBuffer, offsetVal, lengthVal };
  JSValue dataView = JS_CallConstructor(env->context, dataViewCtor, 3, args);
  
  // Free temporary values
  JS_FreeValue(env->context, offsetVal);
  JS_FreeValue(env->context, lengthVal);
  JS_FreeValue(env->context, dataViewCtor);
  JS_FreeValue(env->context, global);
  
  if (JS_IsException(dataView)) {
    return napi_set_last_error(env, napi_generic_failure);
  }
  
  *result = FromJSValue(env, dataView);
  napi_clear_last_error(env);
  return napi_ok;
}

napi_status napi_get_typedarray_info(napi_env env, napi_value typedarray, napi_typedarray_type* type, size_t* length, void** data, napi_value* arraybuffer, size_t* byte_offset) {
  CHECK_ENV(env);
  CHECK_ARG(env, typedarray);
  
  JSValue jsTypedArray = ToJSValue(typedarray);
  
  size_t offset, byteLength, bytesPerElement;
  JSValue buffer = JS_GetTypedArrayBuffer(env->context, jsTypedArray, &offset, &byteLength, &bytesPerElement);
  
  if (JS_IsException(buffer)) {
    return napi_set_last_error(env, napi_generic_failure);
  }
  
  if (byte_offset != nullptr) {
    *byte_offset = offset;
  }
  
  if (length != nullptr) {
    *length = byteLength / bytesPerElement;
  }
  
  if (data != nullptr) {
    size_t bufferSize;
    uint8_t* bufferData = JS_GetArrayBuffer(env->context, &bufferSize, buffer);
    *data = bufferData + offset;
  }
  
  if (type != nullptr) {
    int taType = JS_GetTypedArrayType(jsTypedArray);
    switch (taType) {
      case JS_TYPED_ARRAY_INT8: *type = napi_int8_array; break;
      case JS_TYPED_ARRAY_UINT8: *type = napi_uint8_array; break;
      case JS_TYPED_ARRAY_UINT8C: *type = napi_uint8_clamped_array; break;
      case JS_TYPED_ARRAY_INT16: *type = napi_int16_array; break;
      case JS_TYPED_ARRAY_UINT16: *type = napi_uint16_array; break;
      case JS_TYPED_ARRAY_INT32: *type = napi_int32_array; break;
      case JS_TYPED_ARRAY_UINT32: *type = napi_uint32_array; break;
      case JS_TYPED_ARRAY_FLOAT32: *type = napi_float32_array; break;
      case JS_TYPED_ARRAY_FLOAT64: *type = napi_float64_array; break;
      case JS_TYPED_ARRAY_BIG_INT64: *type = napi_bigint64_array; break;
      case JS_TYPED_ARRAY_BIG_UINT64: *type = napi_biguint64_array; break;
      default: *type = napi_uint8_array; break;
    }
  }
  
  if (arraybuffer != nullptr) {
    *arraybuffer = FromJSValue(env, buffer);
  } else {
    JS_FreeValue(env->context, buffer);
  }
  
  napi_clear_last_error(env);
  return napi_ok;
}

// DataView support
napi_status napi_is_dataview(napi_env env, napi_value value, bool* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, value);
  CHECK_ARG(env, result);
  
  // QuickJS doesn't have a direct way to check DataView, so use instanceof
  JSValue jsValue = ToJSValue(value);
  JSValue global = JS_GetGlobalObject(env->context);
  JSValue dataViewCtor = JS_GetPropertyStr(env->context, global, "DataView");
  
  int ret = JS_IsInstanceOf(env->context, jsValue, dataViewCtor);
  *result = (ret > 0);
  
  JS_FreeValue(env->context, dataViewCtor);
  JS_FreeValue(env->context, global);
  
  napi_clear_last_error(env);
  return napi_ok;
}

napi_status napi_get_dataview_info(napi_env env, napi_value dataview, size_t* byte_length, void** data, napi_value* arraybuffer, size_t* byte_offset) {
  CHECK_ENV(env);
  CHECK_ARG(env, dataview);
  
  JSValue jsDataView = ToJSValue(dataview);
  
  // Get buffer property
  JSValue buffer = JS_GetPropertyStr(env->context, jsDataView, "buffer");
  JSValue byteOffsetVal = JS_GetPropertyStr(env->context, jsDataView, "byteOffset");
  JSValue byteLengthVal = JS_GetPropertyStr(env->context, jsDataView, "byteLength");
  
  if (byte_offset != nullptr) {
    int64_t offset;
    JS_ToInt64(env->context, &offset, byteOffsetVal);
    *byte_offset = static_cast<size_t>(offset);
  }
  
  if (byte_length != nullptr) {
    int64_t length;
    JS_ToInt64(env->context, &length, byteLengthVal);
    *byte_length = static_cast<size_t>(length);
  }
  
  if (arraybuffer != nullptr) {
    *arraybuffer = FromJSValue(env, JS_DupValue(env->context, buffer));
  }
  
  if (data != nullptr) {
    size_t bufferSize;
    uint8_t* bufferData = JS_GetArrayBuffer(env->context, &bufferSize, buffer);
    int64_t offset;
    JS_ToInt64(env->context, &offset, byteOffsetVal);
    *data = bufferData + offset;
  }
  
  JS_FreeValue(env->context, buffer);
  JS_FreeValue(env->context, byteOffsetVal);
  JS_FreeValue(env->context, byteLengthVal);
  
  napi_clear_last_error(env);
  return napi_ok;
}

// Version
napi_status napi_get_version(napi_env env, uint32_t* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);
  *result = NAPI_VERSION;
  napi_clear_last_error(env);
  return napi_ok;
}

// Promise support
napi_status napi_create_promise(napi_env env, napi_deferred* deferred, napi_value* promise) {
  CHECK_ENV(env);
  CHECK_ARG(env, deferred);
  CHECK_ARG(env, promise);
  
  JSValue resolving_funcs[2];
  JSValue jsPromise = JS_NewPromiseCapability(env->context, resolving_funcs);
  
  if (JS_IsException(jsPromise)) {
    return napi_set_last_error(env, napi_generic_failure);
  }
  
  // Store resolve/reject functions in a container object
  JSValue container = JS_NewObject(env->context);
  JS_SetPropertyStr(env->context, container, "resolve", resolving_funcs[0]);
  JS_SetPropertyStr(env->context, container, "reject", resolving_funcs[1]);
  
  // Create reference directly from container WITHOUT going through FromJSValue
  // to avoid double-ownership (handle scope + reference)
  RefInfo* refInfo = new RefInfo{ container, 1 }; // Use refcount 1 for immediate access
  napi_ref ref = reinterpret_cast<napi_ref>(refInfo);
  
  *deferred = reinterpret_cast<napi_deferred>(ref);
  *promise = FromJSValue(env, jsPromise);
  
  napi_clear_last_error(env);
  return napi_ok;
}

napi_status napi_resolve_deferred(napi_env env, napi_deferred deferred, napi_value resolution) {
  CHECK_ENV(env);
  CHECK_ARG(env, deferred);
  CHECK_ARG(env, resolution);
  
  napi_ref ref = reinterpret_cast<napi_ref>(deferred);
  napi_value container;
  CHECK_NAPI(napi_get_reference_value(env, ref, &container));
  
  if (container == nullptr) {
    return napi_set_last_error(env, napi_invalid_arg);
  }
  
  napi_value resolve;
  CHECK_NAPI(napi_get_named_property(env, container, "resolve", &resolve));
  
  napi_value undefined;
  CHECK_NAPI(napi_get_undefined(env, &undefined));
  CHECK_NAPI(napi_call_function(env, undefined, resolve, 1, &resolution, nullptr));
  
  // Clean up the reference
  CHECK_NAPI(napi_delete_reference(env, ref));
  
  return napi_ok;
}

napi_status napi_reject_deferred(napi_env env, napi_deferred deferred, napi_value rejection) {
  CHECK_ENV(env);
  CHECK_ARG(env, deferred);
  CHECK_ARG(env, rejection);
  
  napi_ref ref = reinterpret_cast<napi_ref>(deferred);
  napi_value container;
  CHECK_NAPI(napi_get_reference_value(env, ref, &container));
  
  if (container == nullptr) {
    return napi_set_last_error(env, napi_invalid_arg);
  }
  
  napi_value reject;
  CHECK_NAPI(napi_get_named_property(env, container, "reject", &reject));
  
  napi_value undefined;
  CHECK_NAPI(napi_get_undefined(env, &undefined));
  CHECK_NAPI(napi_call_function(env, undefined, reject, 1, &rejection, nullptr));
  
  // Clean up the reference
  CHECK_NAPI(napi_delete_reference(env, ref));
  
  return napi_ok;
}

napi_status napi_is_promise(napi_env env, napi_value value, bool* is_promise) {
  CHECK_ENV(env);
  CHECK_ARG(env, value);
  CHECK_ARG(env, is_promise);
  
  napi_value global, promise_ctor;
  CHECK_NAPI(napi_get_global(env, &global));
  CHECK_NAPI(napi_get_named_property(env, global, "Promise", &promise_ctor));
  CHECK_NAPI(napi_instanceof(env, value, promise_ctor, is_promise));
  
  return napi_ok;
}

// Script execution
napi_status napi_run_script(napi_env env, napi_value script, napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, script);
  CHECK_ARG(env, result);
  
  JSValue jsScript = ToJSValue(script);
  
  size_t len;
  const char* str = JS_ToCStringLen(env->context, &len, jsScript);
  if (!str) {
    return napi_set_last_error(env, napi_string_expected);
  }
  
  JSValue jsResult = JS_Eval(env->context, str, len, "<script>", JS_EVAL_TYPE_GLOBAL);
  JS_FreeCString(env->context, str);
  
  if (JS_IsException(jsResult)) {
    return napi_set_last_error(env, napi_pending_exception);
  }
  
  *result = FromJSValue(env, jsResult);
  napi_clear_last_error(env);
  return napi_ok;
}

napi_status napi_run_script(napi_env env, napi_value script, const char* source_url, napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, script);
  CHECK_ARG(env, result);
  
  JSValue jsScript = ToJSValue(script);
  
  size_t len;
  const char* str = JS_ToCStringLen(env->context, &len, jsScript);
  if (!str) {
    return napi_set_last_error(env, napi_string_expected);
  }
  
  JSValue jsResult = JS_Eval(env->context, str, len, source_url ? source_url : "<script>", JS_EVAL_TYPE_GLOBAL);
  JS_FreeCString(env->context, str);
  
  if (JS_IsException(jsResult)) {
    return napi_set_last_error(env, napi_pending_exception);
  }
  
  *result = FromJSValue(env, jsResult);
  napi_clear_last_error(env);
  return napi_ok;
}

// Wrap/Unwrap for native objects
//
// Instances created by a napi class constructor carry the js_wrap_class_id
// class (see the constructor trampoline in ExternalCallback::Callback), which
// gives them an opaque slot plus the NapiWrap finalizer. In that case (the only
// one exercised in this codebase, since node-addon-api's ObjectWrap always
// wraps the constructor's `this`) we store the native pointer directly on the
// instance, so nothing is observable from JS: the prototype is untouched and no
// own-property is added. The prototype-chain fallback below only runs for the
// theoretical case of wrapping an object we did not construct.
napi_status napi_wrap(napi_env env, napi_value js_object, void* native_object, napi_finalize finalize_cb, void* finalize_hint, napi_ref* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, js_object);
  
  JSRuntime* rt = JS_GetRuntime(env->context);
  InitClassIds(rt);
  
  JSValue jsObject = ToJSValue(js_object);
  
  ExternalData* externalData = new ExternalData(env, native_object, finalize_cb, finalize_hint);
  
  if (JS_GetClassID(jsObject) == js_wrap_class_id) {
    // Fast path: the instance itself owns the opaque slot. Reject a double wrap
    // rather than leaking the previously stored ExternalData.
    if (JS_GetOpaque(jsObject, js_wrap_class_id) != nullptr) {
      delete externalData;
      return napi_set_last_error(env, napi_invalid_arg);
    }
    JS_SetOpaque(jsObject, externalData);
  } else {
    // Fallback: create a wrapper object carrying the opaque slot and splice it
    // into the prototype chain. Not reached by ObjectWrap-based classes.
    JSValue wrapper = JS_NewObjectClass(env->context, js_wrap_class_id);
    JS_SetOpaque(wrapper, externalData);

    JSValue prototype = JS_GetPrototype(env->context, jsObject);
    JS_SetPrototype(env->context, wrapper, prototype);
    JS_SetPrototype(env->context, jsObject, wrapper);

    JS_FreeValue(env->context, wrapper);
    JS_FreeValue(env->context, prototype);
  }
  
  if (result != nullptr) {
    CHECK_NAPI(napi_create_reference(env, js_object, 0, result));
  }
  
  napi_clear_last_error(env);
  return napi_ok;
}

napi_status napi_unwrap(napi_env env, napi_value js_object, void** result) {
  CHECK_ENV(env);
  CHECK_ARG(env, js_object);
  CHECK_ARG(env, result);
  
  JSValue jsObject = ToJSValue(js_object);
  
  // Fast path: native pointer stored on the instance itself.
  if (JS_GetClassID(jsObject) == js_wrap_class_id) {
    ExternalData* externalData = reinterpret_cast<ExternalData*>(JS_GetOpaque(jsObject, js_wrap_class_id));
    *result = externalData ? externalData->Data() : nullptr;
    napi_clear_last_error(env);
    return napi_ok;
  }

  // Fallback: search the prototype chain for a legacy wrapper object.
  JSValue current = JS_GetPrototype(env->context, jsObject);
  
  while (!JS_IsNull(current)) {
    if (JS_GetClassID(current) == js_wrap_class_id) {
      ExternalData* externalData = reinterpret_cast<ExternalData*>(JS_GetOpaque(current, js_wrap_class_id));
      *result = externalData ? externalData->Data() : nullptr;
      JS_FreeValue(env->context, current);
      napi_clear_last_error(env);
      return napi_ok;
    }
    
    JSValue proto = JS_GetPrototype(env->context, current);
    JS_FreeValue(env->context, current);
    current = proto;
  }
  
  JS_FreeValue(env->context, current); // Free the final JS_NULL value
  *result = nullptr;
  return napi_set_last_error(env, napi_invalid_arg);
}

napi_status napi_remove_wrap(napi_env env, napi_value js_object, void** result) {
  CHECK_ENV(env);
  CHECK_ARG(env, js_object);

  JSValue jsObject = ToJSValue(js_object);

  // Fast path: the instance owns the opaque slot. Detach the finalizer by
  // clearing the opaque and deleting the adapter directly (without running the
  // user finalize callback) so a later GC does not run it on native memory that
  // is being handed back or has already been freed (e.g. an ObjectWrap
  // constructor that threw during stack unwinding).
  if (JS_GetClassID(jsObject) == js_wrap_class_id) {
    ExternalData* externalData = reinterpret_cast<ExternalData*>(JS_GetOpaque(jsObject, js_wrap_class_id));
    if (result != nullptr) {
      *result = externalData ? externalData->Data() : nullptr;
    }
    JS_SetOpaque(jsObject, nullptr);
    delete externalData;
    napi_clear_last_error(env);
    return napi_ok;
  }

  // Fallback: walk the prototype chain looking for a legacy wrapper object.
  // `parent` is the object whose prototype is `current`, so once the wrapper is
  // found it can be spliced out of the chain.
  JSValue parent = JS_DupValue(env->context, jsObject);
  JSValue current = JS_GetPrototype(env->context, jsObject);

  while (!JS_IsNull(current)) {
    if (JS_GetClassID(current) == js_wrap_class_id) {
      ExternalData* externalData = reinterpret_cast<ExternalData*>(JS_GetOpaque(current, js_wrap_class_id));
      if (result != nullptr) {
        *result = externalData ? externalData->Data() : nullptr;
      }

      JS_SetOpaque(current, nullptr);

      JSValue wrapperProto = JS_GetPrototype(env->context, current);
      JS_SetPrototype(env->context, parent, wrapperProto);
      JS_FreeValue(env->context, wrapperProto);

      delete externalData;

      JS_FreeValue(env->context, current);
      JS_FreeValue(env->context, parent);
      napi_clear_last_error(env);
      return napi_ok;
    }

    JS_FreeValue(env->context, parent);
    parent = current; // transfer ownership
    current = JS_GetPrototype(env->context, parent);
  }

  JS_FreeValue(env->context, parent);
  JS_FreeValue(env->context, current); // Free the final JS_NULL value
  if (result != nullptr) {
    *result = nullptr;
  }
  return napi_set_last_error(env, napi_invalid_arg);
}

// External values
napi_status napi_create_external(napi_env env, void* data, napi_finalize finalize_cb, void* finalize_hint, napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);
  
  JSRuntime* rt = JS_GetRuntime(env->context);
  InitClassIds(rt);
  
  ExternalData* externalData = new ExternalData(env, data, finalize_cb, finalize_hint);
  
  JSValue external = JS_NewObjectClass(env->context, js_external_class_id);
  JS_SetOpaque(external, externalData);
  
  *result = FromJSValue(env, external);
  napi_clear_last_error(env);
  return napi_ok;
}

napi_status napi_get_value_external(napi_env env, napi_value value, void** result) {
  CHECK_ENV(env);
  CHECK_ARG(env, value);
  CHECK_ARG(env, result);
  
  JSValue jsValue = ToJSValue(value);
  
  ExternalData* externalData = reinterpret_cast<ExternalData*>(JS_GetOpaque(jsValue, js_external_class_id));
  *result = externalData ? externalData->Data() : nullptr;
  
  napi_clear_last_error(env);
  return napi_ok;
}

// Define class
napi_status napi_define_class(napi_env env, const char* utf8name, size_t length, napi_callback constructor, void* data, size_t property_count, const napi_property_descriptor* properties, napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, constructor);
  CHECK_ARG(env, result);
  
  // Create constructor function
  napi_value ctor;
  CHECK_NAPI(napi_create_function_with_magic(env, utf8name, length, constructor, data, &ctor));
  
  // Mark as constructor
  JSValue jsCtor = ToJSValue(ctor);
  JS_SetConstructorBit(env->context, jsCtor, true);

  // Create and set prototype object manually
  napi_value prototype;
  CHECK_NAPI(napi_create_object(env, &prototype));
  CHECK_NAPI(napi_set_named_property(env, ctor, "prototype", prototype));
  
  // Set constructor property on prototype
  CHECK_NAPI(napi_set_named_property(env, prototype, "constructor", ctor));
  
  // Define properties
  for (size_t i = 0; i < property_count; i++) {
    const napi_property_descriptor* p = &properties[i];
    
    if (p->attributes & napi_static) {
      // Static property on constructor
      napi_property_descriptor static_prop = *p;
      static_prop.attributes = static_cast<napi_property_attributes>(p->attributes & ~napi_static);
      CHECK_NAPI(napi_define_properties(env, ctor, 1, &static_prop));
    } else {
      // Instance property on prototype
      CHECK_NAPI(napi_define_properties(env, prototype, 1, p));
    }
  }
  
  *result = ctor;
  napi_clear_last_error(env);
  return napi_ok;
}

// BigInt support
napi_status napi_create_bigint_int64(napi_env env, int64_t value, napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);
  
  JSValue bigint = JS_NewBigInt64(env->context, value);
  if (JS_IsException(bigint)) {
    return napi_set_last_error(env, napi_generic_failure);
  }
  
  *result = FromJSValue(env, bigint);
  napi_clear_last_error(env);
  return napi_ok;
}

napi_status napi_create_bigint_uint64(napi_env env, uint64_t value, napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);
  
  JSValue bigint = JS_NewBigUint64(env->context, value);
  if (JS_IsException(bigint)) {
    return napi_set_last_error(env, napi_generic_failure);
  }
  
  *result = FromJSValue(env, bigint);
  napi_clear_last_error(env);
  return napi_ok;
}

napi_status napi_get_value_bigint_int64(napi_env env, napi_value value, int64_t* result, bool* lossless) {
  CHECK_ENV(env);
  CHECK_ARG(env, value);
  CHECK_ARG(env, result);
  
  JSValue jsValue = ToJSValue(value);
  
  if (JS_ToBigInt64(env->context, result, jsValue) < 0) {
    return napi_set_last_error(env, napi_bigint_expected);
  }
  
  if (lossless != nullptr) {
    *lossless = true; // QuickJS BigInt is arbitrary precision
  }
  
  napi_clear_last_error(env);
  return napi_ok;
}

napi_status napi_get_value_bigint_uint64(napi_env env, napi_value value, uint64_t* result, bool* lossless) {
  CHECK_ENV(env);
  CHECK_ARG(env, value);
  CHECK_ARG(env, result);
  
  JSValue jsValue = ToJSValue(value);
  
  int64_t val;
  if (JS_ToBigInt64(env->context, &val, jsValue) < 0) {
    return napi_set_last_error(env, napi_bigint_expected);
  }
  
  *result = static_cast<uint64_t>(val);
  
  if (lossless != nullptr) {
    *lossless = (val >= 0);
  }
  
  napi_clear_last_error(env);
  return napi_ok;
}

// Object freeze/seal
napi_status napi_object_freeze(napi_env env, napi_value object) {
  CHECK_ENV(env);
  CHECK_ARG(env, object);
  
  JSValue jsObject = ToJSValue(object);
  JSValue global = JS_GetGlobalObject(env->context);
  JSValue objectCtor = JS_GetPropertyStr(env->context, global, "Object");
  JSValue freezeFunc = JS_GetPropertyStr(env->context, objectCtor, "freeze");
  
  JSValueConst args[1] = { jsObject };
  JSValue result = JS_Call(env->context, freezeFunc, objectCtor, 1, args);
  
  JS_FreeValue(env->context, result);
  JS_FreeValue(env->context, freezeFunc);
  JS_FreeValue(env->context, objectCtor);
  JS_FreeValue(env->context, global);
  
  napi_clear_last_error(env);
  return napi_ok;
}

napi_status napi_object_seal(napi_env env, napi_value object) {
  CHECK_ENV(env);
  CHECK_ARG(env, object);
  
  JSValue jsObject = ToJSValue(object);
  JSValue global = JS_GetGlobalObject(env->context);
  JSValue objectCtor = JS_GetPropertyStr(env->context, global, "Object");
  JSValue sealFunc = JS_GetPropertyStr(env->context, objectCtor, "seal");
  
  JSValueConst args[1] = { jsObject };
  JSValue result = JS_Call(env->context, sealFunc, objectCtor, 1, args);
  
  JS_FreeValue(env->context, result);
  JS_FreeValue(env->context, sealFunc);
  JS_FreeValue(env->context, objectCtor);
  JS_FreeValue(env->context, global);
  
  napi_clear_last_error(env);
  return napi_ok;
}

// Date support
napi_status napi_create_date(napi_env env, double time, napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);
  
  JSValue date = JS_NewDate(env->context, time);
  if (JS_IsException(date)) {
    return napi_set_last_error(env, napi_generic_failure);
  }
  
  *result = FromJSValue(env, date);
  napi_clear_last_error(env);
  return napi_ok;
}

napi_status napi_is_date(napi_env env, napi_value value, bool* is_date) {
  CHECK_ENV(env);
  CHECK_ARG(env, value);
  CHECK_ARG(env, is_date);
  
  napi_value global, date_ctor;
  CHECK_NAPI(napi_get_global(env, &global));
  CHECK_NAPI(napi_get_named_property(env, global, "Date", &date_ctor));
  CHECK_NAPI(napi_instanceof(env, value, date_ctor, is_date));
  
  return napi_ok;
}

napi_status napi_get_date_value(napi_env env, napi_value value, double* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, value);
  CHECK_ARG(env, result);
  
  JSValue jsValue = ToJSValue(value);
  JSValue getTimeFunc = JS_GetPropertyStr(env->context, jsValue, "getTime");
  JSValue timeValue = JS_Call(env->context, getTimeFunc, jsValue, 0, nullptr);
  
  if (JS_ToFloat64(env->context, result, timeValue) < 0) {
    JS_FreeValue(env->context, timeValue);
    JS_FreeValue(env->context, getTimeFunc);
    return napi_set_last_error(env, napi_date_expected);
  }
  
  JS_FreeValue(env->context, timeValue);
  JS_FreeValue(env->context, getTimeFunc);
  
  napi_clear_last_error(env);
  return napi_ok;
}

// Detach arraybuffer
napi_status napi_detach_arraybuffer(napi_env env, napi_value arraybuffer) {
  CHECK_ENV(env);
  CHECK_ARG(env, arraybuffer);
  
  JSValue jsArrayBuffer = ToJSValue(arraybuffer);
  JS_DetachArrayBuffer(env->context, jsArrayBuffer);
  
  napi_clear_last_error(env);
  return napi_ok;
}

napi_status napi_is_detached_arraybuffer(napi_env env, napi_value value, bool* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, value);
  CHECK_ARG(env, result);
  
  JSValue jsValue = ToJSValue(value);
  size_t size;
  uint8_t* data = JS_GetArrayBuffer(env->context, &size, jsValue);
  
  // A detached ArrayBuffer returns NULL for data
  *result = (data == nullptr);
  
  napi_clear_last_error(env);
  return napi_ok;
}

// Exception handling
napi_status napi_is_exception_pending(napi_env env, bool* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);
  
  // Use the CURRENT execution context, not the stored context
  // This matches the behavior in napi_throw and napi_get_and_clear_last_exception
  JSContext* targetCtx = env->current_context ? env->current_context : env->context;
  *result = JS_HasException(targetCtx) != 0;
  
  napi_clear_last_error(env);
  return napi_ok;
}

// Get and clear last exception
napi_status napi_get_and_clear_last_exception(napi_env env, napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);
  
  // Use the CURRENT execution context, not the stored context
  // This matches the behavior in napi_throw and napi_is_exception_pending
  JSContext* targetCtx = env->current_context ? env->current_context : env->context;
  
  if (JS_HasException(targetCtx)) {
    JSValue exception = JS_GetException(targetCtx);
    *result = FromJSValue(env, exception);
  } else {
    static JSValue undefined = JS_UNDEFINED;
    *result = reinterpret_cast<napi_value>(&undefined);
  }
  
  napi_clear_last_error(env);
  return napi_ok;
}

// instanceof
napi_status napi_instanceof(napi_env env, napi_value object, napi_value constructor, bool* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, object);
  CHECK_ARG(env, constructor);
  CHECK_ARG(env, result);
  
  JSValue jsObj = ToJSValue(object);
  JSValue jsCtor = ToJSValue(constructor);
  
  int ret = JS_IsInstanceOf(env->context, jsObj, jsCtor);
  if (ret < 0) {
    return napi_set_last_error(env, napi_generic_failure);
  }
  
  *result = (ret != 0);
  napi_clear_last_error(env);
  return napi_ok;
}
