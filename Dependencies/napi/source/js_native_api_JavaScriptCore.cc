#include "js_native_api_JavaScriptCore.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <string>
#include <vector>

namespace {
  napi_status napi_clear_last_error(napi_env env) {
    env->last_error.error_code = napi_ok;
    env->last_error.engine_error_code = 0;
    env->last_error.engine_reserved = nullptr;
    return napi_ok;
  }

  napi_status napi_set_last_error(napi_env env, napi_status error_code, uint32_t engine_error_code = 0, void* engine_reserved = nullptr) {
    env->last_error.error_code = error_code;
    env->last_error.engine_error_code = engine_error_code;
    env->last_error.engine_reserved = engine_reserved;
    return error_code;
  }

  napi_status napi_set_exception(napi_env env, JSValueRef exception) {
    env->last_exception = exception;
    return napi_set_last_error(env, napi_pending_exception);
  }

  class JSString {
   public:
    JSString(const JSString&) = delete;

    JSString(JSString&& other) {
      _string = other._string;
      other._string = nullptr;
    }

    JSString(const char* string, size_t length = NAPI_AUTO_LENGTH)
      : _string{JSStringCreateWithUTF8CString(length == NAPI_AUTO_LENGTH ? string : std::string(string, length).data())} {
    }

    JSString(const JSChar* string, size_t length = NAPI_AUTO_LENGTH)
      : _string{JSStringCreateWithCharacters(string, length == NAPI_AUTO_LENGTH ? std::char_traits<JSChar>::length(string) : length)} {
    }

    ~JSString() {
      if (_string != nullptr) {
        JSStringRelease(_string);
      }
    }

    static JSString Attach(JSStringRef string) {
      return {string};
    }

    static JSString FromNumber(JSContextRef ctx, double number, JSValueRef* exception) {
        JSValueRef numberValueRef = JSValueMakeNumber(ctx, number);
        JSStringRef numberStringRef = JSValueToStringCopy(ctx, numberValueRef, exception);
        return Attach(numberStringRef);
    }

    operator JSStringRef() const {
      return _string;
    }

    size_t Length() const {
      return JSStringGetLength(_string);
    }

    size_t LengthUTF8() const {
      std::vector<char> buffer(JSStringGetMaximumUTF8CStringSize(_string));
      return JSStringGetUTF8CString(_string, buffer.data(), buffer.size()) - 1;
    }

    size_t LengthLatin1() const {
      // Latin1 has the same length as Unicode.
      return JSStringGetLength(_string);
    }

    void CopyTo(JSChar* buf, size_t bufsize, size_t* result) const {
      size_t length{JSStringGetLength(_string)};
      const JSChar* chars{JSStringGetCharactersPtr(_string)};
      size_t size{std::min(length, bufsize - 1)};
      std::memcpy(buf, chars, size);
      buf[size] = 0;
      if (result != nullptr) {
        *result = size;
      }
    }

    void CopyToUTF8(char* buf, size_t bufsize, size_t* result) const {
      size_t size{JSStringGetUTF8CString(_string, buf, bufsize)};
      if (result != nullptr) {
        // JSStringGetUTF8CString returns size with null terminator.
        *result = size - 1;
      }
    }

    void CopyToLatin1(char* buf, size_t bufsize, size_t* result) const {
      size_t length{JSStringGetLength(_string)};
      const JSChar* chars{JSStringGetCharactersPtr(_string)};
      size_t size{std::min(length, bufsize - 1)};
      for (int i = 0; i < size; ++i) {
        const JSChar ch{chars[i]};
        buf[i] = (ch < 256) ? ch : '?';
      }
      if (result != nullptr) {
        *result = size;
      }
    }

   private:
    JSString(JSStringRef string)
        : _string{string} {
    }

    JSStringRef _string;
  };

  JSValueRef ToJSValue(const napi_value value) {
    return reinterpret_cast<JSValueRef>(value);
  }

  const JSValueRef* ToJSValues(const napi_value* values) {
    return reinterpret_cast<const JSValueRef*>(values);
  }

  JSObjectRef ToJSObject(napi_env env, const napi_value value) {
    assert(value == nullptr || JSValueIsObject(env->context, reinterpret_cast<JSValueRef>(value)));
    return reinterpret_cast<JSObjectRef>(value);
  }

  JSString ToJSString(napi_env env, napi_value value, JSValueRef* exception) {
    return JSString::Attach(JSValueToStringCopy(env->context, ToJSValue(value), exception));
  }

  napi_value ToNapi(const JSValueRef value) {
    return reinterpret_cast<napi_value>(const_cast<OpaqueJSValue*>(value));
  }

  napi_value* ToNapi(const JSValueRef* values) {
    return reinterpret_cast<napi_value*>(const_cast<OpaqueJSValue**>(values));
  }

  struct CallbackInfo {
    napi_value newTarget;
    napi_value thisArg;
    napi_value* argv;
    void* data;
    uint16_t argc;
  };

  class Constructor {
   public:
    Constructor(napi_env env, const char* name, size_t length, napi_callback cb, void* data)
      : _env{env}
      , _name{name, (length == NAPI_AUTO_LENGTH ? strlen(name) : length)}
      , _cb{cb}
      , _data{data} {
      JSClassDefinition definition = kJSClassDefinitionEmpty;
      definition.className = _name.data();
      _instanceClass = JSClassCreate(&definition);
      definition.callAsConstructor = Constructor::Callback;
      definition.finalize = Constructor::Finalize;
      _class = JSClassCreate(&definition);;
    }

    ~Constructor() {
      JSClassRelease(_instanceClass);
      JSClassRelease(_class);
    }

    JSClassRef Class() const {
      return _class;
    }

   private:
    // JSObjectCallAsConstructorCallback
    static JSObjectRef Callback(JSContextRef ctx,
                                JSObjectRef constructor,
                                size_t argumentCount,
                                const JSValueRef arguments[],
                                JSValueRef* exception) {
      Constructor* ctor{reinterpret_cast<Constructor*>(JSObjectGetPrivate(constructor))};

      // Make sure any errors encountered last time we were in N-API are gone.
      napi_clear_last_error(ctor->_env);

      JSObjectRef instance{JSObjectMake(ctx, ctor->_instanceClass, nullptr)};
      JSValueRef prototype{JSObjectGetPrototype(ctx, constructor)};
      JSObjectSetPrototype(ctx, instance, prototype);

      CallbackInfo cbInfo;
      cbInfo.thisArg = ToNapi(instance);
      cbInfo.newTarget = ToNapi(constructor);
      cbInfo.argc = argumentCount;
      cbInfo.argv = ToNapi(arguments);
      cbInfo.data = ctor->_data;

      napi_value result = ctor->_cb(ctor->_env, reinterpret_cast<napi_callback_info>(&cbInfo));
      return ToJSObject(ctor->_env, result);
    }

    // JSObjectFinalizeCallback
    static void Finalize(JSObjectRef object) {
      Constructor* ctor{reinterpret_cast<Constructor*>(JSObjectGetPrivate(object))};
      delete ctor;
    }

    napi_env _env;
    std::string _name;
    napi_callback _cb;
    void* _data;
    JSClassRef _class;
    JSClassRef _instanceClass;
  };

  class Function {
   public:
    Function(napi_env env, const char* name, size_t length, napi_callback cb, void* data)
      : _env{env}
      , _name{name, (length == NAPI_AUTO_LENGTH ? strlen(name) : length)}
      , _cb{cb}
      , _data{data} {
      JSClassDefinition definition = kJSClassDefinitionEmpty;
      definition.className = _name.data();
      definition.attributes = kJSClassAttributeNoAutomaticPrototype;
      definition.callAsFunction = Function::Callback;
      definition.finalize = Function::Finalize;
      _class = JSClassCreate(&definition);
    }

    ~Function() {
      JSClassRelease(_class);
    }

    JSClassRef Class() const {
      return _class;
    }

