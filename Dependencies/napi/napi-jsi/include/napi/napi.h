#pragma once

#include <jsi/jsi.h>
#include <functional>
#include <initializer_list>
#include <memory>
#include <string>
#include <vector>
#include <optional>

// Copied from js_native_api.h
typedef enum {
  napi_default = 0,
  napi_writable = 1 << 0,
  napi_enumerable = 1 << 1,
  napi_configurable = 1 << 2,
} napi_property_attributes;

typedef enum {
  // ES6 types (corresponds to typeof)
  napi_undefined,
  napi_null,
  napi_boolean,
  napi_number,
  napi_string,
  napi_symbol,
  napi_object,
  napi_function,
  napi_external,
} napi_valuetype;

typedef enum {
  napi_int8_array,
  napi_uint8_array,
  napi_uint8_clamped_array,
  napi_int16_array,
  napi_uint16_array,
  napi_int32_array,
  napi_uint32_array,
  napi_float32_array,
  napi_float64_array,
} napi_typedarray_type;

struct napi_env__ {
  napi_env__(facebook::jsi::Runtime& rt)
    : rt{rt}
    , native_name{facebook::jsi::PropNameID::forAscii(rt, "__native__")}
    , array_buffer_ctor{rt.global().getPropertyAsFunction(rt, "ArrayBuffer")}
    , promise_ctor{rt.global().getPropertyAsFunction(rt, "Promise")}
    , typed_array_ctor{
        rt.global().getPropertyAsFunction(rt, "Int8Array"),
        rt.global().getPropertyAsFunction(rt, "Uint8Array"),
        rt.global().getPropertyAsFunction(rt, "Uint8ClampedArray"),
        rt.global().getPropertyAsFunction(rt, "Int16Array"),
        rt.global().getPropertyAsFunction(rt, "Uint16Array"),
        rt.global().getPropertyAsFunction(rt, "Int32Array"),
        rt.global().getPropertyAsFunction(rt, "Uint32Array"),
        rt.global().getPropertyAsFunction(rt, "Float32Array"),
        rt.global().getPropertyAsFunction(rt, "Float64Array")} {
  }

  facebook::jsi::Runtime& rt;
  facebook::jsi::PropNameID native_name;
  facebook::jsi::Function array_buffer_ctor;
  facebook::jsi::Function promise_ctor;
  facebook::jsi::Function typed_array_ctor[9];
};

using napi_env = napi_env__*;

#ifdef NAPI_DISABLE_CPP_EXCEPTIONS
  #error Exceptions cannot be disabled for NAPI/JSI
#endif

////////////////////////////////////////////////////////////////////////////////
/// N-API C++ Wrapper Classes
///
/// These classes wrap the "N-API" ABI-stable C APIs for Node.js, providing a
/// C++ object model and C++ exception-handling semantics with low overhead.
/// The wrappers are all header-only so that they do not affect the ABI.
////////////////////////////////////////////////////////////////////////////////
namespace Napi {

  using namespace facebook;

  // Forward declarations
  class Env;
  class Value;
  class Boolean;
  class Number;
  class String;
  class Object;
  class Array;
  class Function;
  class Error;
  //class PropertyDescriptor;
  class CallbackInfo;
  template <typename T> class Reference;
  class TypedArray;
  template <typename T> class TypedArrayOf;

  typedef TypedArrayOf<int8_t> Int8Array;     ///< Typed-array of signed 8-bit integers
  typedef TypedArrayOf<uint8_t> Uint8Array;   ///< Typed-array of unsigned 8-bit integers
  typedef TypedArrayOf<int16_t> Int16Array;   ///< Typed-array of signed 16-bit integers
  typedef TypedArrayOf<uint16_t> Uint16Array; ///< Typed-array of unsigned 16-bit integers
  typedef TypedArrayOf<int32_t> Int32Array;   ///< Typed-array of signed 32-bit integers
  typedef TypedArrayOf<uint32_t> Uint32Array; ///< Typed-array of unsigned 32-bit integers
  typedef TypedArrayOf<float> Float32Array;   ///< Typed-array of 32-bit floating-point values
  typedef TypedArrayOf<double> Float64Array;  ///< Typed-array of 64-bit floating-point values

  /// Environment for N-API values and operations.
  ///
  /// All N-API values and operations must be associated with an environment. An environment
  /// instance is always provided to callback functions; that environment must then be used for any
  /// creation of N-API values or other N-API operations within the callback. (Many methods infer
  /// the environment from the `this` instance that the method is called on.)
  ///
  /// In the future, multiple environments per process may be supported, although current
  /// implementations only support one environment per process.
  ///
  /// In the V8 JavaScript engine, a N-API environment approximately corresponds to an Isolate.
  class Env {
  public:
    Env(napi_env env);

    operator napi_env() const;

    Object Global() const;
    Value Undefined() const;
    Value Null() const;

  private:
    napi_env _env;
  };

  /// A JavaScript value of unknown type.
  ///
  /// For type-specific operations, convert to one of the Value subclasses using a `To*` or `As()`
  /// method. The `To*` methods do type coercion; the `As()` method does not.
  ///
  ///     Napi::Value value = ...
  ///     if (!value.IsString()) throw Napi::TypeError::New(env, "Invalid arg...");
  ///     Napi::String str = value.As<Napi::String>(); // Cast to a string value
  ///
  ///     Napi::Value anotherValue = ...
  ///     bool isTruthy = anotherValue.ToBoolean(); // Coerce to a boolean value
  class Value {
  public:
    Value();                               ///< Creates a new _empty_ Value instance.
    Value(napi_env env, jsi::Value value); ///< Wraps a N-API value primitive.

    // Move semantics
    Value(Value&&) = default;
    Value& operator =(Value&&) = default;
    operator jsi::Value&&() &&;

    // Copy semantics
    Value(const Value& other);
    Value& operator =(const Value& other);
    operator jsi::Value&() &;
    operator const jsi::Value&() const &;

    /// Creates a JS value from a C++ primitive.
    ///
    /// `value` may be any of:
    /// - bool
    /// - Any integer type
    /// - Any floating point type
    /// - const char* (encoded using UTF-8, null-terminated)
    /// - const char16_t* (encoded using UTF-16-LE, null-terminated)
    /// - std::string (encoded using UTF-8)
    /// - std::u16string
    /// - napi::Value
    /// - jsi::Value
    template <typename T>
    static Value From(napi_env env, const T& value);

    /// Tests if this value strictly equals another value.
    bool operator ==(const Value& other) const;

    /// Tests if this value does not strictly equal another value.
    bool operator !=(const Value& other) const;

    /// Tests if this value strictly equals another value.
    bool StrictEquals(const Value& other) const;

    /// Gets the environment the value is associated with.
    Napi::Env Env() const;

