#ifndef SRC_JS_NATIVE_API_V8_H_
#define SRC_JS_NATIVE_API_V8_H_

// This file needs to be compatible with C compilers.
#include <string.h>  // NOLINT(modernize-deprecated-headers)
#include <napi/js_native_api_types.h>
#include "js_native_api_v8_internals.h"
#include <thread>
#include <cassert>

static napi_status napi_clear_last_error(napi_env env);

struct napi_env__ {
  explicit napi_env__(v8::Local<v8::Context> context)
      : isolate(context->GetIsolate()),
        context_persistent(isolate, context) {
    CHECK_EQ(isolate, context->GetIsolate());
  }
  virtual ~napi_env__() {
    // First we must finalize those references that have `napi_finalizer`
    // callbacks. The reason is that addons might store other references which
    // they delete during their `napi_finalizer` callbacks. If we deleted such
    // references here first, they would be doubly deleted when the
    // `napi_finalizer` deleted them subsequently.
    v8impl::RefTracker::FinalizeAll(&finalizing_reflist);
    v8impl::RefTracker::FinalizeAll(&reflist);
    if (instance_data.finalize_cb != nullptr) {
      CallIntoModuleThrow([&](napi_env env) {
        instance_data.finalize_cb(env, instance_data.data, instance_data.hint);
      });
    }
  }
  v8::Isolate* const isolate;  // Shortcut for context()->GetIsolate()
  v8impl::Persistent<v8::Context> context_persistent;

  inline v8::Local<v8::Context> context() const {
    return v8impl::PersistentToLocal::Strong(context_persistent);
  }

  inline void Ref() { refs++; }
  inline void Unref() { if ( --refs == 0) delete this; }

  virtual bool can_call_into_js() const { return true; }

  template <typename T, typename U>
  void CallIntoModule(T&& call, U&& handle_exception) {
    int open_handle_scopes_before = open_handle_scopes;
    int open_callback_scopes_before = open_callback_scopes;
    napi_clear_last_error(this);
    call(this);
    CHECK_EQ(open_handle_scopes, open_handle_scopes_before);
    CHECK_EQ(open_callback_scopes, open_callback_scopes_before);
    if (!last_exception.IsEmpty()) {
      handle_exception(this, last_exception.Get(this->isolate));
      last_exception.Reset();
    }
  }

  template <typename T>
  void CallIntoModuleThrow(T&& call) {
    CallIntoModule(call, [&](napi_env env, v8::Local<v8::Value> value) {
      env->isolate->ThrowException(value);
    });
  }

  v8impl::Persistent<v8::Value> last_exception;

  // We store references in two different lists, depending on whether they have
  // `napi_finalizer` callbacks, because we must first finalize the ones that
  // have such a callback. See `~napi_env__()` above for details.
  v8impl::RefTracker::RefList reflist;
  v8impl::RefTracker::RefList finalizing_reflist;
  napi_extended_error_info last_error;
  int open_handle_scopes = 0;
  int open_callback_scopes = 0;
  int refs = 1;
  struct {
    void* data = nullptr;
    void* hint = nullptr;
    napi_finalize finalize_cb = nullptr;
  } instance_data;

  const std::thread::id thread_id{std::this_thread::get_id()};
};

static inline napi_status napi_clear_last_error(napi_env env) {
  env->last_error.error_code = napi_ok;

  // TODO(boingoing): Should this be a callback?
  env->last_error.engine_error_code = 0;
  env->last_error.engine_reserved = nullptr;
  return napi_ok;
}

static inline
napi_status napi_set_last_error(napi_env env, napi_status error_code,
                                uint32_t engine_error_code = 0,
                                void* engine_reserved = nullptr) {
  env->last_error.error_code = error_code;
  env->last_error.engine_error_code = engine_error_code;
  env->last_error.engine_reserved = engine_reserved;
  return error_code;
}

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

#define CHECK_MAYBE_EMPTY(env, maybe, status) \
  RETURN_STATUS_IF_FALSE((env), !((maybe).IsEmpty()), (status))