   private:
    // JSObjectCallAsFunctionCallback
    static JSValueRef Callback(JSContextRef ctx,
                               JSObjectRef function,
                               JSObjectRef thisObject,
                               size_t argumentCount,
                               const JSValueRef arguments[],
                               JSValueRef* exception) {
      Function* func{reinterpret_cast<Function*>(JSObjectGetPrivate(function))};

      // Make sure any errors encountered last time we were in N-API are gone.
      napi_clear_last_error(func->_env);

      CallbackInfo cbInfo;
      cbInfo.thisArg = ToNapi(thisObject);
      cbInfo.newTarget = nullptr;
      cbInfo.argc = argumentCount;
      cbInfo.argv = ToNapi(arguments);
      cbInfo.data = func->_data;

      napi_value result = func->_cb(func->_env, reinterpret_cast<napi_callback_info>(&cbInfo));
      return ToJSValue(result);
    }

    // JSObjectFinalizeCallback
    static void Finalize(JSObjectRef object) {
      Function* info{reinterpret_cast<Function*>(JSObjectGetPrivate(object))};
      delete info;
    }

    napi_env _env;
    std::string _name;
    napi_callback _cb;
    void* _data;
    JSClassRef _class;
  };

  class External {
   public:
    External(napi_env env, void* data, napi_finalize finalize_cb, void* hint)
      : _env{env}
      , _data{data}
      , _cb{finalize_cb}
      , _hint{hint} {
      JSClassDefinition definition = kJSClassDefinitionEmpty;
      definition.attributes = kJSClassAttributeNoAutomaticPrototype;
      definition.className = "External";
      definition.finalize = External::Finalize;
      _class = JSClassCreate(&definition);
    }

    ~External() {
      JSClassRelease(_class);
    }

    JSClassRef Class() const {
      return _class;
    }

    void* Data() const {
      return _data;
    }

    // JSTypedArrayBytesDeallocator
    static void Finalize(void* bytes, void* deallocatorContext) {
      External* external{reinterpret_cast<External*>(deallocatorContext)};
      if (external->_cb != nullptr) {
        external->_cb(external->_env, external->_data, external->_hint);
      }
      delete external;
    }

   private:
    // JSObjectFinalizeCallback
    static void Finalize(JSObjectRef object) {
      External* external{reinterpret_cast<External*>(JSObjectGetPrivate(object))};
      if (external->_cb != nullptr) {
        external->_cb(external->_env, external->_data, external->_hint);
      }
      delete external;
    }

    napi_env _env;
    void* _data;
    napi_finalize _cb;
    void* _hint;
    JSClassRef _class;
  };

  struct RefInfo {
    JSValueRef value;
    uint32_t count;
  };

  napi_status napi_set_error_code(napi_env env,
                                  napi_value error,
                                  napi_value code,
                                  const char* code_cstring) {
    napi_value code_value{code};
    if (code_value == nullptr) {
      code_value = ToNapi(JSValueMakeString(env->context, JSString(code_cstring)));
    } else {
      RETURN_STATUS_IF_FALSE(env, JSValueIsString(env->context, ToJSValue(code_value)), napi_string_expected);
    }

    CHECK_NAPI(napi_set_named_property(env, error, "code", code_value));
    return napi_ok;
  }
}

// Warning: Keep in-sync with napi_status enum
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
};

napi_status napi_get_last_error_info(napi_env env,
                                     const napi_extended_error_info** result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);

  // you must update this assert to reference the last message
  // in the napi_status enum each time a new error message is added.
  // We don't have a napi_status_last as this would result in an ABI
  // change each time a message was added.
  static_assert(
    std::size(error_messages) == napi_bigint_expected + 1,
    "Count of error messages must match count of error values");
  assert(env->last_error.error_code <= napi_callback_scope_mismatch);

  // Wait until someone requests the last error information to fetch the error
  // message string
  env->last_error.error_message =
    error_messages[env->last_error.error_code];

  *result = &env->last_error;
  return napi_ok;
}

napi_status napi_create_function(napi_env env,
                                 const char* utf8name,
                                 size_t length,
                                 napi_callback cb,
                                 void* callback_data,
                                 napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);

  Function* function{new Function(env, utf8name, length, cb, callback_data)};
  if (function == nullptr) {
    return napi_set_last_error(env, napi_generic_failure);
  }
  
  JSObjectRef instance{JSObjectMake(env->context, function->Class(), function)};

  napi_value global{}, function_ctor{}, function_proto{};
  CHECK_NAPI(napi_get_global(env, &global));
  CHECK_NAPI(napi_get_named_property(env, global, "Function", &function_ctor));
  CHECK_NAPI(napi_get_prototype(env, function_ctor, &function_proto));

  JSObjectSetPrototype(env->context, instance, ToJSValue(function_proto));
 
  *result = ToNapi(instance);
  return napi_ok;
}

napi_status napi_define_class(napi_env env,
                              const char* utf8name,
                              size_t length,
                              napi_callback cb,
                              void* data,
                              size_t property_count,
                              const napi_property_descriptor* properties,
                              napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);

  Constructor* ctor{new Constructor(env, utf8name, length, cb, data)};
  if (ctor == nullptr) {
    return napi_set_last_error(env, napi_generic_failure);
  }

  napi_value constructor{ToNapi(JSObjectMake(env->context, ctor->Class(), ctor))};

  napi_value prototype{};
  CHECK_NAPI(napi_get_prototype(env, constructor, &prototype));
  CHECK_NAPI(napi_set_named_property(env, prototype, "constructor", constructor));

  int instancePropertyCount{0};
  int staticPropertyCount{0};
  for (size_t i = 0; i < property_count; i++) {
    if ((properties[i].attributes & napi_static) != 0) {
      staticPropertyCount++;
    } else {
      instancePropertyCount++;
    }
  }

  std::vector<napi_property_descriptor> staticDescriptors{};
  std::vector<napi_property_descriptor> instanceDescriptors{};
  staticDescriptors.reserve(staticPropertyCount);
  instanceDescriptors.reserve(instancePropertyCount);

  for (size_t i = 0; i < property_count; i++) {
    if ((properties[i].attributes & napi_static) != 0) {
      staticDescriptors.push_back(properties[i]);
    } else {
      instanceDescriptors.push_back(properties[i]);
    }
  }

  if (staticPropertyCount > 0) {
    CHECK_NAPI(napi_define_properties(env,
                                      constructor,
                                      staticDescriptors.size(),
                                      staticDescriptors.data()));
  }

  if (instancePropertyCount > 0) {
    CHECK_NAPI(napi_define_properties(env,
                                      prototype,
                                      instanceDescriptors.size(),
                                      instanceDescriptors.data()));
  }

  *result = constructor;
  return napi_ok;
}

napi_status napi_get_property_names(napi_env env,
                                    napi_value object,
                                    napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);

  napi_value global{}, object_ctor{}, function{};
  CHECK_NAPI(napi_get_global(env, &global));
  CHECK_NAPI(napi_get_named_property(env, global, "Object", &object_ctor));
  CHECK_NAPI(napi_get_named_property(env, object_ctor, "getOwnPropertyNames", &function));
  CHECK_NAPI(napi_call_function(env, object_ctor, function, 0, nullptr, result));

  return napi_ok;
}

napi_status napi_set_property(napi_env env,
                              napi_value object,
                              napi_value key,
                              napi_value value) {
  CHECK_ENV(env);
  CHECK_ARG(env, key);
  CHECK_ARG(env, value);

  JSValueRef exception{};
  JSString key_str{ToJSString(env, key, &exception)};
  CHECK_JSC(env, exception);

  JSObjectSetProperty(
    env->context,
    ToJSObject(env, object),
    key_str,
    ToJSValue(value),
    kJSPropertyAttributeNone,
    &exception);
  CHECK_JSC(env, exception);

  return napi_ok;
}

napi_status napi_has_property(napi_env env,
                              napi_value object,
                              napi_value key,
                              bool* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);
  CHECK_ARG(env, key);

  JSValueRef exception{};
  JSString key_str{ToJSString(env, key, &exception)};
  CHECK_JSC(env, exception);

  *result = JSObjectHasProperty(
    env->context,
    ToJSObject(env, object),
    key_str);
  return napi_ok;
}