    /// Checks if the value is empty (uninitialized).
    ///
    /// An empty value is invalid, and most attempts to perform an operation on an empty value
    /// will result in an exception. Note an empty value is distinct from JavaScript `null` or
    /// `undefined`, which are valid values.
    ///
    /// When C++ exceptions are disabled at compile time, a method with a `Value` return type may
    /// return an empty value to indicate a pending exception. So when not using C++ exceptions,
    /// callers should check whether the value is empty before attempting to use it.
    bool IsEmpty() const;

    napi_valuetype Type() const; ///< Gets the type of the value.

    bool IsUndefined() const;   ///< Tests if a value is an undefined JavaScript value.
    bool IsNull() const;        ///< Tests if a value is a null JavaScript value.
    bool IsBoolean() const;     ///< Tests if a value is a JavaScript boolean.
    bool IsNumber() const;      ///< Tests if a value is a JavaScript number.
    bool IsString() const;      ///< Tests if a value is a JavaScript string.
    bool IsSymbol() const;      ///< Tests if a value is a JavaScript symbol.
    bool IsArray() const;       ///< Tests if a value is a JavaScript array.
    bool IsArrayBuffer() const; ///< Tests if a value is a JavaScript array buffer.
    bool IsTypedArray() const;  ///< Tests if a value is a JavaScript typed array.
    bool IsObject() const;      ///< Tests if a value is a JavaScript object.
    bool IsFunction() const;    ///< Tests if a value is a JavaScript function.
    bool IsPromise() const;     ///< Tests if a value is a JavaScript promise.
    bool IsDataView() const;    ///< Tests if a value is a JavaScript data view.
    bool IsExternal() const;    ///< Tests if a value is a pointer to external data.

    /// Casts to another type of `Napi::Value`, when the actual type is known or assumed.
    ///
    /// This conversion does NOT coerce the type. Calling any methods inappropriate for the actual
    /// value type will throw `Napi::Error`.
    template <typename T> T As() const;

    Boolean ToBoolean() const; ///< Coerces a value to a JavaScript boolean.
    Number ToNumber() const;   ///< Coerces a value to a JavaScript number.
    String ToString() const;   ///< Coerces a value to a JavaScript string.
    Object ToObject() const;   ///< Coerces a value to a JavaScript object.

  protected:
    /// !cond INTERNAL
    napi_env _env;
    jsi::Value _value;
    /// !endcond
  };

  /// A JavaScript boolean value.
  class Boolean : public Value {
  public:
    static Boolean New(
      napi_env env, ///< N-API environment
      bool value    ///< Boolean value
    );

    Boolean();                               ///< Creates a new _empty_ Boolean instance.
    Boolean(napi_env env, jsi::Value value); ///< Wraps a N-API value primitive.

    operator bool() const; ///< Converts a Boolean value to a boolean primitive.
    bool Value() const;    ///< Converts a Boolean value to a boolean primitive.
  };

  /// A JavaScript number value.
  class Number : public Value {
  public:
    static Number New(
      napi_env env, ///< N-API environment
      double value  ///< Number value
    );

    Number();                               ///< Creates a new _empty_ Number instance.
    Number(napi_env env, jsi::Value value); ///< Wraps a N-API value primitive.

    operator int32_t() const;  ///< Converts a Number value to a 32-bit signed integer value.
    operator uint32_t() const; ///< Converts a Number value to a 32-bit unsigned integer value.
    operator int64_t() const;  ///< Converts a Number value to a 64-bit signed integer value.
    operator float() const;    ///< Converts a Number value to a 32-bit floating-point value.
    operator double() const;   ///< Converts a Number value to a 64-bit floating-point value.

    int32_t Int32Value() const;   ///< Converts a Number value to a 32-bit signed integer value.
    uint32_t Uint32Value() const; ///< Converts a Number value to a 32-bit unsigned integer value.
    int64_t Int64Value() const;   ///< Converts a Number value to a 64-bit signed integer value.
    float FloatValue() const;     ///< Converts a Number value to a 32-bit floating-point value.
    double DoubleValue() const;   ///< Converts a Number value to a 64-bit floating-point value.
  };

  /// A JavaScript string or symbol value (that can be used as a property name).
  class Name : public Value {
  public:
    Name();                               ///< Creates a new _empty_ Name instance.
    Name(napi_env env, jsi::Value value); ///< Wraps a N-API value primitive.
  };

  /// A JavaScript string value.
  class String : public Name {
  public:
    /// Creates a new String value from a UTF-8 encoded C++ string.
    static String New(
      napi_env env,            ///< N-API environment
      const std::string& value ///< UTF-8 encoded C++ string
    );

    /// Creates a new String value from a UTF-16 encoded C++ string.
    static String New(
      napi_env env,               ///< N-API environment
      const std::u16string& value ///< UTF-16 encoded C++ string
    );

    /// Creates a new String value from a UTF-8 encoded C string.
    static String New(
      napi_env env,     ///< N-API environment
      const char* value ///< UTF-8 encoded null-terminated C string
    );

    /// Creates a new String value from a UTF-16 encoded C string.
    static String New(
      napi_env env,         ///< N-API environment
      const char16_t* value ///< UTF-16 encoded null-terminated C string
    );

    /// Creates a new String value from a UTF-8 encoded C string with specified length.
    static String New(
      napi_env env,      ///< N-API environment
      const char* value, ///< UTF-8 encoded C string (not necessarily null-terminated)
      size_t length      ///< length of the string in bytes
    );

    /// Creates a new String value from a UTF-16 encoded C string with specified length.
    static String New(
      napi_env env,          ///< N-API environment
      const char16_t* value, ///< UTF-16 encoded C string (not necessarily null-terminated)
      size_t length          ///< Length of the string in 2-byte code units
    );

    /// Creates a new String based on the original object's type.
    ///
    /// `value` may be any of:
    /// - const char* (encoded using UTF-8, null-terminated)
    /// - const char16_t* (encoded using UTF-16-LE, null-terminated)
    /// - std::string (encoded using UTF-8)
    /// - std::u16string
    template <typename T>
    static String From(napi_env env, const T& value);

    String();                               ///< Creates a new _empty_ String instance.
    String(napi_env env, jsi::Value value); ///< Wraps a N-API value primitive.

    operator std::string() const;      ///< Converts a String value to a UTF-8 encoded C++ string.
    operator std::u16string() const;   ///< Converts a String value to a UTF-16 encoded C++ string.
    std::string Utf8Value() const;     ///< Converts a String value to a UTF-8 encoded C++ string.
    std::u16string Utf16Value() const; ///< Converts a String value to a UTF-16 encoded C++ string.
  };

