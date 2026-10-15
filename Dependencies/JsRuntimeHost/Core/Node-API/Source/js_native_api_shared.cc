#include "js_native_api_shared.h"

#include <napi/js_native_api.h>

#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

namespace napi_shared {
  namespace {
    #define RETURN_IF_NOT_OK(expression)                    \
      do {                                                  \
        const napi_status status__{(expression)};           \
        if (status__ != napi_ok) {                          \
          return status__;                                  \
        }                                                   \
      } while (0)

    napi_status IsObjectLike(napi_env env, napi_value value, bool& result) {
      napi_valuetype type{};
      RETURN_IF_NOT_OK(napi_typeof(env, value, &type));
      result = (type == napi_object || type == napi_function || type == napi_external);
      return napi_ok;
    }

    // Whether `value` is strictly equal to something already in `seen`.
    napi_status Contains(napi_env env, const std::vector<napi_value>& seen, napi_value value, bool& result) {
      for (const napi_value candidate : seen) {
        bool equal{};
        RETURN_IF_NOT_OK(napi_strict_equals(env, candidate, value, &equal));
        if (equal) {
          result = true;
          return napi_ok;
        }
      }

      result = false;
      return napi_ok;
    }
  }

  napi_status ReleasePropertyNameIntrinsics(napi_env env, PropertyNameIntrinsics& intrinsics) {
    napi_status firstError{napi_ok};
    for (napi_ref* ref : {&intrinsics.object_constructor, &intrinsics.own_names,
                          &intrinsics.own_descriptor, &intrinsics.prototype}) {
      if (*ref != nullptr) {
        const napi_status status{napi_delete_reference(env, *ref)};
        if (status == napi_ok) {
          *ref = nullptr;
        } else if (firstError == napi_ok) {
          firstError = status;
        }
      }
    }
    return firstError;
  }

  napi_status CapturePropertyNameIntrinsics(napi_env env, PropertyNameIntrinsics& intrinsics) {
    napi_value global{};
    napi_value functions[4]{};
    RETURN_IF_NOT_OK(napi_get_global(env, &global));
    RETURN_IF_NOT_OK(napi_get_named_property(env, global, "Object", &functions[0]));
    RETURN_IF_NOT_OK(napi_get_named_property(env, functions[0], "getOwnPropertyNames", &functions[1]));
    RETURN_IF_NOT_OK(napi_get_named_property(env, functions[0], "getOwnPropertyDescriptor", &functions[2]));
    RETURN_IF_NOT_OK(napi_get_named_property(env, functions[0], "getPrototypeOf", &functions[3]));
    for (const napi_value function : functions) {
      napi_valuetype type{};
      RETURN_IF_NOT_OK(napi_typeof(env, function, &type));
      if (type != napi_function) {
        return napi_function_expected;
      }
    }

    napi_ref* refs[]{&intrinsics.object_constructor, &intrinsics.own_names,
                     &intrinsics.own_descriptor, &intrinsics.prototype};
    for (size_t index = 0; index < 4; ++index) {
      const napi_status status{napi_create_reference(env, functions[index], 1, refs[index])};
      if (status != napi_ok) {
        const napi_status cleanupStatus{ReleasePropertyNameIntrinsics(env, intrinsics)};
        return cleanupStatus == napi_ok ? status : cleanupStatus;
      }
    }
    return napi_ok;
  }