napi_status napi_get_property(napi_env env,
                              napi_value object,
                              napi_value key,
                              napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, key);
  CHECK_ARG(env, result);

  JSValueRef exception{};
  JSString key_str{ToJSString(env, key, &exception)};
  CHECK_JSC(env, exception);

  *result = ToNapi(JSObjectGetProperty(
    env->context,
    ToJSObject(env, object),
    key_str,
    &exception));
  CHECK_JSC(env, exception);

  return napi_ok;
}

napi_status napi_delete_property(napi_env env,
                                 napi_value object,
                                 napi_value key,
                                 bool* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);

  JSValueRef exception{};
  JSString key_str{ToJSString(env, key, &exception)};
  CHECK_JSC(env, exception);

  *result = JSObjectDeleteProperty(
    env->context,
    ToJSObject(env, object),
    key_str,
    &exception);
  CHECK_JSC(env, exception);

  return napi_ok;
}

NAPI_EXTERN napi_status napi_has_own_property(napi_env env,
                                              napi_value object,
                                              napi_value key,
                                              bool* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);

  napi_value global{}, object_ctor{}, function{}, value{};
  CHECK_NAPI(napi_get_global(env, &global));
  CHECK_NAPI(napi_get_named_property(env, global, "Object", &object_ctor));
  CHECK_NAPI(napi_get_named_property(env, object_ctor, "hasOwnProperty", &function));
  CHECK_NAPI(napi_call_function(env, object_ctor, function, 0, nullptr, &value));
  *result = JSValueToBoolean(env->context, ToJSValue(value));

  return napi_ok;
}

napi_status napi_set_named_property(napi_env env,
                                    napi_value object,
                                    const char* utf8name,
                                    napi_value value) {
  CHECK_ENV(env);
  CHECK_ARG(env, value);

  JSValueRef exception{};
  JSObjectSetProperty(
    env->context,
    ToJSObject(env, object),
    JSString(utf8name),
    ToJSValue(value),
    kJSPropertyAttributeNone,
    &exception);
  CHECK_JSC(env, exception);

  return napi_ok;
}

napi_status napi_has_named_property(napi_env env,
                                    napi_value object,
                                    const char* utf8name,
                                    bool* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, object);

  *result = JSObjectHasProperty(
    env->context,
    ToJSObject(env, object),
    JSString(utf8name));

  return napi_ok;
}

napi_status napi_get_named_property(napi_env env,
                                    napi_value object,
                                    const char* utf8name,
                                    napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, object);

  JSValueRef exception{};
  *result = ToNapi(JSObjectGetProperty(
    env->context,
    ToJSObject(env, object),
    JSString(utf8name),
    &exception));
  CHECK_JSC(env, exception);

  return napi_ok;
}

napi_status napi_set_element(napi_env env,
                             napi_value object,
                             uint32_t index,
                             napi_value value) {
  CHECK_ENV(env);
  CHECK_ARG(env, value);

  JSValueRef exception{};
  JSObjectSetPropertyAtIndex(
    env->context,
    ToJSObject(env, object),
    index,
    ToJSValue(value),
    &exception);
  CHECK_JSC(env, exception);

  return napi_ok;
}

napi_status napi_has_element(napi_env env,
                             napi_value object,
                             uint32_t index,
                             bool* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);

  JSValueRef exception{};
  JSValueRef value{JSObjectGetPropertyAtIndex(
    env->context,
    ToJSObject(env, object),
    index,
    &exception)};
  CHECK_JSC(env, exception);

  *result = !JSValueIsUndefined(env->context, value);
  return napi_ok;
}

napi_status napi_get_element(napi_env env,
                             napi_value object,
                             uint32_t index,
                             napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);

  JSValueRef exception{};
  *result = ToNapi(JSObjectGetPropertyAtIndex(
    env->context,
    ToJSObject(env, object),
    index,
    &exception));
  CHECK_JSC(env, exception);

  return napi_ok;
}

napi_status napi_delete_element(napi_env env,
                                napi_value object,
                                uint32_t index,
                                bool* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);

  JSValueRef exception{};
  JSString propertyName{JSString::FromNumber(env->context, static_cast<double>(index), &exception)};
  CHECK_JSC(env, exception);

  *result = JSObjectDeleteProperty(
    env->context,
    ToJSObject(env, object),
    propertyName,
    &exception);
  CHECK_JSC(env, exception);

  return napi_ok;
}

napi_status napi_define_properties(napi_env env,
                                   napi_value object,
                                   size_t property_count,
                                   const napi_property_descriptor* properties) {
  CHECK_ENV(env);
  if (property_count > 0) {
    CHECK_ARG(env, properties);
  }

  for (size_t i = 0; i < property_count; i++) {
    const napi_property_descriptor* p{properties + i};

    napi_value descriptor{};
    CHECK_NAPI(napi_create_object(env, &descriptor));

    napi_value configurable{};
    CHECK_NAPI(napi_get_boolean(env, (p->attributes & napi_configurable), &configurable));
    CHECK_NAPI(napi_set_named_property(env, descriptor, "configurable", configurable));

    napi_value enumerable{};
    CHECK_NAPI(napi_get_boolean(env, (p->attributes & napi_configurable), &enumerable));
    CHECK_NAPI(napi_set_named_property(env, descriptor, "enumerable", enumerable));

    if (p->getter != nullptr || p->setter != nullptr) {
      if (p->getter != nullptr) {
        napi_value getter{};
        CHECK_NAPI(napi_create_function(env, p->utf8name, NAPI_AUTO_LENGTH, p->getter, p->data, &getter));
        CHECK_NAPI(napi_set_named_property(env, descriptor, "get", getter));
      }
      if (p->setter != nullptr) {
        napi_value setter{};
        CHECK_NAPI(napi_create_function(env, p->utf8name, NAPI_AUTO_LENGTH, p->setter, p->data, &setter));
        CHECK_NAPI(napi_set_named_property(env, descriptor, "set", setter));
      }
    } else if (p->method != nullptr) {
      napi_value method{};
      CHECK_NAPI(napi_create_function(env, p->utf8name, NAPI_AUTO_LENGTH, p->method, p->data, &method));
      CHECK_NAPI(napi_set_named_property(env, descriptor, "value", method));
    } else {
      RETURN_STATUS_IF_FALSE(env, p->value != nullptr, napi_invalid_arg);

      napi_value writable{};
      CHECK_NAPI(napi_get_boolean(env, (p->attributes & napi_writable), &writable));
      CHECK_NAPI(napi_set_named_property(env, descriptor, "writable", writable));

      CHECK_NAPI(napi_set_named_property(env, descriptor, "value", p->value));
    }

    napi_value propertyName{};
    if (p->utf8name == nullptr) {
      propertyName = p->name;
    } else {
      napi_create_string_utf8(env, p->utf8name, NAPI_AUTO_LENGTH, &propertyName);
    }

    napi_value global{}, object_ctor{}, function{};
    CHECK_NAPI(napi_get_global(env, &global));
    CHECK_NAPI(napi_get_named_property(env, global, "Object", &object_ctor));
    CHECK_NAPI(napi_get_named_property(env, object_ctor, "defineProperty", &function));

    napi_value args[] = { object, propertyName, descriptor };
    CHECK_NAPI(napi_call_function(env, object_ctor, function, 3, args, nullptr));
  }

  return napi_ok;
}

napi_status napi_is_array(napi_env env, napi_value value, bool* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, value);
  CHECK_ARG(env, result);

  *result = JSValueIsArray(
    env->context,
    ToJSValue(value));
  return napi_ok;
}

napi_status napi_get_array_length(napi_env env,
                                  napi_value value,
                                  uint32_t* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, value);
  CHECK_ARG(env, result);

  JSValueRef exception{};
  JSValueRef length = JSObjectGetProperty(
    env->context,
    ToJSObject(env, value),
    JSString("length"),
    &exception);
  CHECK_JSC(env, exception);

  *result = static_cast<uint32_t>(JSValueToNumber(env->context, length, &exception));
  CHECK_JSC(env, exception);

  return napi_ok;
}