  /// A JavaScript symbol value.
  class Symbol : public Name {
  public:
    /// Creates a new Symbol value with an optional description.
    static Symbol New(
      napi_env env,                     ///< N-API environment
      const char* description = nullptr ///< Optional UTF-8 encoded null-terminated C string
                                        ///  describing the symbol
    );

    /// Creates a new Symbol value with a description.
    static Symbol New(
      napi_env env,                  ///< N-API environment
      const std::string& description ///< UTF-8 encoded C++ string describing the symbol
    );

    /// Creates a new Symbol value with a description.
    static Symbol New(
      napi_env env,      ///< N-API environment
      String description ///< String value describing the symbol
    );

    /// Creates a new Symbol value with a description.
    static Symbol New(
      napi_env env,          ///< N-API environment
      jsi::Value description ///< String value describing the symbol
    );

    /// Get a public Symbol (e.g. Symbol.iterator).
    static Symbol WellKnown(napi_env env, const std::string& name);

    Symbol();                               ///< Creates a new _empty_ Symbol instance.
    Symbol(napi_env env, jsi::Value value); ///< Wraps a N-API value primitive.
  };

  /// A JavaScript object value.
  class Object : public Value {
  public:
    /// Enables property and element assignments using indexing syntax.
    ///
    /// Example:
    ///
    ///     Napi::Value propertyValue = object1['A'];
    ///     object2['A'] = propertyValue;
    ///     Napi::Value elementValue = array[0];
    ///     array[1] = elementValue;
    template <typename Key>
    class PropertyLValue {
    public:
      /// Converts an L-value to a value.
      operator Value() const;

      /// Assigns a value to the property. The type of value can be
      /// anything supported by `Object::Set`.
      template <typename ValueType>
      PropertyLValue& operator =(ValueType value);

    private:
      PropertyLValue() = delete;
      PropertyLValue(Object& object, Key key);
      Object& _object;
      Key _key;

      friend class Napi::Object;
    };

    /// Creates a new Object value.
    static Object New(
      napi_env env ///< N-API environment
    );

    Object();                               ///< Creates a new _empty_ Object instance.
    Object(napi_env env, jsi::Value value); ///< Wraps a N-API value primitive.

    // Move semantics
    Object(Object&&) = default;
    Object& operator =(Object&&) = default;
    operator jsi::Object&&() &&;

    // Copy semantics
    Object(const Object& other);
    Object& operator =(const Object& other);
    operator jsi::Object&() &;
    operator const jsi::Object&() const &;

    /// Gets or sets a named property.
    PropertyLValue<std::string> operator [](
      const char* utf8name ///< UTF-8 encoded null-terminated property name
    );

    /// Gets or sets a named property.
    PropertyLValue<std::string> operator [](
      const std::string& utf8name ///< UTF-8 encoded property name
    );

    /// Gets or sets an indexed property or array element.
    PropertyLValue<uint32_t> operator [](
      uint32_t index /// Property / element index
    );

    /// Gets a named property.
    Value operator [](
      const char* utf8name ///< UTF-8 encoded null-terminated property name
    ) const;

    /// Gets a named property.
    Value operator [](
      const std::string& utf8name ///< UTF-8 encoded property name
    ) const;

    /// Gets an indexed property or array element.
    Value operator [](
      uint32_t index ///< Property / element index
    ) const;

    /// Checks whether a property is present.
    bool Has(
      Value key ///< Property key
    ) const;

    /// Checks whether a named property is present.
    bool Has(
      const char* utf8name ///< UTF-8 encoded null-terminated property name
    ) const;

    /// Checks whether a named property is present.
    bool Has(
      const std::string& utf8name ///< UTF-8 encoded property name
    ) const;

    /// Checks whether a own property is present.
    bool HasOwnProperty(
      Value key ///< Property key
    ) const;

    /// Checks whether a own property is present.
    bool HasOwnProperty(
      const char* utf8name ///< UTF-8 encoded null-terminated property name
    ) const;

    /// Checks whether a own property is present.
    bool HasOwnProperty(
      const std::string& utf8name ///< UTF-8 encoded property name
    ) const;

    /// Gets a property.
    Value Get(
      Value key ///< Property key
    ) const;

    /// Gets a named property.
    Value Get(
      const char* utf8name ///< UTF-8 encoded null-terminated property name
    ) const;

    /// Gets a named property.
    Value Get(
      const std::string& utf8name ///< UTF-8 encoded property name
    ) const;

    /// Sets a property.
    template <typename ValueType>
    void Set(
      Value key,  ///< Property key
      const ValueType& value ///< Property value
    );

    /// Sets a named property.
    template <typename ValueType>
    void Set(
      const char* utf8name, ///< UTF-8 encoded null-terminated property name
      const ValueType& value
    );

    /// Sets a named property.
    template <typename ValueType>
    void Set(
      const std::string& utf8name, ///< UTF-8 encoded property name
      const ValueType& value             ///< Property value primitive
    );

    /// Delete property.
    bool Delete(
      Value key ///< Property key
    );

    /// Delete property.
    bool Delete(
      const char* utf8name ///< UTF-8 encoded null-terminated property name
    );

    /// Delete property.
    bool Delete(
      const std::string& utf8name ///< UTF-8 encoded property name
    );

    /// Checks whether an indexed property is present.
    bool Has(
      uint32_t index ///< Property / element index
    ) const;

    /// Gets an indexed property or array element.
    Value Get(
      uint32_t index ///< Property / element index
    ) const;

    /// Sets an indexed property or array element.
    template <typename ValueType>
    void Set(
      uint32_t index,  ///< Property / element index
      const ValueType& value ///< Property value primitive
    );

    /// Deletes an indexed property or array element.
    bool Delete(
      uint32_t index ///< Property / element index
    );

    Array GetPropertyNames() const; ///< Get all property names

    ///// Defines a property on the object.
    //void DefineProperty(
    //  const PropertyDescriptor& property ///< Descriptor for the property to be defined
    //);

    ///// Defines properties on the object.
    //void DefineProperties(
    //  const std::initializer_list<PropertyDescriptor>& properties
    //    ///< List of descriptors for the properties to be defined
    //);

    ///// Defines properties on the object.
    //void DefineProperties(
    //  const std::vector<PropertyDescriptor>& properties
    //    ///< Vector of descriptors for the properties to be defined
    //);

    /// Checks if an object is an instance created by a constructor function.
    ///
    /// This is equivalent to the JavaScript `instanceof` operator.
    bool InstanceOf(
      const Function& constructor ///< Constructor function
    ) const;

  protected:
    std::optional<jsi::Object> _object;
  };

  template <typename T>
  class External : public Value {
  public:
    static External New(napi_env env, T* data);