// NAPI_PREAMBLE is not wrapped in do..while: try_catch must have function scope
#define NAPI_PREAMBLE(env)                                          \
  CHECK_ENV((env));                                                 \
  RETURN_STATUS_IF_FALSE((env),                                     \
      (env)->last_exception.IsEmpty() && (env)->can_call_into_js(), \
      napi_pending_exception);                                      \
  napi_clear_last_error((env));                                     \
  v8impl::TryCatch try_catch((env))

#define CHECK_TO_TYPE(env, type, context, result, src, status)                \
  do {                                                                        \
    CHECK_ARG((env), (src));                                                  \
    auto maybe = v8impl::V8LocalValueFromJsValue((src))->To##type((context)); \
    CHECK_MAYBE_EMPTY((env), maybe, (status));                                \
    (result) = maybe.ToLocalChecked();                                        \
  } while (0)

#define CHECK_TO_FUNCTION(env, result, src)                                 \
  do {                                                                      \
    CHECK_ARG((env), (src));                                                \
    v8::Local<v8::Value> v8value = v8impl::V8LocalValueFromJsValue((src));  \
    RETURN_STATUS_IF_FALSE((env), v8value->IsFunction(), napi_invalid_arg); \
    (result) = v8value.As<v8::Function>();                                  \
  } while (0)

#define CHECK_TO_OBJECT(env, context, result, src) \
  CHECK_TO_TYPE((env), Object, (context), (result), (src), napi_object_expected)

#define CHECK_TO_STRING(env, context, result, src) \
  CHECK_TO_TYPE((env), String, (context), (result), (src), napi_string_expected)

#define GET_RETURN_STATUS(env)      \
  (!try_catch.HasCaught() ? napi_ok \
                         : napi_set_last_error((env), napi_pending_exception))

#define THROW_RANGE_ERROR_IF_FALSE(env, condition, error, message) \
  do {                                                             \
    if (!(condition)) {                                            \
      napi_throw_range_error((env), (error), (message));           \
      return napi_set_last_error((env), napi_generic_failure);     \
    }                                                              \
  } while (0)

namespace v8impl {

//=== Conversion between V8 Handles and napi_value ========================

// This asserts v8::Local<> will always be implemented with a single
// pointer field so that we can pass it around as a void*.
static_assert(sizeof(v8::Local<v8::Value>) == sizeof(napi_value),
  "Cannot convert between v8::Local<v8::Value> and napi_value");

inline napi_value JsValueFromV8LocalValue(v8::Local<v8::Value> local) {
  return reinterpret_cast<napi_value>(*local);
}

inline v8::Local<v8::Value> V8LocalValueFromJsValue(napi_value v) {
  v8::Local<v8::Value> local;
  memcpy(static_cast<void*>(&local), &v, sizeof(v));
  return local;
}

// Adapter for napi_finalize callbacks.
class Finalizer {
 protected:
  Finalizer(napi_env env,
            napi_finalize finalize_callback,
            void* finalize_data,
            void* finalize_hint)
    : _env(env),
      _finalize_callback(finalize_callback),
      _finalize_data(finalize_data),
      _finalize_hint(finalize_hint) {
  }

  ~Finalizer() = default;

 public:
  static Finalizer* New(napi_env env,
                        napi_finalize finalize_callback = nullptr,
                        void* finalize_data = nullptr,
                        void* finalize_hint = nullptr) {
    return new Finalizer(
      env, finalize_callback, finalize_data, finalize_hint);
  }

  static void Delete(Finalizer* finalizer) {
    delete finalizer;
  }

 protected:
  napi_env _env;
  napi_finalize _finalize_callback;
  void* _finalize_data;
  void* _finalize_hint;
  bool _finalize_ran = false;
};

class TryCatch : public v8::TryCatch {
 public:
  explicit TryCatch(napi_env env)
      : v8::TryCatch(env->isolate), _env(env) {}

  ~TryCatch() {
    if (HasCaught()) {
      _env->last_exception.Reset(_env->isolate, Exception());
    }
  }

 private:
  napi_env _env;
};

}  // end of namespace v8impl

#endif  // SRC_JS_NATIVE_API_V8_H_