napi_status napi_strict_equals(napi_env env,
                               napi_value lhs,
                               napi_value rhs,
                               bool* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, lhs);
  CHECK_ARG(env, rhs);
  CHECK_ARG(env, result);
  *result = JSValueIsStrictEqual(
    env->context,
    ToJSValue(lhs),
    ToJSValue(rhs));
  return napi_ok;
}

napi_status napi_get_prototype(napi_env env,
                               napi_value object,
                               napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);
  *result = ToNapi(JSObjectGetPrototype(
    env->context,
    ToJSObject(env, object)));
  return napi_ok;
}

napi_status napi_create_object(napi_env env, napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);
  *result = ToNapi(JSObjectMake(env->context, nullptr, nullptr));
  return napi_ok;
}

napi_status napi_create_array(napi_env env, napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);

  JSValueRef exception{};
  *result = ToNapi(JSObjectMakeArray(env->context, 0, nullptr, &exception));
  CHECK_JSC(env, exception);

  return napi_ok;
}

napi_status napi_create_array_with_length(napi_env env,
                                          size_t length,
                                          napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);

  JSValueRef exception{};
  JSObjectRef array = JSObjectMakeArray(
    env->context,
    0,
    nullptr,
    &exception);
  CHECK_JSC(env, exception);

  JSObjectSetProperty(
    env->context,
    array,
    JSString("length"),
    JSValueMakeNumber(env->context, static_cast<double>(length)),
    kJSPropertyAttributeNone,
    &exception);
  CHECK_JSC(env, exception);

  *result = ToNapi(array);
  return napi_ok;
}

napi_status napi_create_string_latin1(napi_env env,
                                      const char* str,
                                      size_t length,
                                      napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);
  *result = ToNapi(JSValueMakeString(
    env->context,
    JSString(str, length)));
  return napi_ok;
}

napi_status napi_create_string_utf8(napi_env env,
                                    const char* str,
                                    size_t length,
                                    napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);
  *result = ToNapi(JSValueMakeString(
    env->context,
    JSString(str, length)));
  return napi_ok;
}

napi_status napi_create_string_utf16(napi_env env,
                                     const char16_t* str,
                                     size_t length,
                                     napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);
  static_assert(sizeof(char16_t) == sizeof(JSChar));
  *result = ToNapi(JSValueMakeString(
    env->context,
    JSString(reinterpret_cast<const JSChar*>(str), length)));
  return napi_ok;
}

napi_status napi_create_double(napi_env env,
                               double value,
                               napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);
  *result = ToNapi(JSValueMakeNumber(env->context, value));
  return napi_ok;
}

napi_status napi_create_int32(napi_env env,
                              int32_t value,
                              napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);
  *result = ToNapi(JSValueMakeNumber(env->context, static_cast<double>(value)));
  return napi_ok;
}

napi_status napi_create_uint32(napi_env env,
                               uint32_t value,
                               napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);
  *result = ToNapi(JSValueMakeNumber(env->context, static_cast<double>(value)));
  return napi_ok;
}

napi_status napi_create_int64(napi_env env,
                              int64_t value,
                              napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);
  *result = ToNapi(JSValueMakeNumber(env->context, static_cast<double>(value)));
  return napi_ok;
}

napi_status napi_get_boolean(napi_env env, bool value, napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);
  *result = ToNapi(JSValueMakeBoolean(env->context, value));
  return napi_ok;
}

napi_status napi_create_symbol(napi_env env,
                               napi_value description,
                               napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);

  napi_value global{}, symbol_func{};
  CHECK_NAPI(napi_get_global(env, &global));
  CHECK_NAPI(napi_get_named_property(env, global, "Symbol", &symbol_func));
  napi_value args[] = { description };
  CHECK_NAPI(napi_call_function(env, global, symbol_func, 1, args, result));
  return napi_ok;
}

napi_status napi_create_error(napi_env env,
                              napi_value code,
                              napi_value msg,
                              napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, msg);
  CHECK_ARG(env, result);

  JSValueRef exception{};
  JSValueRef args[] = { ToJSValue(msg) };
  napi_value error = ToNapi(JSObjectMakeError(env->context, 1, args, &exception));
  CHECK_JSC(env, exception);

  CHECK_NAPI(napi_set_error_code(env, error, code, nullptr));

  *result = error;
  return napi_ok;
}

napi_status napi_create_type_error(napi_env env,
                                   napi_value code,
                                   napi_value msg,
                                   napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, msg);
  CHECK_ARG(env, result);

  napi_value global{}, error_ctor{}, error{};
  CHECK_NAPI(napi_get_global(env, &global));
  CHECK_NAPI(napi_get_named_property(env, global, "TypeError", &error_ctor));
  CHECK_NAPI(napi_new_instance(env, error_ctor, 1, &msg, &error));
  CHECK_NAPI(napi_set_error_code(env, error, code, nullptr));

  *result = error;
  return napi_ok;
}

napi_status napi_create_range_error(napi_env env,
                                    napi_value code,
                                    napi_value msg,
                                    napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, msg);
  CHECK_ARG(env, result);

  napi_value global{}, error_ctor{}, error{};
  CHECK_NAPI(napi_get_global(env, &global));
  CHECK_NAPI(napi_get_named_property(env, global, "RangeError", &error_ctor));
  CHECK_NAPI(napi_new_instance(env, error_ctor, 1, &msg, &error));
  CHECK_NAPI(napi_set_error_code(env, error, code, nullptr));

  *result = error;
  return napi_ok;
}

napi_status napi_typeof(napi_env env, napi_value value, napi_valuetype* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, value);
  CHECK_ARG(env, result);

  // JSC does not support BigInt
  JSType valueType = JSValueGetType(env->context, ToJSValue(value));
  switch (valueType) {
    case kJSTypeUndefined: *result = napi_undefined; break;
    case kJSTypeNull: *result = napi_null; break;
    case kJSTypeBoolean: *result = napi_boolean; break;
    case kJSTypeNumber: *result = napi_number; break;
    case kJSTypeString: *result = napi_string; break;
    case kJSTypeSymbol: *result = napi_symbol; break;
    default:
      JSObjectRef object{ToJSObject(env, value)};
      if (JSObjectIsFunction(env->context, object)) {
        *result = napi_function;
      } else if (JSObjectGetPrivate(object) == nullptr) {
        *result = napi_object;
      } else {
        *result = napi_external;
      }
      break;
  }

  return napi_ok;
}

napi_status napi_get_undefined(napi_env env, napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);
  *result = ToNapi(JSValueMakeUndefined(env->context));
  return napi_ok;
}

napi_status napi_get_null(napi_env env, napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);
  *result = ToNapi(JSValueMakeNull(env->context));
  return napi_ok;
}

napi_status napi_get_cb_info(napi_env env,              // [in] NAPI environment handle
                             napi_callback_info cbinfo, // [in] Opaque callback-info handle
                             size_t* argc,              // [in-out] Specifies the size of the provided argv array
                                                        // and receives the actual count of args.
                             napi_value* argv,          // [out] Array of values
                             napi_value* this_arg,      // [out] Receives the JS 'this' arg for the call
                             void** data) {             // [out] Receives the data pointer for the callback.
  CHECK_ENV(env);
  CHECK_ARG(env, cbinfo);
  const CallbackInfo* info{reinterpret_cast<CallbackInfo*>(cbinfo)};

  if (argv != nullptr) {
    CHECK_ARG(env, argc);

    size_t i{0};
    size_t min{std::min(*argc, static_cast<size_t>(info->argc))};

    for (; i < min; i++) {
      argv[i] = info->argv[i];
    }

    if (i < *argc) {
      for (; i < *argc; i++) {
        argv[i] = ToNapi(JSValueMakeUndefined(env->context));
      }
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

  return napi_ok;
}

napi_status napi_get_new_target(napi_env env,
                                napi_callback_info cbinfo,
                                napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, cbinfo);
  CHECK_ARG(env, result);

  const CallbackInfo* info{reinterpret_cast<CallbackInfo*>(cbinfo)};
  *result = info->newTarget;
  return napi_ok;
}

napi_status napi_call_function(napi_env env,
                               napi_value recv,
                               napi_value func,
                               size_t argc,
                               const napi_value* argv,
                               napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, recv);
  if (argc > 0) {
    CHECK_ARG(env, argv);
  }
  
  JSValueRef exception{};
  JSValueRef return_value{JSObjectCallAsFunction(
    env->context,
    ToJSObject(env, func),
    JSValueIsUndefined(env->context, ToJSValue(recv)) ? nullptr : ToJSObject(env, recv),
    argc,
    ToJSValues(argv),
    &exception)};
  CHECK_JSC(env, exception);

  if (result != nullptr) {
    *result = ToNapi(return_value);
  }

  return napi_ok;
}