    // Finalizer must implement `void operator()(Env env, T* data)`.
    template <typename Finalizer>
    static External New(napi_env env,
                        T* data,
                        Finalizer finalizeCallback);
    // Finalizer must implement `void operator()(Env env, T* data, Hint* hint)`.
    template <typename Finalizer, typename Hint>
    static External New(napi_env env,
                        T* data,
                        Finalizer finalizeCallback,
                        Hint* finalizeHint);

    External();
    External(napi_env env, jsi::Value value);

    T* Data() const;
  };

  class Array : public Object {
  public:
    static Array New(napi_env env);
    static Array New(napi_env env, size_t length);

    Array();
    Array(napi_env env, jsi::Value value);

    // Move semantics
    Array(Array&&) = default;
    Array& operator =(Array&&) = default;
    operator jsi::Array&&() &&;

    // Copy semantics
    Array(const Array& other);
    Array& operator =(const Array& other);
    operator jsi::Array&() &;
    operator const jsi::Array&() const &;

    uint32_t Length() const;

  private:
    std::optional<jsi::Array> _array;
  };

  /// A JavaScript array buffer value.
  class ArrayBuffer : public Object {
  public:
    /// Creates a new ArrayBuffer instance over a new automatically-allocated buffer.
    static ArrayBuffer New(
      napi_env env,     ///< N-API environment
      size_t byteLength ///< Length of the buffer to be allocated, in bytes
    );

    /// Creates a new ArrayBuffer instance, using an external buffer with specified byte length.
    static ArrayBuffer New(
      napi_env env,       ///< N-API environment
      void* externalData, ///< Pointer to the external buffer to be used by the array
      size_t byteLength   ///< Length of the external buffer to be used by the array, in bytes
    );

    /// Creates a new ArrayBuffer instance, using an external buffer with specified byte length.
    template <typename Finalizer>
    static ArrayBuffer New(
      napi_env env,              ///< N-API environment
      void* externalData,        ///< Pointer to the external buffer to be used by the array
      size_t byteLength,         ///< Length of the external buffer to be used by the array,
                                 ///  in bytes
      Finalizer finalizeCallback ///< Function to be called when the array buffer is destroyed;
                                 ///  must implement `void operator()(Env env, void* externalData)`
    );

    /// Creates a new ArrayBuffer instance, using an external buffer with specified byte length.
    template <typename Finalizer, typename Hint>
    static ArrayBuffer New(
      napi_env env,               ///< N-API environment
      void* externalData,         ///< Pointer to the external buffer to be used by the array
      size_t byteLength,          ///< Length of the external buffer to be used by the array,
                                  ///  in bytes
      Finalizer finalizeCallback, ///< Function to be called when the array buffer is destroyed;
                                  ///  must implement `void operator()(Env env, void* externalData, Hint* hint)`
      Hint* finalizeHint          ///< Hint (second parameter) to be passed to the finalize callback
    );

    ArrayBuffer();                               ///< Creates a new _empty_ ArrayBuffer instance.
    ArrayBuffer(napi_env env, jsi::Value value); ///< Wraps a N-API value primitive.

    // Move semantics
    ArrayBuffer(ArrayBuffer&&) = default;
    ArrayBuffer& operator =(ArrayBuffer&&) = default;
    operator jsi::ArrayBuffer&&() &&;

    // Copy semantics
    ArrayBuffer(const ArrayBuffer& other);
    ArrayBuffer& operator =(const ArrayBuffer& other);
    operator jsi::ArrayBuffer&() &;
    operator const jsi::ArrayBuffer&() const &;

    void* Data() const;        ///< Gets a pointer to the data buffer.
    size_t ByteLength() const; ///< Gets the length of the array buffer in bytes.

  private:
    std::optional<jsi::ArrayBuffer> _arrayBuffer;
  };

  /// A JavaScript typed-array value with unknown array type.
  ///
  /// For type-specific operations, cast to a `TypedArrayOf<T>` instance using the `As()`
  /// method:
  ///
  ///     Napi::TypedArray array = ...
  ///     if (t.TypedArrayType() == napi_int32_array) {
  ///         Napi::Int32Array int32Array = t.As<Napi::Int32Array>();
  ///     }
  class TypedArray : public Object {
  public:
    TypedArray();                               ///< Creates a new _empty_ TypedArray instance.
    TypedArray(napi_env env, jsi::Value value); ///< Wraps a N-API value primitive.

    napi_typedarray_type TypedArrayType() const; ///< Gets the type of this typed-array.
    Napi::ArrayBuffer ArrayBuffer() const;       ///< Gets the backing array buffer.

    uint8_t ElementSize() const;  ///< Gets the size in bytes of one element in the array.
    size_t ElementLength() const; ///< Gets the number of elements in the array.
    size_t ByteOffset() const;    ///< Gets the offset into the buffer where the array starts.
    size_t ByteLength() const;    ///< Gets the length of the array in bytes.

  protected:
    /// !cond INTERNAL
    TypedArray(napi_env env, jsi::Value value, napi_typedarray_type type, size_t length);

    mutable napi_typedarray_type _type;
    mutable size_t _length;

    static const napi_typedarray_type unknown_array_type = static_cast<napi_typedarray_type>(-1);

    template <typename T>
    static
    constexpr napi_typedarray_type TypedArrayTypeForPrimitiveType() {
      return std::is_same<T, int8_t>::value ? napi_int8_array
        : std::is_same<T, uint8_t>::value ? napi_uint8_array
        : std::is_same<T, int16_t>::value ? napi_int16_array
        : std::is_same<T, uint16_t>::value ? napi_uint16_array
        : std::is_same<T, int32_t>::value ? napi_int32_array
        : std::is_same<T, uint32_t>::value ? napi_uint32_array
        : std::is_same<T, float>::value ? napi_float32_array
        : std::is_same<T, double>::value ? napi_float64_array
        : unknown_array_type;
    }

    static void GetTypedArrayInfo(napi_env env,
                                  const jsi::Object& typedArray,
                                  napi_typedarray_type* type,
                                  size_t* length);
    /// !endcond
  };

  /// A JavaScript typed-array value with known array type.
  ///
  /// Note while it is possible to create and access Uint8 "clamped" arrays using this class,
  /// the _clamping_ behavior is only applied in JavaScript.
  template <typename T>
  class TypedArrayOf : public TypedArray {
  public:
    /// Creates a new TypedArray instance over a new automatically-allocated array buffer.
    ///
    /// The array type parameter can normally be omitted (because it is inferred from the template
    /// parameter T), except when creating a "clamped" array:
    ///
    ///     Uint8Array::New(env, length, napi_uint8_clamped_array)
    static TypedArrayOf New(
      napi_env env,         ///< N-API environment
      size_t elementLength, ///< Length of the created array, as a number of elements
      napi_typedarray_type type = TypedArray::TypedArrayTypeForPrimitiveType<T>() ///< Type of array, if different from the default array type for the template parameter T.
    );