  napi_status GetEnumerablePropertyNames(napi_env env, napi_value object, napi_value* result,
                                         const PropertyNameIntrinsics& intrinsics) {
    // Take one own-key snapshot per prototype level, then inspect each
    // descriptor to determine enumerability. This matches `for...in` for
    // proxies, whose `ownKeys` trap must not be invoked twice at one level.
    napi_value global{};
    napi_value objectConstructor{};
    napi_value getOwnPropertyNames{};
    napi_value getOwnPropertyDescriptor{};
    napi_value getPrototypeOf{};
    RETURN_IF_NOT_OK(napi_get_global(env, &global));
    RETURN_IF_NOT_OK(napi_get_reference_value(env, intrinsics.object_constructor, &objectConstructor));
    RETURN_IF_NOT_OK(napi_get_reference_value(env, intrinsics.own_names, &getOwnPropertyNames));
    RETURN_IF_NOT_OK(napi_get_reference_value(env, intrinsics.own_descriptor, &getOwnPropertyDescriptor));
    RETURN_IF_NOT_OK(napi_get_reference_value(env, intrinsics.prototype, &getPrototypeOf));

    napi_value names{};
    RETURN_IF_NOT_OK(napi_create_array(env, &names));
    uint32_t nameCount{};

    std::unordered_set<std::u16string> shadowed{};
    std::vector<napi_value> visited{};

    // `ToObject` is what the specification (and the V8 implementation) applies
    // to the argument, so a primitive is wrapped and its properties reported.
    // `null` and `undefined` have no wrapper, and V8 reports that as
    // `napi_object_expected`; check explicitly rather than relying on
    // `napi_coerce_to_object`, whose behaviour for those two values differs
    // between engines (QuickJS yields an empty object, JavaScriptCore throws).
    napi_valuetype type{};
    RETURN_IF_NOT_OK(napi_typeof(env, object, &type));
    if (type == napi_null || type == napi_undefined) {
      return napi_object_expected;
    }

    napi_value current{object};
    if (type != napi_object && type != napi_function && type != napi_external) {
      RETURN_IF_NOT_OK(napi_call_function(env, global, objectConstructor, 1, &object, &current));
    }

    while (true) {
      bool isObjectLike{};
      RETURN_IF_NOT_OK(IsObjectLike(env, current, isObjectLike));
      if (!isObjectLike) {
        break;
      }

      bool alreadyVisited{};
      RETURN_IF_NOT_OK(Contains(env, visited, current, alreadyVisited));
      if (alreadyVisited) {
        RETURN_IF_NOT_OK(napi_throw_range_error(env, nullptr, "Cyclic prototype chain"));
        return napi_pending_exception;
      }
      visited.push_back(current);

      napi_value ownNames{};
      RETURN_IF_NOT_OK(napi_call_function(env, global, getOwnPropertyNames, 1, &current, &ownNames));

      uint32_t ownNameCount{};
      RETURN_IF_NOT_OK(napi_get_array_length(env, ownNames, &ownNameCount));
      for (uint32_t index = 0; index < ownNameCount; ++index) {
        napi_value name{};
        RETURN_IF_NOT_OK(napi_get_element(env, ownNames, index, &name));

        size_t length{};
        RETURN_IF_NOT_OK(napi_get_value_string_utf16(env, name, nullptr, 0, &length));
        std::u16string key(length + 1, u'\0');
        size_t copied{};
        RETURN_IF_NOT_OK(napi_get_value_string_utf16(env, name, key.data(), key.size(), &copied));
        key.resize(copied);
        if (shadowed.find(key) != shadowed.end()) {
          continue;
        }

        napi_value descriptorArgs[]{current, name};
        napi_value descriptor{};
        RETURN_IF_NOT_OK(napi_call_function(
            env, global, getOwnPropertyDescriptor, 2, descriptorArgs, &descriptor));

        napi_valuetype descriptorType{};
        RETURN_IF_NOT_OK(napi_typeof(env, descriptor, &descriptorType));
        if (descriptorType == napi_undefined) {
          continue;
        }

        shadowed.insert(std::move(key));

        napi_value enumerableValue{};
        RETURN_IF_NOT_OK(napi_get_named_property(env, descriptor, "enumerable", &enumerableValue));
        bool enumerable{};
        RETURN_IF_NOT_OK(napi_get_value_bool(env, enumerableValue, &enumerable));
        if (enumerable) {
          RETURN_IF_NOT_OK(napi_set_element(env, names, nameCount++, name));
        }
      }

      napi_value next{};
      RETURN_IF_NOT_OK(napi_call_function(env, global, getPrototypeOf, 1, &current, &next));

      current = next;
    }

    *result = names;
    return napi_ok;
  }

  #undef RETURN_IF_NOT_OK
}