napi_status napi_get_global(napi_env env, napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);
  *result = ToNapi(JSContextGetGlobalObject(env->context));
  return napi_ok;
}

napi_status napi_throw(napi_env env, napi_value error) {
  CHECK_ENV(env);

  napi_value global{}, throw_error_func{};
  CHECK_NAPI(napi_get_global(env, &global));
  CHECK_NAPI(napi_get_named_property(env, global, "__throwError__", &throw_error_func));

  if (JSValueIsUndefined(env->context, ToJSValue(throw_error_func))) {
    napi_value script{};
    CHECK_NAPI(napi_create_string_utf16(
      env,
      u"globalThis.__throwError__ = function (error) { throw error; }",
      NAPI_AUTO_LENGTH,
      &script));
    CHECK_NAPI(napi_run_script(env, script, nullptr, &throw_error_func));
  }

  napi_call_function(env, nullptr, throw_error_func, 1, &error, nullptr);
  return napi_clear_last_error(env);
}

napi_status napi_throw_error(napi_env env,
                             const char* code,
                             const char* msg) {
  CHECK_ENV(env);
  napi_value code_value{ToNapi(JSValueMakeString(env->context, JSString(code)))};
  napi_value msg_value{ToNapi(JSValueMakeString(env->context, JSString(msg)))};
  napi_value error{};
  CHECK_NAPI(napi_create_error(env, code_value, msg_value, &error));
  return napi_throw(env, error);
}

napi_status napi_throw_type_error(napi_env env,
                                  const char* code,
                                  const char* msg) {
  CHECK_ENV(env);
  napi_value code_value{ToNapi(JSValueMakeString(env->context, JSString(code)))};
  napi_value msg_value{ToNapi(JSValueMakeString(env->context, JSString(msg)))};
  napi_value error{};
  CHECK_NAPI(napi_create_type_error(env, code_value, msg_value, &error));
  return napi_throw(env, error);
}

napi_status napi_throw_range_error(napi_env env,
                                   const char* code,
                                   const char* msg) {
  CHECK_ENV(env);
  napi_value code_value{ToNapi(JSValueMakeString(env->context, JSString(code)))};
  napi_value msg_value{ToNapi(JSValueMakeString(env->context, JSString(msg)))};
  napi_value error{};
  CHECK_NAPI(napi_create_range_error(env, code_value, msg_value, &error));
  return napi_throw(env, error);
}

napi_status napi_is_error(napi_env env, napi_value value, bool* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, value);
  CHECK_ARG(env, result);

  napi_value global{}, error_ctor{};
  CHECK_NAPI(napi_get_global(env, &global));
  CHECK_NAPI(napi_get_named_property(env, global, "Error", &error_ctor));
  CHECK_NAPI(napi_instanceof(env, value, error_ctor, result));

  return napi_ok;
}

napi_status napi_get_value_double(napi_env env, napi_value value, double* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, value);
  CHECK_ARG(env, result);

  JSValueRef exception{};
  *result = JSValueToNumber(env->context, ToJSValue(value), &exception);
  CHECK_JSC(env, exception);

  return napi_ok;
}

napi_status napi_get_value_int32(napi_env env, napi_value value, int32_t* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, value);
  CHECK_ARG(env, result);

  JSValueRef exception{};
  *result = static_cast<int32_t>(JSValueToNumber(env->context, ToJSValue(value), &exception));
  CHECK_JSC(env, exception);

  return napi_ok;
}

napi_status napi_get_value_uint32(napi_env env, napi_value value, uint32_t* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, value);
  CHECK_ARG(env, result);

  JSValueRef exception{};
  *result = static_cast<uint32_t>(JSValueToNumber(env->context, ToJSValue(value), &exception));
  CHECK_JSC(env, exception);

  return napi_ok;
}

napi_status napi_get_value_int64(napi_env env, napi_value value, int64_t* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, value);
  CHECK_ARG(env, result);

  JSValueRef exception{};
  double number = JSValueToNumber(env->context, ToJSValue(value), &exception);
  CHECK_JSC(env, exception);

  if (std::isfinite(number)) {
    *result = static_cast<int64_t>(number);
  } else {
    *result = 0;
  }

  return napi_ok;
}

napi_status napi_get_value_bool(napi_env env, napi_value value, bool* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, value);
  CHECK_ARG(env, result);
  *result = JSValueToBoolean(env->context, ToJSValue(value));
  return napi_ok;
}

// Copies a JavaScript string into a LATIN-1 string buffer. The result is the
// number of bytes (excluding the null terminator) copied into buf.
// A sufficient buffer size should be greater than the length of string,
// reserving space for null terminator.
// If bufsize is insufficient, the string will be truncated and null terminated.
// If buf is NULL, this method returns the length of the string (in bytes)
// via the result parameter.
// The result argument is optional unless buf is NULL.
napi_status napi_get_value_string_latin1(napi_env env,
                                         napi_value value,
                                         char* buf,
                                         size_t bufsize,
                                         size_t* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, value);

  JSValueRef exception{};
  JSString string{ToJSString(env, value, &exception)};
  CHECK_JSC(env, exception);

  if (buf == nullptr) {
    *result = string.LengthLatin1();
  } else {
    string.CopyToLatin1(buf, bufsize, result);
  }

  return napi_ok;
}

// Copies a JavaScript string into a UTF-8 string buffer. The result is the
// number of bytes (excluding the null terminator) copied into buf.
// A sufficient buffer size should be greater than the length of string,
// reserving space for null terminator.
// If bufsize is insufficient, the string will be truncated and null terminated.
// If buf is NULL, this method returns the length of the string (in bytes)
// via the result parameter.
// The result argument is optional unless buf is NULL.
napi_status napi_get_value_string_utf8(napi_env env,
                                       napi_value value,
                                       char* buf,
                                       size_t bufsize,
                                       size_t* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, value);

  JSValueRef exception{};
  JSString string{ToJSString(env, value, &exception)};
  CHECK_JSC(env, exception);

  if (buf == nullptr) {
    *result = string.LengthUTF8();
  } else {
    string.CopyToUTF8(buf, bufsize, result);
  }

  return napi_ok;
}

// Copies a JavaScript string into a UTF-16 string buffer. The result is the
// number of 2-byte code units (excluding the null terminator) copied into buf.
// A sufficient buffer size should be greater than the length of string,
// reserving space for null terminator.
// If bufsize is insufficient, the string will be truncated and null terminated.
// If buf is NULL, this method returns the length of the string (in 2-byte
// code units) via the result parameter.
// The result argument is optional unless buf is NULL.
napi_status napi_get_value_string_utf16(napi_env env,
                                        napi_value value,
                                        char16_t* buf,
                                        size_t bufsize,
                                        size_t* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, value);

  JSValueRef exception{};
  JSString string{ToJSString(env, value, &exception)};
  CHECK_JSC(env, exception);

  if (buf == nullptr) {
    *result = string.Length();
  } else {
    static_assert(sizeof(char16_t) == sizeof(JSChar));
    string.CopyTo(reinterpret_cast<JSChar*>(buf), bufsize, result);
  }

  return napi_ok;
}

napi_status napi_coerce_to_bool(napi_env env,
                                napi_value value,
                                napi_value* result) {
  CHECK_ARG(env, result);
  *result = ToNapi(JSValueMakeBoolean(env->context,
    JSValueToBoolean(env->context, ToJSValue(value))));
  return napi_ok;
}