    /// Creates a new TypedArray instance over a provided array buffer.
    ///
    /// The array type parameter can normally be omitted (because it is inferred from the template
    /// parameter T), except when creating a "clamped" array:
    ///
    ///     Uint8Array::New(env, length, buffer, 0, napi_uint8_clamped_array)
    static TypedArrayOf New(
      napi_env env,                  ///< N-API environment
      size_t elementLength,          ///< Length of the created array, as a number of elements
      Napi::ArrayBuffer arrayBuffer, ///< Backing array buffer instance to use
      size_t bufferOffset,           ///< Offset into the array buffer where the typed-array starts
      napi_typedarray_type type = TypedArray::TypedArrayTypeForPrimitiveType<T>() ///< Type of array, if different from the default array type for the template parameter T.
    );

    TypedArrayOf();                               ///< Creates a new _empty_ TypedArrayOf instance.
    TypedArrayOf(napi_env env, jsi::Value value); ///< Wraps a N-API value primitive.

    T& operator [](size_t index);             ///< Gets or sets an element in the array.
    const T& operator [](size_t index) const; ///< Gets an element in the array.

    /// Gets a pointer to the array's backing buffer.
    ///
    /// This is not necessarily the same as the `ArrayBuffer::Data()` pointer, because the
    /// typed-array may have a non-zero `ByteOffset()` into the `ArrayBuffer`.
    T* Data();

    /// Gets a pointer to the array's backing buffer.
    ///
    /// This is not necessarily the same as the `ArrayBuffer::Data()` pointer, because the
    /// typed-array may have a non-zero `ByteOffset()` into the `ArrayBuffer`.
    const T* Data() const;

  private:
    T* _data;

    TypedArrayOf(napi_env env,
                 jsi::Value value,
                 napi_typedarray_type type,
                 size_t length,
                 T* data);
  };

  /// The DataView provides a low-level interface for reading/writing multiple
  /// number types in an ArrayBuffer irrespective of the platform's endianness.
  class DataView : public Object {
  public:
    static DataView New(napi_env env,
                        Napi::ArrayBuffer arrayBuffer);
    static DataView New(napi_env env,
                        Napi::ArrayBuffer arrayBuffer,
                        size_t byteOffset);
    static DataView New(napi_env env,
                        Napi::ArrayBuffer arrayBuffer,
                        size_t byteOffset,
                        size_t byteLength);

    DataView(napi_env env, jsi::Value value); ///< Wraps a N-API value primitive.

    Napi::ArrayBuffer ArrayBuffer() const;    ///< Gets the backing array buffer.
    size_t ByteOffset() const;    ///< Gets the offset into the buffer where the array starts.
    size_t ByteLength() const;    ///< Gets the length of the array in bytes.

    void* Data() const;

    float GetFloat32(size_t byteOffset) const;
    double GetFloat64(size_t byteOffset) const;
    int8_t GetInt8(size_t byteOffset) const;
    int16_t GetInt16(size_t byteOffset) const;
    int32_t GetInt32(size_t byteOffset) const;
    uint8_t GetUint8(size_t byteOffset) const;
    uint16_t GetUint16(size_t byteOffset) const;
    uint32_t GetUint32(size_t byteOffset) const;

    void SetFloat32(size_t byteOffset, float value) const;
    void SetFloat64(size_t byteOffset, double value) const;
    void SetInt8(size_t byteOffset, int8_t value) const;
    void SetInt16(size_t byteOffset, int16_t value) const;
    void SetInt32(size_t byteOffset, int32_t value) const;
    void SetUint8(size_t byteOffset, uint8_t value) const;
    void SetUint16(size_t byteOffset, uint16_t value) const;
    void SetUint32(size_t byteOffset, uint32_t value) const;

  private:
    template <typename T>
    T ReadData(size_t byteOffset) const;

    template <typename T>
    void WriteData(size_t byteOffset, T value) const;

    void* _data;
    size_t _length;
  };

  class Function : public Object {
  public:
    /// Callable must implement operator() accepting a const CallbackInfo&
    /// and return either void or Value.
    template <typename Callable>
    static Function New(napi_env env,
                        Callable cb,
                        const char* utf8name = nullptr,
                        void* data = nullptr);
    /// Callable must implement operator() accepting a const CallbackInfo&
    /// and return either void or Value.
    template <typename Callable>
    static Function New(napi_env env,
                        Callable cb,
                        const std::string& utf8name,
                        void* data = nullptr);

    Function();
    Function(napi_env env, jsi::Value value);

    // Move semantics
    Function(Function&&) = default;
    Function& operator =(Function&&) = default;
    operator jsi::Function&&() &&;

    // Copy semantics
    Function(const Function& other);
    Function& operator =(const Function& other);
    operator jsi::Function&() &;
    operator const jsi::Function&() const &;

    Value operator ()(const std::initializer_list<Value>& args) const;

    Value Call(const std::initializer_list<Value>& args) const;
    Value Call(const std::vector<Value>& args) const;
    Value Call(size_t argc, const Value* args) const;
    Value Call(const Value& recv, const std::initializer_list<Value>& args) const;
    Value Call(const Value& recv, const std::vector<Value>& args) const;
    Value Call(const Value& recv, size_t argc, const Value* args) const;

    Object New(const std::initializer_list<Value>& args) const;
    Object New(const std::vector<Value>& args) const;
    Object New(size_t argc, const Value* args) const;

  private:
    std::optional<jsi::Function> _function;
  };

  class Promise : public Object {
  public:
    class Deferred {
    public:
      static Deferred New(napi_env env);
      Deferred(napi_env env);

      // Move semantics
      Deferred(Deferred&&) = default;
      Deferred& operator =(Deferred&&) = default;

      // Copy semantics
      Deferred(const Deferred& other);
      Deferred& operator =(const Deferred& other);

      Napi::Promise Promise() const;
      Napi::Env Env() const;

      void Resolve(const jsi::Value& value) const;
      void Reject(const jsi::Value& value) const;

    private:
      napi_env _env;
      jsi::Value _promise;
      std::optional<jsi::Function> _resolve;
      std::optional<jsi::Function> _reject;
    };

    Promise(napi_env env, jsi::Value value);
  };

  /// Holds a counted reference to a value; initially a weak reference unless otherwise specified,
  /// may be changed to/from a strong reference by adjusting the refcount.
  ///
  /// The referenced value is not immediately destroyed when the reference count is zero; it is
  /// merely then eligible for garbage-collection if there are no other references to the value.
  ///
  /// NOTE: This class is implemented as a no-op for JSI.
  template <typename T>
  class Reference {
  public:
    static Reference<T> New(const T& object, uint32_t initialRefcount = 0);

    Reference();
    Reference(napi_env env, T object);

    // A reference can be moved but cannot be copied.
    Reference(Reference<T>&& other);
    Reference<T>& operator =(Reference<T>&& other);

    bool operator ==(const Reference<T> &other) const;
    bool operator !=(const Reference<T> &other) const;
    operator bool() const;

    Napi::Env Env() const;
    bool IsEmpty() const;

    // Note when getting the value of a Reference it is usually correct to do so
    // within a HandleScope so that the value handle gets cleaned up efficiently.
    const T& Value() const;
    T& Value();

    uint32_t Ref();
    uint32_t Unref();
    void Reset();
    void Reset(const T& value, uint32_t refcount = 0);

    // Call this on a reference that is declared as static data, to prevent its destructor
    // from running at program shutdown time, which would attempt to reset the reference when
    // the environment is no longer valid.
    // NOTE: This is no-op for JSI.
    void SuppressDestruct();

  protected:
    Reference(const Reference<T>&);

    /// !cond INTERNAL
    napi_env _env;
    T _object;
    uint32_t _refcount;
    /// !endcond
  };

  class ObjectReference : public Reference<Object> {
  public:
    ObjectReference();
    ObjectReference(napi_env env, jsi::Object object);

    // A reference can be moved but cannot be copied.
    ObjectReference(Reference<Object>&& other);
    ObjectReference& operator =(Reference<Object>&& other);
    ObjectReference(ObjectReference&& other);
    ObjectReference& operator =(ObjectReference&& other);

    Napi::Value Get(const char* utf8name) const;
    Napi::Value Get(const std::string& utf8name) const;
    void Set(const char* utf8name, Napi::Value value);
    void Set(const char* utf8name, const char* utf8value);
    void Set(const char* utf8name, bool boolValue);
    void Set(const char* utf8name, double numberValue);
    void Set(const std::string& utf8name, Napi::Value value);
    void Set(const std::string& utf8name, std::string& utf8value);
    void Set(const std::string& utf8name, bool boolValue);
    void Set(const std::string& utf8name, double numberValue);

    Napi::Value Get(uint32_t index) const;
    void Set(uint32_t index, const Napi::Value value);
    void Set(uint32_t index, const char* utf8value);
    void Set(uint32_t index, const std::string& utf8value);
    void Set(uint32_t index, bool boolValue);
    void Set(uint32_t index, double numberValue);

  protected:
    ObjectReference(const ObjectReference&);
  };

  class FunctionReference : public Reference<Function> {
  public:
    FunctionReference();
    FunctionReference(napi_env env, jsi::Function function);

    // A reference can be moved but cannot be copied.
    FunctionReference(Reference<Function>&& other);
    FunctionReference& operator =(Reference<Function>&& other);
    FunctionReference(FunctionReference&& other);
    FunctionReference& operator =(FunctionReference&& other);

    Napi::Value operator ()(const std::initializer_list<Napi::Value>& args) const;

    Napi::Value Call(const std::initializer_list<Napi::Value>& args) const;
    Napi::Value Call(const std::vector<Napi::Value>& args) const;
    Napi::Value Call(const Napi::Value& recv, const std::initializer_list<Napi::Value>& args) const;
    Napi::Value Call(const Napi::Value& recv, const std::vector<Napi::Value>& args) const;
    Napi::Value Call(const Napi::Value& recv, size_t argc, const Napi::Value* args) const;

    Object New(const std::initializer_list<Napi::Value>& args) const;
    Object New(const std::vector<Napi::Value>& args) const;
  };

  // Shortcuts to creating a new reference with inferred type and refcount = 0.
  template <typename T> Reference<T> Weak(T value);
  ObjectReference Weak(Object value);
  FunctionReference Weak(Function value);

  // Shortcuts to creating a new reference with inferred type and refcount = 1.
  template <typename T> Reference<T> Persistent(T value);
  ObjectReference Persistent(Object value);
  FunctionReference Persistent(Function value);

  /// A persistent reference to a JavaScript error object. Use of this class depends somewhat
  /// on whether C++ exceptions are enabled at compile time.
  ///
  /// ### Handling Errors With C++ Exceptions
  ///
  /// If C++ exceptions are enabled, then the `Error` class extends `std::exception` and enables
  /// integrated error-handling for C++ exceptions and JavaScript exceptions.
  ///
  /// If a N-API call fails without executing any JavaScript code (for example due to an invalid
  /// argument), then the N-API wrapper automatically converts and throws the error as a C++
  /// exception of type `Napi::Error`. Or if a JavaScript function called by C++ code via N-API
  /// throws a JavaScript exception, then the N-API wrapper automatically converts and throws it as
  /// a C++ exception of type `Napi::Error`.
  ///
  /// If a C++ exception of type `Napi::Error` escapes from a N-API C++ callback, then the N-API
  /// wrapper automatically converts and throws it as a JavaScript exception. Therefore, catching
  /// a C++ exception of type `Napi::Error` prevents a JavaScript exception from being thrown.
  ///
  /// #### Example 1A - Throwing a C++ exception:
  ///
  ///     Napi::Env env = ...
  ///     throw Napi::Error::New(env, "Example exception");
  ///
  /// Following C++ statements will not be executed. The exception will bubble up as a C++
  /// exception of type `Napi::Error`, until it is either caught while still in C++, or else
  /// automatically propataged as a JavaScript exception when the callback returns to JavaScript.
  ///
  /// #### Example 2A - Propagating a N-API C++ exception:
  ///
  ///     Napi::Function jsFunctionThatThrows = someObj.As<Napi::Function>();
  ///     Napi::Value result = jsFunctionThatThrows({ arg1, arg2 });
  ///
  /// Following C++ statements will not be executed. The exception will bubble up as a C++
  /// exception of type `Napi::Error`, until it is either caught while still in C++, or else
  /// automatically propagated as a JavaScript exception when the callback returns to JavaScript.
  ///
  /// #### Example 3A - Handling a N-API C++ exception:
  ///
  ///     Napi::Function jsFunctionThatThrows = someObj.As<Napi::Function>();
  ///     Napi::Value result;
  ///     try {
  ///        result = jsFunctionThatThrows({ arg1, arg2 });
  ///     } catch (const Napi::Error& e) {
  ///       cerr << "Caught JavaScript exception: " + e.what();
  ///     }
  ///
  /// Since the exception was caught here, it will not be propagated as a JavaScript exception.
  class Error : public ObjectReference, public std::exception
    {
  public:
    static Error New(napi_env env, const char* message);
    static Error New(napi_env env, const std::string& message);

    static Error New(napi_env env, const std::exception& exception);
    static Error New(napi_env env, const std::exception_ptr& exception_ptr);

    static void Fatal(const char* location, const char* message);