napi_status napi_coerce_to_number(napi_env env,
                                  napi_value value,
                                  napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, value);
  CHECK_ARG(env, result);

  JSValueRef exception{};
  double number{JSValueToNumber(env->context, ToJSValue(value), &exception)};
  CHECK_JSC(env, exception);

  *result = ToNapi(JSValueMakeNumber(env->context, number));
  return napi_ok;
}

napi_status napi_coerce_to_object(napi_env env,
                                  napi_value value,
                                  napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, value);
  CHECK_ARG(env, result);

  JSValueRef exception{};
  *result = ToNapi(JSValueToObject(env->context, ToJSValue(value), &exception));
  CHECK_JSC(env, exception);

  return napi_ok;
}

napi_status napi_coerce_to_string(napi_env env,
                                  napi_value value,
                                  napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, value);
  CHECK_ARG(env, result);

  JSValueRef exception{};
  JSString string{ToJSString(env, value, &exception)};
  CHECK_JSC(env, exception);

  *result = ToNapi(JSValueMakeString(env->context, string));
  return napi_ok;
}

napi_status napi_wrap(napi_env env,
                      napi_value js_object,
                      void* native_object,
                      napi_finalize finalize_cb,
                      void* finalize_hint,
                      napi_ref* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, js_object);

  void* data{};
  CHECK_NAPI(napi_get_value_external(env, js_object, &data));
  RETURN_STATUS_IF_FALSE(env, data == nullptr, napi_invalid_arg);

  napi_value external{};
  CHECK_NAPI(napi_create_external(env, native_object, finalize_cb, finalize_hint, &external));

  JSValueRef exception{};
  JSObjectSetProperty(
    env->context,
    ToJSObject(env, js_object),
    JSString("__native__"),
    ToJSValue(external),
    kJSPropertyAttributeReadOnly | kJSPropertyAttributeDontEnum,
    &exception);
  CHECK_JSC(env, exception);

  if (result != nullptr) {
    CHECK_NAPI(napi_create_reference(env, external, 0, result));
  }

  return napi_ok;
}

napi_status napi_unwrap(napi_env env, napi_value js_object, void** result) {
  CHECK_ENV(env);
  CHECK_ARG(env, js_object);

  JSValueRef exception{};
  napi_value external{ToNapi(JSObjectGetProperty(
    env->context,
    ToJSObject(env, js_object),
    JSString("__native__"),
    &exception))};
  CHECK_JSC(env, exception);

  CHECK_NAPI(napi_get_value_external(env, external, result));
  return napi_ok;
}

napi_status napi_remove_wrap(napi_env env, napi_value js_object, void** result) {
  CHECK_ENV(env);
  CHECK_ARG(env, js_object);

  CHECK_NAPI(napi_unwrap(env, js_object, result));

  JSValueRef exception{};
  JSObjectDeleteProperty(
    env->context,
    ToJSObject(env, js_object),
    JSString("__native__"),
    &exception);
  CHECK_JSC(env, exception);

  return napi_ok;
}

napi_status napi_create_external(napi_env env,
                                 void* data,
                                 napi_finalize finalize_cb,
                                 void* finalize_hint,
                                 napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);

  External* external{new External(env, data, finalize_cb, finalize_hint)};
  if (external == nullptr) {
    return napi_set_last_error(env, napi_generic_failure);
  }

  *result = ToNapi(JSObjectMake(env->context, external->Class(), external));
  return napi_ok;
}

napi_status napi_get_value_external(napi_env env, napi_value value, void** result) {
  CHECK_ENV(env);
  CHECK_ARG(env, value);
  CHECK_ARG(env, result);

  External* external{reinterpret_cast<External*>(JSObjectGetPrivate(ToJSObject(env, value)))};
  *result = (external == nullptr ? nullptr : external->Data());
  return napi_ok;
}

// Set initial_refcount to 0 for a weak reference, >0 for a strong reference.
napi_status napi_create_reference(napi_env env,
                                  napi_value value,
                                  uint32_t initial_refcount,
                                  napi_ref* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, value);
  CHECK_ARG(env, result);

  RefInfo* info{new RefInfo{ToJSValue(value), initial_refcount}};
  if (info->count != 0) {
    JSValueProtect(env->context, info->value);
  }

  *result = reinterpret_cast<napi_ref>(info);
  return napi_ok;
}

// Deletes a reference. The referenced value is released, and may be GC'd
// unless there are other references to it.
napi_status napi_delete_reference(napi_env env, napi_ref ref) {
  CHECK_ENV(env);
  CHECK_ARG(env, ref);

  RefInfo* info{reinterpret_cast<RefInfo*>(ref)};
  if (info->count != 0) {
    JSValueUnprotect(env->context, info->value);
  }

  delete info;
  return napi_ok;
}

// Increments the reference count, optionally returning the resulting count.
// After this call the reference will be a strong reference because its refcount
// is >0, and the referenced object is effectively "pinned". Calling this when
// the refcount is 0 and the target is unavailable results in an error.
napi_status napi_reference_ref(napi_env env, napi_ref ref, uint32_t* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, ref);

  RefInfo* info{reinterpret_cast<RefInfo*>(ref)};
  if (info->count++ == 0) {
    JSValueProtect(env->context, info->value);
  }

  if (result != nullptr) {
    *result = info->count;
  }

  return napi_ok;
}

// Decrements the reference count, optionally returning the resulting count.
// If the result is 0 the reference is now weak and the object may be GC'd at
// any time if there are no other references. Calling this when the refcount
// is already 0 results in an error.
napi_status napi_reference_unref(napi_env env, napi_ref ref, uint32_t* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, ref);

  RefInfo* info{reinterpret_cast<RefInfo*>(ref)};
  if (--info->count == 0) {
    JSValueUnprotect(env->context, info->value);
  }

  if (result != nullptr) {
    *result = info->count;
  }

  return napi_ok;
}

// Attempts to get a referenced value. If the reference is weak, the value
// might no longer be available, in that case the call is still successful but
// the result is NULL.
napi_status napi_get_reference_value(napi_env env,
                                     napi_ref ref,
                                     napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, ref);
  CHECK_ARG(env, result);

  RefInfo* info{reinterpret_cast<RefInfo*>(ref)};

  if (info->count == 0) {
    *result = nullptr;
  } else {
    *result = ToNapi(info->value);
  }

  return napi_ok;
}

// Stub implementation of handle scope apis for JSC.
napi_status napi_open_handle_scope(napi_env env,
                                   napi_handle_scope* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);
  *result = reinterpret_cast<napi_handle_scope>(1);
  return napi_ok;
}

// Stub implementation of handle scope apis for JSC.
napi_status napi_close_handle_scope(napi_env env,
                                    napi_handle_scope scope) {
  CHECK_ENV(env);
  CHECK_ARG(env, scope);
  return napi_ok;
}

// Stub implementation of handle scope apis for JSC.
napi_status napi_open_escapable_handle_scope(napi_env env,
                                             napi_escapable_handle_scope* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);
  *result = reinterpret_cast<napi_escapable_handle_scope>(1);
  return napi_ok;
}

// Stub implementation of handle scope apis for JSC.
napi_status napi_close_escapable_handle_scope(napi_env env,
                                              napi_escapable_handle_scope scope) {
  CHECK_ENV(env);
  CHECK_ARG(env, scope);
  return napi_ok;
}

// Stub implementation of handle scope apis for JSC.
// This one will return escapee value as this is called from leveldown db.
napi_status napi_escape_handle(napi_env env,
                               napi_escapable_handle_scope scope,
                               napi_value escapee,
                               napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, scope);
  CHECK_ARG(env, escapee);
  CHECK_ARG(env, result);
  *result = escapee;
  return napi_ok;
}

napi_status napi_new_instance(napi_env env,
                              napi_value constructor,
                              size_t argc,
                              const napi_value* argv,
                              napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, constructor);
  if (argc > 0) {
    CHECK_ARG(env, argv);
  }
  CHECK_ARG(env, result);

  JSValueRef exception{};
  *result = ToNapi(JSObjectCallAsConstructor(
    env->context,
    ToJSObject(env, constructor),
    argc,
    ToJSValues(argv),
    &exception));
  CHECK_JSC(env, exception);

  return napi_ok;
}