    Error();
    Error(napi_env env, jsi::Object object);

    // An error can be moved or copied.
    Error(Error&& other);
    Error& operator =(Error&& other);
    Error(const Error&);
    Error& operator =(Error&);

    const std::string& Message() const;
    void ThrowAsJavaScriptException() const;

    const char* what() const noexcept override;

  protected:
    /// !cond INTERNAL
    template <typename TError, typename TMessage>
    static TError New(napi_env env, TMessage message, const char* constructor);
    /// !endcond

  private:
    mutable std::string _message;
  };

  class TypeError : public Error {
  public:
    static TypeError New(napi_env env, const char* message);
    static TypeError New(napi_env env, const std::string& message);

    TypeError();
    TypeError(napi_env env, jsi::Object object);
  };

  class RangeError : public Error {
  public:
    static RangeError New(napi_env env, const char* message);
    static RangeError New(napi_env env, const std::string& message);

    RangeError();
    RangeError(napi_env env, jsi::Object object);
  };

  class CallbackInfo {
  public:
    CallbackInfo(napi_env env, const jsi::Value& thisVal, const jsi::Value* args, size_t argc, const jsi::Value& newTarget, void* data);
    ~CallbackInfo();

    // Disallow copying to prevent multiple free of _dynamicArgs
    CallbackInfo(CallbackInfo const &) = delete;
    void operator=(CallbackInfo const &) = delete;

    Napi::Env Env() const;
    Value NewTarget() const;
    bool IsConstructCall() const;
    size_t Length() const;
    const Value operator [](size_t index) const;
    Value This() const;
    void* Data() const;
    void SetData(void* data);

  private:
    napi_env _env;
    jsi::Value _this;
    size_t _argc;
    jsi::Value* _argv;
    jsi::Value _staticArgs[6];
    jsi::Value* _dynamicArgs;
    jsi::Value _newTarget;
    void* _data;
  };

  // TODO: not implemented
  //class PropertyDescriptor {
  //public:
  //  template <typename Getter>
  //  static PropertyDescriptor Accessor(Napi::Env env,
  //                                     Napi::Object object,
  //                                     const char* utf8name,
  //                                     Getter getter,
  //                                     napi_property_attributes attributes = napi_default,
  //                                     void* data = nullptr);
  //  template <typename Getter>
  //  static PropertyDescriptor Accessor(Napi::Env env,
  //                                     Napi::Object object,
  //                                     const std::string& utf8name,
  //                                     Getter getter,
  //                                     napi_property_attributes attributes = napi_default,
  //                                     void* data = nullptr);
  //  template <typename Getter>
  //  static PropertyDescriptor Accessor(Napi::Env env,
  //                                     Napi::Object object,
  //                                     Name name,
  //                                     Getter getter,
  //                                     napi_property_attributes attributes = napi_default,
  //                                     void* data = nullptr);
  //  template <typename Getter, typename Setter>
  //  static PropertyDescriptor Accessor(Napi::Env env,
  //                                     Napi::Object object,
  //                                     const char* utf8name,
  //                                     Getter getter,
  //                                     Setter setter,
  //                                     napi_property_attributes attributes = napi_default,
  //                                     void* data = nullptr);
  //  template <typename Getter, typename Setter>
  //  static PropertyDescriptor Accessor(Napi::Env env,
  //                                     Napi::Object object,
  //                                     const std::string& utf8name,
  //                                     Getter getter,
  //                                     Setter setter,
  //                                     napi_property_attributes attributes = napi_default,
  //                                     void* data = nullptr);
  //  template <typename Getter, typename Setter>
  //  static PropertyDescriptor Accessor(Napi::Env env,
  //                                     Napi::Object object,
  //                                     Name name,
  //                                     Getter getter,
  //                                     Setter setter,
  //                                     napi_property_attributes attributes = napi_default,
  //                                     void* data = nullptr);
  //  template <typename Callable>
  //  static PropertyDescriptor Function(Napi::Env env,
  //                                     Napi::Object object,
  //                                     const char* utf8name,
  //                                     Callable cb,
  //                                     napi_property_attributes attributes = napi_default,
  //                                     void* data = nullptr);
  //  template <typename Callable>
  //  static PropertyDescriptor Function(Napi::Env env,
  //                                     Napi::Object object,
  //                                     const std::string& utf8name,
  //                                     Callable cb,
  //                                     napi_property_attributes attributes = napi_default,
  //                                     void* data = nullptr);
  //  template <typename Callable>
  //  static PropertyDescriptor Function(Napi::Env env,
  //                                     Napi::Object object,
  //                                     Name name,
  //                                     Callable cb,
  //                                     napi_property_attributes attributes = napi_default,
  //                                     void* data = nullptr);
  //  static PropertyDescriptor Value(const char* utf8name,
  //                                  jsi::Value value,
  //                                  napi_property_attributes attributes = napi_default);
  //  static PropertyDescriptor Value(const std::string& utf8name,
  //                                  jsi::Value value,
  //                                  napi_property_attributes attributes = napi_default);
  //  static PropertyDescriptor Value(jsi::Value name,
  //                                  jsi::Value value,
  //                                  napi_property_attributes attributes = napi_default);
  //  static PropertyDescriptor Value(Name name,
  //                                  Napi::Value value,
  //                                  napi_property_attributes attributes = napi_default);
  //private:
  //  napi_property_descriptor _desc;
  //};

  /// Base class to be extended by C++ classes exposed to JavaScript; each C++ class instance gets
  /// "wrapped" by a JavaScript object that is managed by this class.
  ///
  /// At initialization time, the `DefineClass()` method must be used to
  /// hook up the accessor and method callbacks. It takes a list of
  /// property descriptors, which can be constructed via the various
  /// static methods on the base class.
  ///
  /// #### Example:
  ///
  ///     class Example: public Napi::ObjectWrap<Example> {
  ///       public:
  ///         static void Initialize(Napi::Env& env, Napi::Object& target) {
  ///           Napi::Function constructor = DefineClass(env, "Example", {
  ///             InstanceAccessor("value", &Example::GetSomething, &Example::SetSomething),
  ///             InstanceMethod("doSomething", &Example::DoSomething),
  ///           });
  ///           target.Set("Example", constructor);
  ///         }
  ///
  ///         Example(const Napi::CallbackInfo& info); // Constructor
  ///         Napi::Value GetSomething(const Napi::CallbackInfo& info);
  ///         void SetSomething(const Napi::CallbackInfo& info, const Napi::Value& value);
  ///         Napi::Value DoSomething(const Napi::CallbackInfo& info);
  ///     }
  template <typename T>
  class ObjectWrap : public jsi::HostObject {
  public:
    ObjectWrap(const CallbackInfo& callbackInfo);
    virtual ~ObjectWrap();