napi_status napi_instanceof(napi_env env,
                            napi_value object,
                            napi_value constructor,
                            bool* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, object);
  CHECK_ARG(env, result);

  JSValueRef exception{};
  *result = JSValueIsInstanceOfConstructor(
    env->context,
    ToJSValue(object),
    ToJSObject(env, constructor),
    &exception);
  CHECK_JSC(env, exception);

  return napi_ok;
}

napi_status napi_is_exception_pending(napi_env env, bool* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);

  *result = (env->last_exception != nullptr);
  return napi_clear_last_error(env);
}

napi_status napi_get_and_clear_last_exception(napi_env env,
                                              napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);

  if (env->last_exception == nullptr) {
    return napi_get_undefined(env, result);
  } else {
    *result = ToNapi(env->last_exception);
    env->last_exception = nullptr;
  }

  return napi_clear_last_error(env);
}

napi_status napi_is_arraybuffer(napi_env env, napi_value value, bool* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, value);
  CHECK_ARG(env, result);

  JSValueRef exception{};
  JSTypedArrayType type{JSValueGetTypedArrayType(env->context, ToJSValue(value), &exception)};
  CHECK_JSC(env, exception);

  *result = (type == kJSTypedArrayTypeArrayBuffer);
  return napi_ok;
}

napi_status napi_create_arraybuffer(napi_env env,
                                    size_t byte_length,
                                    void** data,
                                    napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);

  *data = malloc(byte_length);
  JSValueRef exception{};
  *result = ToNapi(JSObjectMakeArrayBufferWithBytesNoCopy(
    env->context,
    *data,
    byte_length,
    [](void* bytes, void* deallocatorContext) {
      free(bytes);
    },
    nullptr,
    &exception));
  CHECK_JSC(env, exception);

  return napi_ok;
}

napi_status napi_create_external_arraybuffer(napi_env env,
                                             void* external_data,
                                             size_t byte_length,
                                             napi_finalize finalize_cb,
                                             void* finalize_hint,
                                             napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);

  External* info{new External(env, external_data, finalize_cb, finalize_hint)};
  if (info == nullptr) {
    return napi_set_last_error(env, napi_generic_failure);
  }

  JSValueRef exception{};
  *result = ToNapi(JSObjectMakeArrayBufferWithBytesNoCopy(
    env->context,
    external_data,
    byte_length,
    External::Finalize,
    info,
    &exception));
  CHECK_JSC(env, exception);

  return napi_ok;
}

napi_status napi_get_arraybuffer_info(napi_env env,
                                      napi_value arraybuffer,
                                      void** data,
                                      size_t* byte_length) {
  CHECK_ENV(env);
  CHECK_ARG(env, arraybuffer);

  JSValueRef exception{};

  if (data != nullptr) {
    *data = JSObjectGetArrayBufferBytesPtr(env->context, ToJSObject(env, arraybuffer), &exception);
    CHECK_JSC(env, exception);
  }

  if (byte_length != nullptr) {
    *byte_length = JSObjectGetArrayBufferByteLength(env->context, ToJSObject(env, arraybuffer), &exception);
    CHECK_JSC(env, exception);
  }

  return napi_ok;
}

napi_status napi_is_typedarray(napi_env env, napi_value value, bool* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, value);
  CHECK_ARG(env, result);

  JSValueRef exception{};
  JSTypedArrayType type{JSValueGetTypedArrayType(env->context, ToJSValue(value), &exception)};
  CHECK_JSC(env, exception);

  *result = (type != kJSTypedArrayTypeNone && type != kJSTypedArrayTypeArrayBuffer);
  return napi_ok;
}

napi_status napi_create_typedarray(napi_env env,
                                   napi_typedarray_type type,
                                   size_t length,
                                   napi_value arraybuffer,
                                   size_t byte_offset,
                                   napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, arraybuffer);
  CHECK_ARG(env, result);

  JSTypedArrayType jsType{};
  switch (type) {
    case napi_int8_array:
      jsType = kJSTypedArrayTypeInt8Array;
      break;
    case napi_uint8_array:
      jsType = kJSTypedArrayTypeUint8Array;
      break;
    case napi_uint8_clamped_array:
      jsType = kJSTypedArrayTypeUint8ClampedArray;
      break;
    case napi_int16_array:
      jsType = kJSTypedArrayTypeInt16Array;
      break;
    case napi_uint16_array:
      jsType = kJSTypedArrayTypeUint16Array;
      break;
    case napi_int32_array:
      jsType = kJSTypedArrayTypeInt32Array;
      break;
    case napi_uint32_array:
      jsType = kJSTypedArrayTypeUint32Array;
      break;
    case napi_float32_array:
      jsType = kJSTypedArrayTypeFloat32Array;
      break;
    case napi_float64_array:
      jsType = kJSTypedArrayTypeFloat64Array;
      break;
    default:
      return napi_set_last_error(env, napi_invalid_arg);
  }

  JSValueRef exception{};
  *result = ToNapi(JSObjectMakeTypedArrayWithArrayBufferAndOffset(
    env->context,
    jsType,
    ToJSObject(env, arraybuffer),
    byte_offset,
    length,
    &exception));
  CHECK_JSC(env, exception);

  return napi_ok;
}

napi_status napi_get_typedarray_info(napi_env env,
                                     napi_value typedarray,
                                     napi_typedarray_type* type,
                                     size_t* length,
                                     void** data,
                                     napi_value* arraybuffer,
                                     size_t* byte_offset) {
  CHECK_ENV(env);
  CHECK_ARG(env, typedarray);

  JSValueRef exception{};

  JSObjectRef object{ToJSObject(env, typedarray)};

  if (type != nullptr) {
    JSTypedArrayType typedArrayType{JSValueGetTypedArrayType(env->context, object, &exception)};
    CHECK_JSC(env, exception);

    switch (typedArrayType) {
      case kJSTypedArrayTypeInt8Array:
        *type = napi_int8_array;
        break;
      case kJSTypedArrayTypeUint8Array:
        *type = napi_uint8_array;
        break;
      case kJSTypedArrayTypeUint8ClampedArray:
        *type = napi_uint8_clamped_array;
        break;
      case kJSTypedArrayTypeInt16Array:
        *type = napi_int16_array;
        break;
      case kJSTypedArrayTypeUint16Array:
        *type = napi_uint16_array;
        break;
      case kJSTypedArrayTypeInt32Array:
        *type = napi_int32_array;
        break;
      case kJSTypedArrayTypeUint32Array:
        *type = napi_uint32_array;
        break;
      case kJSTypedArrayTypeFloat32Array:
        *type = napi_float32_array;
        break;
      case kJSTypedArrayTypeFloat64Array:
        *type = napi_float64_array;
        break;
      default:
        return napi_set_last_error(env, napi_generic_failure);
    }
  }

  if (length != nullptr) {
    *length = JSObjectGetTypedArrayByteLength(env->context, object, &exception);
    CHECK_JSC(env, exception);
  }

  if (data != nullptr) {
    *data = JSObjectGetTypedArrayBytesPtr(env->context, object, &exception);
    CHECK_JSC(env, exception);
  }

  if (arraybuffer != nullptr) {
    *arraybuffer = ToNapi(JSObjectGetTypedArrayBuffer(env->context, object, &exception));
    CHECK_JSC(env, exception);
  }

  if (byte_offset != nullptr) {
    *byte_offset = JSObjectGetTypedArrayByteOffset(env->context, object, &exception);
    CHECK_JSC(env, exception);
  }

  return napi_ok;
}

napi_status napi_create_dataview(napi_env env,
                                 size_t byte_length,
                                 napi_value arraybuffer,
                                 size_t byte_offset,
                                 napi_value* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, arraybuffer);
  CHECK_ARG(env, result);

  napi_value global{}, dataview_ctor{};
  CHECK_NAPI(napi_get_global(env, &global));
  CHECK_NAPI(napi_get_named_property(env, global, "DataView", &dataview_ctor));

  napi_value byte_offset_value{}, byte_length_value{};
  napi_create_double(env, static_cast<double>(byte_offset), &byte_offset_value);
  napi_create_double(env, static_cast<double>(byte_length), &byte_length_value);
  napi_value args[] = { arraybuffer, byte_offset_value, byte_length_value };
  CHECK_NAPI(napi_new_instance(env, dataview_ctor, 3, args, result));

  return napi_ok;
}

napi_status napi_is_dataview(napi_env env, napi_value value, bool* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, value);
  CHECK_ARG(env, result);

  napi_value global{}, dataview_ctor{};
  CHECK_NAPI(napi_get_global(env, &global));
  CHECK_NAPI(napi_get_named_property(env, global, "DataView", &dataview_ctor));
  CHECK_NAPI(napi_instanceof(env, value, dataview_ctor, result));

  return napi_ok;
}

napi_status napi_get_dataview_info(napi_env env,
                                   napi_value dataview,
                                   size_t* byte_length,
                                   void** data,
                                   napi_value* arraybuffer,
                                   size_t* byte_offset) {
  CHECK_ENV(env);
  CHECK_ARG(env, dataview);

  if (byte_length != nullptr) {
    napi_value value{};
    double doubleValue{};
    CHECK_NAPI(napi_get_named_property(env, dataview, "byteLength", &value));
    CHECK_NAPI(napi_get_value_double(env, value, &doubleValue));
    *byte_length = static_cast<size_t>(doubleValue);
  }

  if (data != nullptr) {
    napi_value value{};
    CHECK_NAPI(napi_get_named_property(env, dataview, "buffer", &value));
    CHECK_NAPI(napi_get_arraybuffer_info(env, value, data, nullptr));
  }

  if (arraybuffer != nullptr) {
    CHECK_NAPI(napi_get_named_property(env, dataview, "buffer", arraybuffer));
  }

  if (byte_offset != nullptr) {
    napi_value value{};
    double doubleValue{};
    CHECK_NAPI(napi_get_named_property(env, dataview, "byteOffset", &value));
    CHECK_NAPI(napi_get_value_double(env, value, &doubleValue));
    *byte_offset = static_cast<size_t>(doubleValue);
  }

  return napi_ok;
}

napi_status napi_get_version(napi_env env, uint32_t* result) {
  CHECK_ENV(env);
  CHECK_ARG(env, result);
  *result = NAPI_VERSION;
  return napi_ok;
}

napi_status napi_create_promise(napi_env env,
                                napi_deferred* deferred,
                                napi_value* promise) {
  CHECK_ENV(env);
  CHECK_ARG(env, deferred);
  CHECK_ARG(env, promise);

  napi_value global{}, promise_ctor{};
  CHECK_NAPI(napi_get_global(env, &global));
  CHECK_NAPI(napi_get_named_property(env, global, "Promise", &promise_ctor));

  struct Wrapper {
    napi_value resolve{};
    napi_value reject{};

    static napi_value Callback(napi_env env, napi_callback_info info) {
      CallbackInfo* callbackInfo = reinterpret_cast<CallbackInfo*>(info);
      Wrapper* wrapper = reinterpret_cast<Wrapper*>(callbackInfo->data);
      wrapper->resolve = callbackInfo->argv[0];
      wrapper->reject = callbackInfo->argv[1];
      return nullptr;
    }
  } wrapper;

  napi_value executor{};
  CHECK_NAPI(napi_create_function(env, "executor", NAPI_AUTO_LENGTH, Wrapper::Callback, &wrapper, &executor));
  CHECK_NAPI(napi_new_instance(env, promise_ctor, 1, &executor, promise));

  napi_value deferred_value{};
  CHECK_NAPI(napi_create_object(env, &deferred_value));
  CHECK_NAPI(napi_set_named_property(env, deferred_value, "resolve", wrapper.resolve));
  CHECK_NAPI(napi_set_named_property(env, deferred_value, "reject", wrapper.reject));

  napi_ref deferred_ref{};
  CHECK_NAPI(napi_create_reference(env, deferred_value, 1, &deferred_ref));
  *deferred = reinterpret_cast<napi_deferred>(deferred_ref);

  return napi_ok;
}

napi_status napi_resolve_deferred(napi_env env,
                                  napi_deferred deferred,
                                  napi_value resolution) {
  CHECK_ENV(env);
  CHECK_ARG(env, deferred);

  napi_ref deferred_ref{reinterpret_cast<napi_ref>(deferred)};
  napi_value undefined{}, deferred_value{}, resolve{};
  CHECK_NAPI(napi_get_undefined(env, &undefined));
  CHECK_NAPI(napi_get_reference_value(env, deferred_ref, &deferred_value));
  CHECK_NAPI(napi_get_named_property(env, deferred_value, "resolve", &resolve));
  CHECK_NAPI(napi_call_function(env, undefined, resolve, 1, &resolution, nullptr));
  CHECK_NAPI(napi_delete_reference(env, deferred_ref));

  return napi_ok;
}

napi_status napi_reject_deferred(napi_env env,
                                 napi_deferred deferred,
                                 napi_value rejection) {
  CHECK_ENV(env);
  CHECK_ARG(env, deferred);

  napi_ref deferred_ref{reinterpret_cast<napi_ref>(deferred)};
  napi_value undefined{}, deferred_value{}, reject{};
  CHECK_NAPI(napi_get_undefined(env, &undefined));
  CHECK_NAPI(napi_get_reference_value(env, deferred_ref, &deferred_value));
  CHECK_NAPI(napi_get_named_property(env, deferred_value, "reject", &reject));
  CHECK_NAPI(napi_call_function(env, undefined, reject, 1, &rejection, nullptr));
  CHECK_NAPI(napi_delete_reference(env, deferred_ref));

  return napi_ok;
}

napi_status napi_is_promise(napi_env env,
                            napi_value promise,
                            bool* is_promise) {
  CHECK_ARG(env, promise);
  CHECK_ARG(env, is_promise);

  napi_value global{}, promise_ctor{};
  CHECK_NAPI(napi_get_global(env, &global));
  CHECK_NAPI(napi_get_named_property(env, global, "Promise", &promise_ctor));
  CHECK_NAPI(napi_instanceof(env, promise, promise_ctor, is_promise));

  return napi_ok;
}

napi_status napi_run_script(napi_env env,
                            napi_value script,
                            napi_value* result) {
  CHECK_ARG(env, script);
  CHECK_ARG(env, result);

  JSValueRef exception{};

  JSString script_str{ToJSString(env, script, &exception)};
  CHECK_JSC(env, exception);

  *result = ToNapi(JSEvaluateScript(
    env->context, script_str, nullptr, nullptr, 0, &exception));
  CHECK_JSC(env, exception);

  return napi_ok;
}

napi_status napi_run_script(napi_env env,
                            napi_value script,
                            const char* source_url,
                            napi_value* result) {
  CHECK_ARG(env, script);
  CHECK_ARG(env, result);

  JSValueRef exception{};

  JSString script_str{ToJSString(env, script, &exception)};
  CHECK_JSC(env, exception);

  JSValueRef return_value{JSEvaluateScript(
    env->context, script_str, nullptr, JSString(source_url), 0, &exception)};
  CHECK_JSC(env, exception);

  if (result != nullptr) {
    *result = ToNapi(return_value);
  }

  return napi_ok;
}

napi_status napi_add_finalizer(napi_env env,
                               napi_value js_object,
                               void* native_object,
                               napi_finalize finalize_cb,
                               void* finalize_hint,
                               napi_ref* result) {
  return napi_ok;
}

napi_status napi_adjust_external_memory(napi_env env,
                                        int64_t change_in_bytes,
                                        int64_t* adjusted_value) {
  CHECK_ARG(env, adjusted_value);

  // TODO: Determine if JSC needs or is able to do anything here
  // For now, we can lie and say that we always adjusted more memory
  *adjusted_value = change_in_bytes;

  return napi_ok;
}