    Napi::Env Env() const;

    static T* Unwrap(Object wrapper);

    // Methods exposed to JavaScript must conform to one of these callback signatures.
    typedef void (*StaticVoidMethodCallback)(const CallbackInfo& info);
    typedef Napi::Value (*StaticMethodCallback)(const CallbackInfo& info);
    typedef Napi::Value (*StaticGetterCallback)(const CallbackInfo& info);
    typedef void (*StaticSetterCallback)(const CallbackInfo& info, const Napi::Value& value);
    typedef void (T::*InstanceVoidMethodCallback)(const CallbackInfo& info);
    typedef Napi::Value (T::*InstanceMethodCallback)(const CallbackInfo& info);
    typedef Napi::Value (T::*InstanceGetterCallback)(const CallbackInfo& info);
    typedef void (T::*InstanceSetterCallback)(const CallbackInfo& info, const Napi::Value& value);

    class PropertyDescriptor {
      friend ObjectWrap<T>;

      const char* utf8name;
      StaticVoidMethodCallback staticVoidMethod;
      StaticMethodCallback staticMethod;
      StaticGetterCallback staticGetter;
      StaticSetterCallback staticSetter;
      Napi::Value staticValue;
      InstanceVoidMethodCallback instanceVoidMethod;
      InstanceMethodCallback instanceMethod;
      InstanceGetterCallback instanceGetter;
      InstanceSetterCallback instanceSetter;
      Napi::Value instanceValue;
      napi_property_attributes attributes;
      void* data;
    };

    static Function DefineClass(napi_env env,
                                const char* utf8name,
                                const std::initializer_list<PropertyDescriptor>& properties,
                                void* data = nullptr);
    static Function DefineClass(napi_env env,
                                const char* utf8name,
                                const std::vector<PropertyDescriptor>& properties,
                                void* data = nullptr);
    static PropertyDescriptor StaticMethod(const char* utf8name,
                                           StaticVoidMethodCallback method,
                                           napi_property_attributes attributes = napi_default,
                                           void* data = nullptr);
    static PropertyDescriptor StaticMethod(const char* utf8name,
                                           StaticMethodCallback method,
                                           napi_property_attributes attributes = napi_default,
                                           void* data = nullptr);
    static PropertyDescriptor StaticMethod(Symbol name,
                                           StaticVoidMethodCallback method,
                                           napi_property_attributes attributes = napi_default,
                                           void* data = nullptr);
    static PropertyDescriptor StaticMethod(Symbol name,
                                           StaticMethodCallback method,
                                           napi_property_attributes attributes = napi_default,
                                           void* data = nullptr);
    static PropertyDescriptor StaticAccessor(const char* utf8name,
                                             StaticGetterCallback getter,
                                             StaticSetterCallback setter,
                                             napi_property_attributes attributes = napi_default,
                                             void* data = nullptr);
    static PropertyDescriptor StaticAccessor(Symbol name,
                                             StaticGetterCallback getter,
                                             StaticSetterCallback setter,
                                             napi_property_attributes attributes = napi_default,
                                             void* data = nullptr);
    static PropertyDescriptor InstanceMethod(const char* utf8name,
                                             InstanceVoidMethodCallback method,
                                             napi_property_attributes attributes = napi_default,
                                             void* data = nullptr);
    static PropertyDescriptor InstanceMethod(const char* utf8name,
                                             InstanceMethodCallback method,
                                             napi_property_attributes attributes = napi_default,
                                             void* data = nullptr);
    static PropertyDescriptor InstanceMethod(Symbol name,
                                             InstanceVoidMethodCallback method,
                                             napi_property_attributes attributes = napi_default,
                                             void* data = nullptr);
    static PropertyDescriptor InstanceMethod(Symbol name,
                                             InstanceMethodCallback method,
                                             napi_property_attributes attributes = napi_default,
                                             void* data = nullptr);
    static PropertyDescriptor InstanceAccessor(const char* utf8name,
                                               InstanceGetterCallback getter,
                                               InstanceSetterCallback setter,
                                               napi_property_attributes attributes = napi_default,
                                               void* data = nullptr);
    static PropertyDescriptor InstanceAccessor(Symbol name,
                                               InstanceGetterCallback getter,
                                               InstanceSetterCallback setter,
                                               napi_property_attributes attributes = napi_default,
                                               void* data = nullptr);
    static PropertyDescriptor StaticValue(const char* utf8name,
                                          Napi::Value value,
                                          napi_property_attributes attributes = napi_default);
    static PropertyDescriptor StaticValue(Symbol name,
                                          Napi::Value value,
                                          napi_property_attributes attributes = napi_default);
    static PropertyDescriptor InstanceValue(const char* utf8name,
                                            Napi::Value value,
                                            napi_property_attributes attributes = napi_default);
    static PropertyDescriptor InstanceValue(Symbol name,
                                            Napi::Value value,
                                            napi_property_attributes attributes = napi_default);
    virtual void Finalize(napi_env env);

  private:
    static Function DefineClass(napi_env env,
                                const char* utf8name,
                                const size_t props_count,
                                const PropertyDescriptor* props,
                                void* data = nullptr);

    static T* Unwrap(napi_env env, const jsi::Object& object);

    template <typename TCallback>
    struct MethodCallbackData {
      TCallback callback;
      void* data;
    };
    typedef MethodCallbackData<StaticVoidMethodCallback> StaticVoidMethodCallbackData;
    typedef MethodCallbackData<StaticMethodCallback> StaticMethodCallbackData;
    typedef MethodCallbackData<InstanceVoidMethodCallback> InstanceVoidMethodCallbackData;
    typedef MethodCallbackData<InstanceMethodCallback> InstanceMethodCallbackData;

    template <typename TGetterCallback, typename TSetterCallback>
    struct AccessorCallbackData {
      TGetterCallback getterCallback;
      TSetterCallback setterCallback;
      void* data;
    };
    typedef AccessorCallbackData<StaticGetterCallback, StaticSetterCallback>
      StaticAccessorCallbackData;
    typedef AccessorCallbackData<InstanceGetterCallback, InstanceSetterCallback>
      InstanceAccessorCallbackData;

    napi_env _env;
  };

  // No-op stub class
  class HandleScope {
  public:
    explicit HandleScope(napi_env env);
    ~HandleScope();

    Napi::Env Env() const;

  private:
    napi_env _env;
  };

  // No-op stub class
  class EscapableHandleScope {
  public:
    explicit EscapableHandleScope(napi_env env);
    ~EscapableHandleScope();

    Napi::Env Env() const;
    Value Escape(jsi::Value escapee);

  private:
    napi_env _env;
  };
} // namespace Napi

// Inline implementations of all the above class methods are included here.
#include "napi-inl.h"
