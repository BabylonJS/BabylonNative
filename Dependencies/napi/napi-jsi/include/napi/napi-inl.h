#include <stdexcept>

namespace Napi {

namespace details {
  template <typename T>
  class External : public jsi::HostObject {
  public:
    External(napi_env env, T* data)
      : _env{env}, _data{data} {
    }

    T* Data() const {
      return _data;
    }

  protected:
    napi_env _env;
    T* _data;
  };

  template <typename T, typename Finalizer>
  class ExternalWithFinalizer : public External<T> {
  public:
    ExternalWithFinalizer(napi_env env, T* data, Finalizer finalizer)
      : External<T>{env, data}, _finalizer{std::move(finalizer)} {
    }

    ~ExternalWithFinalizer() {
      _finalizer(this->_env, this->_data);
    }

  private:
    Finalizer _finalizer;
  };

  template <typename T, typename Finalizer, typename Hint>
  class ExternalWithFinalizerAndHint : public External<T> {
  public:
    ExternalWithFinalizerAndHint(napi_env env, T* data, Finalizer finalizer, Hint* hint)
      : External<T>{env, data}, _finalizer{std::move(finalizer)}, _hint{hint} {
    }

    ~ExternalWithFinalizerAndHint() {
      _finalizer(this->_env, this->_data, _hint);
    }

  private:
    Finalizer _finalizer;
    Hint* _hint;
  };
} // details

////////////////////////////////////////////////////////////////////////////////
// Env class
////////////////////////////////////////////////////////////////////////////////

inline Env::Env(napi_env env) : _env{env} {
}

inline Env::operator napi_env() const {
  return _env;
}

inline Object Env::Global() const {
  return {_env, _env->rt.global()};
}

inline Value Env::Undefined() const {
  return {_env, jsi::Value::undefined()};
}

inline Value Env::Null() const {
  return {_env, jsi::Value::null()};
}

////////////////////////////////////////////////////////////////////////////////
// Value class
////////////////////////////////////////////////////////////////////////////////

inline Value::Value()
  : _env{nullptr}, _value{} {
}

inline Value::Value(napi_env env, jsi::Value value)
  : _env{env}, _value{std::move(value)} {
}

inline Value::operator jsi::Value&&() && {
  return std::move(_value);
}

inline Value::Value(const Value& other)
  : _env{other._env}, _value{_env == nullptr ? jsi::Value{} : jsi::Value{_env->rt, other._value}} {
}

inline Value& Value::operator =(const Value& other){
  _env = other._env;
  _value = {_env->rt, other._value};
  return *this;
}

inline Value::operator jsi::Value&() & {
  return _value;
}

inline Value::operator const jsi::Value&() const & {
  return _value;
}

inline bool Value::operator ==(const Value& other) const {
  return StrictEquals(other);
}

inline bool Value::operator !=(const Value& other) const {
  return !this->operator ==(other);
}

inline bool Value::StrictEquals(const Value& other) const {
  return jsi::Value::strictEquals(_env->rt, _value, other._value);
}

inline Napi::Env Value::Env() const {
  return {_env};
}

inline bool Value::IsEmpty() const {
  return _env == nullptr;
}

inline napi_valuetype Value::Type() const {
  if (_value.isUndefined()) {
    return napi_undefined;
  } else if (_value.isNull()) {
    return napi_null;
  } else if (_value.isBool()) {
    return napi_boolean;
  } else if (_value.isNumber()) {
    return napi_number;
  } else if (_value.isString()) {
    return napi_string;
  } else if (_value.isSymbol()) {
    return napi_symbol;
  } else if (_value.isObject()) {
    auto object{_value.getObject(_env->rt)};
    if (object.isFunction(_env->rt)) {
        return napi_function;
    } else if (object.isHostObject(_env->rt)) {
        return napi_external;
    }
    return napi_object;
  }

  abort();
}

inline bool Value::IsUndefined() const {
  return _value.isUndefined();
}

inline bool Value::IsNull() const {
  return _value.isNull();
}

inline bool Value::IsBoolean() const {
  return _value.isBool();
}

inline bool Value::IsNumber() const {
  return _value.isNumber();
}

inline bool Value::IsString() const {
  return _value.isString();
}

inline bool Value::IsSymbol() const {
  return _value.isSymbol();
}

inline bool Value::IsArray() const {
  if (!_value.isObject()) {
      return false;
  }

  auto object{_value.getObject(_env->rt)};
  return object.isArray(_env->rt);
}

inline bool Value::IsArrayBuffer() const {
  if (!_value.isObject()) {
      return false;
  }

  auto object{_value.getObject(_env->rt)};
  return object.isArrayBuffer(_env->rt);
}

inline bool Value::IsTypedArray() const {
  throw std::runtime_error{"TODO"};
}

inline bool Value::IsObject() const {
  return _value.isObject();
}

inline bool Value::IsFunction() const {
  return _value.isObject() && _value.getObject(_env->rt).isFunction(_env->rt);
}

inline bool Value::IsPromise() const {
  throw std::runtime_error{"TODO"};
}

inline bool Value::IsDataView() const {
  throw std::runtime_error{"TODO"};
}

inline bool Value::IsExternal() const {
  return _value.isObject() && _value.getObject(_env->rt).isHostObject(_env->rt);
}

template <typename T>
inline T Value::As() const {
  return T(_env, {_env->rt, _value});
}

inline Boolean Value::ToBoolean() const {
  // TODO: need to coerce if not the right type
  return Boolean::New(_env, _value.getBool());
}

inline Number Value::ToNumber() const {
  // TODO: need to coerce if not the right type
  return Number::New(_env, _value.getNumber());
}

inline String Value::ToString() const {
  return {_env, {_env->rt, _value.toString(_env->rt)}};
}

inline Object Value::ToObject() const {
  return {_env, _value.getObject(_env->rt)};
}

////////////////////////////////////////////////////////////////////////////////
// Boolean class
////////////////////////////////////////////////////////////////////////////////

inline Boolean::Boolean() {
}

inline Boolean Boolean::New(napi_env env, bool val) {
  return Boolean(env, {val});
}

inline Boolean::Boolean(napi_env env, jsi::Value value)
  : Napi::Value(env, std::move(value)) {
}

inline Boolean::operator bool() const {
  return Value();
}

inline bool Boolean::Value() const {
  return _value.getBool();
}

////////////////////////////////////////////////////////////////////////////////
// Number class
////////////////////////////////////////////////////////////////////////////////

inline Number::Number() {
}

inline Number Number::New(napi_env env, double val) {
  return Number(env, {env->rt, jsi::Value(val)});
}

inline Number::Number(napi_env env, jsi::Value value)
  : Value(env, std::move(value)) {
}

inline Number::operator int32_t() const {
  return Int32Value();
}

inline Number::operator uint32_t() const {
  return Uint32Value();
}

inline Number::operator int64_t() const {
  return Int64Value();
}

inline Number::operator float() const {
  return FloatValue();
}

inline Number::operator double() const {
  return DoubleValue();
}

inline int32_t Number::Int32Value() const {
  return static_cast<int32_t>(_value.getNumber());
}

inline uint32_t Number::Uint32Value() const {
  return static_cast<uint32_t>(_value.getNumber());
}

inline int64_t Number::Int64Value() const {
  return static_cast<int64_t>(_value.getNumber());
}

inline float Number::FloatValue() const {
  return static_cast<float>(_value.getNumber());
}

inline double Number::DoubleValue() const {
  return _value.getNumber();
}

////////////////////////////////////////////////////////////////////////////////
// Name class
////////////////////////////////////////////////////////////////////////////////

inline Name::Name(napi_env env, jsi::Value value)
  : Value(env, std::move(value)) {
}

////////////////////////////////////////////////////////////////////////////////
// String class
////////////////////////////////////////////////////////////////////////////////

inline String String::New(napi_env env, const std::string& val) {
  return String::New(env, val.c_str(), val.size());
}

inline String String::New(napi_env env, const std::u16string& val) {
  return String::New(env, val.c_str(), val.size());
}

inline String String::New(napi_env env, const char* val) {
  return String(env, {env->rt, jsi::String::createFromUtf8(env->rt, val)});
}

inline String String::New(napi_env env, const char16_t* val) {
  (void)env;
  (void)val;
  throw std::runtime_error("TODO");
}

inline String String::New(napi_env env, const char* val, size_t length) {
  return String(env, {env->rt, jsi::String::createFromUtf8(env->rt, reinterpret_cast<const uint8_t*>(val), length)});
}

inline String String::New(napi_env env, const char16_t* val, size_t length) {
  (void)env;
  (void)val;
  (void)length;
  throw std::runtime_error("TODO");
}

inline String::String(napi_env env, jsi::Value value)
  : Name(env, std::move(value)) {
}

inline String::operator std::string() const {
  return Utf8Value();
}

inline String::operator std::u16string() const {
  return Utf16Value();
}

inline std::string String::Utf8Value() const {
  return _value.getString(_env->rt).utf8(_env->rt);
}

inline std::u16string String::Utf16Value() const {
  throw std::runtime_error("TODO");
}

////////////////////////////////////////////////////////////////////////////////
// Symbol class
////////////////////////////////////////////////////////////////////////////////

inline Symbol Symbol::New(napi_env env, const char* description) {
  (void)env;
  (void)description;
  throw std::runtime_error("TODO");
}

inline Symbol Symbol::New(napi_env env, const std::string& description) {
  (void)env;
  (void)description;
  throw std::runtime_error("TODO");
}

inline Symbol Symbol::New(napi_env env, String description) {
  (void)env;
  (void)description;
  throw std::runtime_error("TODO");
}

inline Symbol Symbol::New(napi_env env, jsi::Value description) {
  (void)env;
  (void)description;
  throw std::runtime_error("TODO");
}

inline Symbol Symbol::WellKnown(napi_env env, const std::string& name) {
  // TODO: can be optimized
  return Napi::Env(env).Global().Get("Symbol").As<Object>().Get(name).As<Symbol>();
}

inline Symbol::Symbol(napi_env env, jsi::Value value)
  : Name(env, std::move(value)) {
}

////////////////////////////////////////////////////////////////////////////////
// Automagic value creation
////////////////////////////////////////////////////////////////////////////////

namespace details {
template <typename T>
struct vf_number {
  static Number From(napi_env env, T value) {
    return Number::New(env, static_cast<double>(value));
  }
};

template<>
struct vf_number<bool> {
  static Boolean From(napi_env env, bool value) {
    return Boolean::New(env, value);
  }
};

struct vf_utf8_charp {
  static String From(napi_env env, const char* value) {
    return String::New(env, value);
  }
};

struct vf_utf16_charp {
  static String From(napi_env env, const char16_t* value) {
    return String::New(env, value);
  }
};
struct vf_utf8_string {
  static String From(napi_env env, const std::string& value) {
    return String::New(env, value);
  }
};

struct vf_utf16_string {
  static String From(napi_env env, const std::u16string& value) {
    return String::New(env, value);
  }
};

// TODO: this can probably be folded into the Helper/disjunction
template <typename T>
struct vf_fallback {
  static Value From(napi_env, const T& value) {
    return value;
  }
};

template <>
struct vf_fallback<jsi::Value> {
  static Value From(napi_env env, const jsi::Value& value) {
    return {env, {env->rt, value}};
  }
};

template <typename...> struct disjunction : std::false_type {};
template <typename B> struct disjunction<B> : B {};
template <typename B, typename... Bs>
struct disjunction<B, Bs...>
    : std::conditional<bool(B::value), B, disjunction<Bs...>>::type {};

template <typename T>
struct can_make_string
    : disjunction<typename std::is_convertible<T, const char *>::type,
                  typename std::is_convertible<T, const char16_t *>::type,
                  typename std::is_convertible<T, std::string>::type,
                  typename std::is_convertible<T, std::u16string>::type> {};
}

template <typename T>
Value Value::From(napi_env env, const T& value) {
  using Helper = typename std::conditional<
    std::is_integral<T>::value || std::is_floating_point<T>::value,
    details::vf_number<T>,
    typename std::conditional<
      details::can_make_string<T>::value,
      String,
      details::vf_fallback<T>
    >::type
  >::type;
  return Helper::From(env, value);
}

template <typename T>
String String::From(napi_env env, const T& value) {
  struct Dummy {};
  using Helper = typename std::conditional<
    std::is_convertible<T, const char*>::value,
    details::vf_utf8_charp,
    typename std::conditional<
      std::is_convertible<T, const char16_t*>::value,
      details::vf_utf16_charp,
      typename std::conditional<
        std::is_convertible<T, std::string>::value,
        details::vf_utf8_string,
        typename std::conditional<
          std::is_convertible<T, std::u16string>::value,
          details::vf_utf16_string,
          Dummy
        >::type
      >::type
    >::type
  >::type;
  return Helper::From(env, value);
}

////////////////////////////////////////////////////////////////////////////////
// Object class
////////////////////////////////////////////////////////////////////////////////

template <typename Key>
inline Object::PropertyLValue<Key>::operator Value() const {
  return _object.Get(_key);
}

template <typename Key> template <typename ValueType>
inline Object::PropertyLValue<Key>& Object::PropertyLValue<Key>::operator =(ValueType value) {
  _object.Set(_key, value);
  return *this;
}

template <typename Key>
inline Object::PropertyLValue<Key>::PropertyLValue(Object& object, Key key)
  : _object{object}, _key{key} {
}

inline Object::Object() {
}

inline Object::Object(napi_env env, jsi::Value value)
  : Value{env, std::move(value)}, _object{_value.getObject(env->rt)} {
}

inline Object::operator jsi::Object&&() && {
  return std::move(*_object);
}

inline Object::Object(const Object& other)
  : Object{other._env, {other._env->rt, other._value}} {
}

inline Object& Object::operator =(const Object& other){
  Value::operator =(other);
  _object = _value.getObject(_env->rt);
  return *this;
}

inline Object::operator jsi::Object&() & {
  return *_object;
}

inline Object::operator const jsi::Object&() const & {
  return *_object;
}

inline Object Object::New(napi_env env) {
  return {env, jsi::Object{env->rt}};
}

inline Object::PropertyLValue<std::string> Object::operator [](const char* utf8name) {
  return PropertyLValue<std::string>(*this, utf8name);
}

inline Object::PropertyLValue<std::string> Object::operator [](const std::string& utf8name) {
  return PropertyLValue<std::string>(*this, utf8name);
}

inline Object::PropertyLValue<uint32_t> Object::operator [](uint32_t index) {
  return PropertyLValue<uint32_t>(*this, index);
}

inline Value Object::operator [](const char* utf8name) const {
  return Get(utf8name);
}

inline Value Object::operator [](const std::string& utf8name) const {
  return Get(utf8name);
}

inline Value Object::operator [](uint32_t index) const {
  return Get(index);
}

inline bool Object::Has(const char* utf8name) const {
  return _object->hasProperty(_env->rt, utf8name);
}

inline bool Object::Has(const std::string& utf8name) const {
  return Has(utf8name.c_str());
}

inline bool Object::HasOwnProperty(const char* utf8name) const {
  (void)utf8name;
  throw std::runtime_error("TODO");
}

inline bool Object::HasOwnProperty(const std::string& utf8name) const {
  return HasOwnProperty(utf8name.c_str());
}

inline Value Object::Get(const char* utf8name) const {
  return {_env, _object->getProperty(_env->rt, utf8name)};
}

inline Value Object::Get(const std::string& utf8name) const {
  return Get(utf8name.c_str());
}

template <typename ValueType>
inline void Object::Set(const char* utf8name, const ValueType& value) {
  _object->setProperty(_env->rt, utf8name, static_cast<jsi::Value&&>(Value::From(_env, value)));
}

template <typename ValueType>
inline void Object::Set(const std::string& utf8name, const ValueType& value) {
  Set(utf8name.c_str(), value);
}

inline bool Object::Delete(const char* utf8name) {
  (void)utf8name;
  throw std::runtime_error("TODO");
}

inline bool Object::Delete(const std::string& utf8name) {
  return Delete(utf8name.c_str());
}

inline bool Object::Has(uint32_t index) const {
  (void)index;
  throw std::runtime_error{"TODO"};
}

inline Value Object::Get(uint32_t index) const {
  // TODO: does this need to work for non-array objects?
  return {_env, _object->getArray(_env->rt).getValueAtIndex(_env->rt, static_cast<size_t>(index))};
}

template <typename ValueType>
inline void Object::Set(uint32_t index, const ValueType& value) {
  // TODO: does this need to work for non-array objects?
  _object->getArray(_env->rt).setValueAtIndex(_env->rt, static_cast<size_t>(index), static_cast<jsi::Value&&>(Value::From(_env, value)));
}

inline bool Object::Delete(uint32_t index) {
  (void)index;
  throw std::runtime_error{"TODO"};
}

inline Array Object::GetPropertyNames() const {
  throw std::runtime_error{"TODO"};
}

// TODO: not implemented
//inline void Object::DefineProperty(const PropertyDescriptor& property) {
//  throw std::runtime_error{"TODO"};
//}
//
//inline void Object::DefineProperties(const std::initializer_list<PropertyDescriptor>& properties) {
//  throw std::runtime_error{"TODO"};
//}
//
//inline void Object::DefineProperties(const std::vector<PropertyDescriptor>& properties) {
//  throw std::runtime_error{"TODO"};
//}

inline bool Object::InstanceOf(const Function& constructor) const {
  // REVIEW: why is jsi::Object::instanceOf not a const function?
  return const_cast<jsi::Object&>(*_object).instanceOf(_env->rt, constructor);
}

////////////////////////////////////////////////////////////////////////////////
// External class
////////////////////////////////////////////////////////////////////////////////

template <typename T>
inline External<T> External<T>::New(napi_env env, T* data) {
  return {env, jsi::Object::createFromHostObject(env->rt, std::make_shared<details::External<T>>(env, data))};
}

template <typename T>
template <typename Finalizer>
inline External<T> External<T>::New(napi_env env,
                                    T* data,
                                    Finalizer finalizeCallback) {
  return {env, jsi::Object::createFromHostObject(env->rt, std::make_shared<details::ExternalWithFinalizer<T, Finalizer>>(env, data, std::move(finalizeCallback)))};
}

template <typename T>
template <typename Finalizer, typename Hint>
inline External<T> External<T>::New(napi_env env,
                                    T* data,
                                    Finalizer finalizeCallback,
                                    Hint* finalizeHint) {
  return {env, jsi::Object::createFromHostObject(env->rt, std::make_shared<details::ExternalWithFinalizerAndHint<T, Finalizer, Hint>>(env, data, std::move(finalizeCallback), finalizeHint))};
}

template <typename T>
inline External<T>::External(napi_env env, jsi::Value value)
  : Value(env, std::move(value)) {
}

template <typename T>
inline T* External<T>::Data() const {
  return _value.getObject(_env->rt).template getHostObject<details::External<T>>(_env->rt)->Data();
}

////////////////////////////////////////////////////////////////////////////////
// Array class
////////////////////////////////////////////////////////////////////////////////

inline Array Array::New(napi_env env) {
  return New(env, 0);
}

inline Array Array::New(napi_env env, size_t length) {
  return {env, jsi::Array{env->rt, length}};
}

inline Array::Array() {
}

inline Array::Array(napi_env env, jsi::Value value)
  : Object{env, std::move(value)}, _array{_object->getArray(env->rt)} {
}

inline Array::operator jsi::Array&&() && {
  return std::move(*_array);
}

inline Array::Array(const Array& other)
  : Array{other._env, {other._env->rt, other._value}} {
}

inline Array& Array::operator =(const Array& other){
  Object::operator =(other);
  _array = _object->getArray(_env->rt);
  return *this;
}

inline Array::operator jsi::Array&() & {
  return *_array;
}

inline Array::operator const jsi::Array&() const & {
  return *_array;
}

inline uint32_t Array::Length() const {
  return static_cast<uint32_t>(_array->length(_env->rt));
}

////////////////////////////////////////////////////////////////////////////////
// ArrayBuffer class
////////////////////////////////////////////////////////////////////////////////

inline ArrayBuffer ArrayBuffer::New(napi_env env, size_t byteLength) {
  jsi::Value value{env->array_buffer_ctor.callAsConstructor(env->rt, static_cast<int>(byteLength))};
  jsi::ArrayBuffer arrayBuffer{value.getObject(env->rt).getArrayBuffer(env->rt)};
  return {env, std::move(value)};
}

inline ArrayBuffer ArrayBuffer::New(napi_env env,
                                    void* externalData,
                                    size_t byteLength) {
  // must copy since jsi does not support array buffers with external data
  jsi::Value value{ env->array_buffer_ctor.callAsConstructor(env->rt, static_cast<int>(byteLength)) };
  jsi::ArrayBuffer arrayBuffer{ value.getObject(env->rt).getArrayBuffer(env->rt) };
  std::memcpy(arrayBuffer.data(env->rt), externalData, byteLength);
  return {env, std::move(value)};
}

template <typename Finalizer>
inline ArrayBuffer ArrayBuffer::New(napi_env env,
                                    void* externalData,
                                    size_t byteLength,
                                    Finalizer finalizeCallback) {
  return New(env, externalData, byteLength, finalizeCallback, nullptr);
}

template <typename Finalizer, typename Hint>
inline ArrayBuffer ArrayBuffer::New(napi_env env,
                                    void* externalData,
                                    size_t byteLength,
                                    Finalizer finalizeCallback,
                                    Hint* finalizeHint) {
  (void)env;
  (void)externalData;
  (void)byteLength;
  (void)finalizeCallback;
  (void)finalizeHint;
  throw std::runtime_error{"TODO"};
}

inline ArrayBuffer::ArrayBuffer() {
}

inline ArrayBuffer::ArrayBuffer(napi_env env, jsi::Value value)
  : Object{env, std::move(value)}, _arrayBuffer{_object->getArrayBuffer(env->rt)} {
}

inline ArrayBuffer::operator jsi::ArrayBuffer&&() && {
  return std::move(*_arrayBuffer);
}

inline ArrayBuffer::ArrayBuffer(const ArrayBuffer& other)
  : ArrayBuffer{other._env, {other._env->rt, other._value}} {
}

inline ArrayBuffer& ArrayBuffer::operator =(const ArrayBuffer& other){
  Object::operator =(other);
  _arrayBuffer = _object->getArrayBuffer(_env->rt);
  return *this;
}

inline ArrayBuffer::operator jsi::ArrayBuffer&() & {
  return *_arrayBuffer;
}

inline ArrayBuffer::operator const jsi::ArrayBuffer&() const & {
  return *_arrayBuffer;
}

inline void* ArrayBuffer::Data() const {
  return const_cast<jsi::ArrayBuffer&>(*_arrayBuffer).data(_env->rt);
}

inline size_t ArrayBuffer::ByteLength() const {
  return _arrayBuffer->length(_env->rt);
}

////////////////////////////////////////////////////////////////////////////////
// DataView class
////////////////////////////////////////////////////////////////////////////////
inline DataView DataView::New(napi_env env,
                              Napi::ArrayBuffer arrayBuffer) {
  return New(env, arrayBuffer, 0, arrayBuffer.ByteLength());
}

inline DataView DataView::New(napi_env env,
                              Napi::ArrayBuffer arrayBuffer,
                              size_t byteOffset) {
  (void)env;
  (void)arrayBuffer;
  (void)byteOffset;
  //if (byteOffset > arrayBuffer.ByteLength()) {
  //  NAPI_THROW(RangeError::New(env,
  //      "Start offset is outside the bounds of the buffer"),
  //      DataView());
  //}
  //return New(env, arrayBuffer, byteOffset,
  //    arrayBuffer.ByteLength() - byteOffset);
  throw std::runtime_error{"TODO"};
}

inline DataView DataView::New(napi_env env,
                              Napi::ArrayBuffer arrayBuffer,
                              size_t byteOffset,
                              size_t byteLength) {
  (void)env;
  (void)arrayBuffer;
  (void)byteOffset;
  (void)byteLength;
  //if (byteOffset + byteLength > arrayBuffer.ByteLength()) {
  //  NAPI_THROW(RangeError::New(env, "Invalid DataView length"),
  //             DataView());
  //}
  //jsi::Value value;
  //napi_status status = napi_create_dataview(
  //  rt, byteLength, arrayBuffer, byteOffset, &value);
  //NAPI_THROW_IF_FAILED(env, status, DataView());
  //return DataView(env, value);
  throw std::runtime_error{"TODO"};
}

inline DataView::DataView(napi_env env, jsi::Value value)
  : Object{env, std::move(value)} {
  //napi_status status = napi_get_dataview_info(
  //  _env,
  //  _value   /* dataView */,
  //  &_length /* byteLength */,
  //  &_data   /* data */,
  //  nullptr  /* arrayBuffer */,
  //  nullptr  /* byteOffset */);
  //NAPI_THROW_IF_FAILED_VOID(_env, status);
  throw std::runtime_error{"TODO"};
}

inline Napi::ArrayBuffer DataView::ArrayBuffer() const {
  //jsi::Value arrayBuffer;
  //napi_status status = napi_get_dataview_info(
  //  _env,
  //  _value       /* dataView */,
  //  nullptr      /* byteLength */,
  //  nullptr      /* data */,
  //  &arrayBuffer /* arrayBuffer */,
  //  nullptr      /* byteOffset */);
  //NAPI_THROW_IF_FAILED(_env, status, Napi::ArrayBuffer());
  //return Napi::ArrayBuffer(_env, arrayBuffer);
  throw std::runtime_error{"TODO"};
}

inline size_t DataView::ByteOffset() const {
  //size_t byteOffset;
  //napi_status status = napi_get_dataview_info(
  //  _env,
  //  _value      /* dataView */,
  //  nullptr     /* byteLength */,
  //  nullptr     /* data */,
  //  nullptr     /* arrayBuffer */,
  //  &byteOffset /* byteOffset */);
  //NAPI_THROW_IF_FAILED(_env, status, 0);
  //return byteOffset;
  throw std::runtime_error{"TODO"};
}

inline size_t DataView::ByteLength() const {
  return _length;
}

inline void* DataView::Data() const {
  return _data;
}

inline float DataView::GetFloat32(size_t byteOffset) const {
  return ReadData<float>(byteOffset);
}

inline double DataView::GetFloat64(size_t byteOffset) const {
  return ReadData<double>(byteOffset);
}

inline int8_t DataView::GetInt8(size_t byteOffset) const {
  return ReadData<int8_t>(byteOffset);
}

inline int16_t DataView::GetInt16(size_t byteOffset) const {
  return ReadData<int16_t>(byteOffset);
}

inline int32_t DataView::GetInt32(size_t byteOffset) const {
  return ReadData<int32_t>(byteOffset);
}

inline uint8_t DataView::GetUint8(size_t byteOffset) const {
  return ReadData<uint8_t>(byteOffset);
}

inline uint16_t DataView::GetUint16(size_t byteOffset) const {
  return ReadData<uint16_t>(byteOffset);
}

inline uint32_t DataView::GetUint32(size_t byteOffset) const {
  return ReadData<uint32_t>(byteOffset);
}

inline void DataView::SetFloat32(size_t byteOffset, float value) const {
  WriteData<float>(byteOffset, value);
}

inline void DataView::SetFloat64(size_t byteOffset, double value) const {
  WriteData<double>(byteOffset, value);
}

inline void DataView::SetInt8(size_t byteOffset, int8_t value) const {
  WriteData<int8_t>(byteOffset, value);
}

inline void DataView::SetInt16(size_t byteOffset, int16_t value) const {
  WriteData<int16_t>(byteOffset, value);
}

inline void DataView::SetInt32(size_t byteOffset, int32_t value) const {
  WriteData<int32_t>(byteOffset, value);
}

inline void DataView::SetUint8(size_t byteOffset, uint8_t value) const {
  WriteData<uint8_t>(byteOffset, value);
}

inline void DataView::SetUint16(size_t byteOffset, uint16_t value) const {
  WriteData<uint16_t>(byteOffset, value);
}

inline void DataView::SetUint32(size_t byteOffset, uint32_t value) const {
  WriteData<uint32_t>(byteOffset, value);
}

template <typename T>
inline T DataView::ReadData(size_t byteOffset) const {
  (void)byteOffset;
  //if (byteOffset + sizeof(T) > _length ||
  //    byteOffset + sizeof(T) < byteOffset) {  // overflow
  //  NAPI_THROW(RangeError::New(_env,
  //      "Offset is outside the bounds of the DataView"), 0);
  //}

  //return *reinterpret_cast<T*>(static_cast<uint8_t*>(_data) + byteOffset);
  throw std::runtime_error{"TODO"};
}

template <typename T>
inline void DataView::WriteData(size_t byteOffset, T value) const {
  (void)byteOffset;
  (void)value;
  //if (byteOffset + sizeof(T) > _length ||
  //    byteOffset + sizeof(T) < byteOffset) {  // overflow
  //  NAPI_THROW_VOID(RangeError::New(_env,
  //      "Offset is outside the bounds of the DataView"));
  //}

  //*reinterpret_cast<T*>(static_cast<uint8_t*>(_data) + byteOffset) = value;
  throw std::runtime_error{"TODO"};
}

////////////////////////////////////////////////////////////////////////////////
// TypedArray class
////////////////////////////////////////////////////////////////////////////////

// JSI does not support typed array directly and thus this implementation is
// suboptimal. See https://github.com/facebook/hermes/issues/182.

inline TypedArray::TypedArray() {
}

inline TypedArray::TypedArray(napi_env env, jsi::Value value)
  : Object{env, std::move(value)}, _type{TypedArray::unknown_array_type}, _length{0} {
}

inline TypedArray::TypedArray(napi_env env, jsi::Value value, napi_typedarray_type type, size_t length)
  : Object(env, std::move(value)), _type(type), _length(length) {
}

inline napi_typedarray_type TypedArray::TypedArrayType() const {
  if (_type == TypedArray::unknown_array_type) {
    GetTypedArrayInfo(_env, *_object, &_type, &_length);
  }

  return _type;
}

inline uint8_t TypedArray::ElementSize() const {
  switch (TypedArrayType()) {
    case napi_int8_array:
    case napi_uint8_array:
    case napi_uint8_clamped_array:
      return 1;
    case napi_int16_array:
    case napi_uint16_array:
      return 2;
    case napi_int32_array:
    case napi_uint32_array:
    case napi_float32_array:
      return 4;
    case napi_float64_array:
      return 8;
    default:
      return 0;
  }
}

inline size_t TypedArray::ElementLength() const {
  if (_type == TypedArray::unknown_array_type) {
    GetTypedArrayInfo(_env, *_object, &_type, &_length);
  }

  return _length;
}

inline size_t TypedArray::ByteOffset() const {
  return static_cast<size_t>(_object->getProperty(_env->rt, "byteOffset").asNumber());
}

inline size_t TypedArray::ByteLength() const {
  return ElementSize() * ElementLength();
}

inline Napi::ArrayBuffer TypedArray::ArrayBuffer() const {
  return {_env, _object->getPropertyAsObject(_env->rt, "buffer").getArrayBuffer(_env->rt)};
}

inline void TypedArray::GetTypedArrayInfo(napi_env env,
                                          const jsi::Object& object,
                                          napi_typedarray_type* type,
                                          size_t* length) {
  if (type != nullptr) {
    std::string name{object.getPropertyAsObject(env->rt, "constructor").getProperty(env->rt, "name").getString(env->rt).utf8(env->rt)};
    if (name == "Int8Array") {
      *type = napi_int8_array;
    } else if (name == "Uint8Array") {
      *type = napi_uint8_array;
    } else if (name == "Uint8ClampedArray") {
      *type = napi_uint8_clamped_array;
    } else if (name == "Int16Array") {
      *type = napi_int16_array;
    } else if (name == "Uint16Array") {
      *type = napi_uint16_array;
    } else if (name == "Int32Array") {
      *type = napi_int32_array;
    } else if (name == "Uint32Array") {
      *type = napi_uint32_array;
    } else if (name == "Float32Array") {
      *type = napi_float32_array;
    } else if (name == "Float64Array") {
      *type = napi_float64_array;
    } else {
      *type = TypedArray::unknown_array_type;
    }
  }

  if (length != nullptr) {
    *length = static_cast<size_t>(object.getProperty(env->rt, "length").asNumber());
  }
}


////////////////////////////////////////////////////////////////////////////////
// TypedArrayOf<T> class
////////////////////////////////////////////////////////////////////////////////

template <typename T>
inline TypedArrayOf<T> TypedArrayOf<T>::New(napi_env env,
                                            size_t elementLength,
                                            napi_typedarray_type type) {
  jsi::Function& ctor{ env->typed_array_ctor[type] };
  jsi::Value value{ctor.callAsConstructor(env->rt, static_cast<int>(elementLength))};
  return {env, std::move(value)};
}

template <typename T>
inline TypedArrayOf<T> TypedArrayOf<T>::New(napi_env env,
                                            size_t elementLength,
                                            Napi::ArrayBuffer arrayBuffer,
                                            size_t bufferOffset,
                                            napi_typedarray_type type) {
  jsi::Function& ctor{env->typed_array_ctor[type]};
  jsi::Value value{ctor.callAsConstructor(env->rt, arrayBuffer, static_cast<int>(bufferOffset), static_cast<int>(elementLength))};
  return {env, std::move(value), type, elementLength, reinterpret_cast<T*>(reinterpret_cast<uint8_t*>(arrayBuffer.Data()) + bufferOffset)};
}

template <typename T>
inline TypedArrayOf<T>::TypedArrayOf() {
}

template <typename T>
inline TypedArrayOf<T>::TypedArrayOf(napi_env env, jsi::Value value)
  : TypedArray{env, std::move(value)}, _data{reinterpret_cast<T*>(jsi::ArrayBuffer{_object->getPropertyAsObject(env->rt, "buffer").getArrayBuffer(env->rt)}.data(env->rt) + ByteOffset())} {
}

template <typename T>
inline TypedArrayOf<T>::TypedArrayOf(napi_env env,
                                     jsi::Value value,
                                     napi_typedarray_type type,
                                     size_t length,
                                     T* data)
  : TypedArray{env, std::move(value), type, length}, _data{data} {
  // TODO
  //if (!(type == TypedArrayTypeForPrimitiveType<T>() ||
  //    (type == napi_uint8_clamped_array && std::is_same<T, uint8_t>::value))) {
  //  NAPI_THROW_VOID(TypeError::New(env, "Array type must match the template parameter. "
  //    "(Uint8 arrays may optionally have the \"clamped\" array type.)"));
  //}
}

template <typename T>
inline T& TypedArrayOf<T>::operator [](size_t index) {
  return _data[index];
}

template <typename T>
inline const T& TypedArrayOf<T>::operator [](size_t index) const {
  return _data[index];
}

template <typename T>
inline T* TypedArrayOf<T>::Data() {
  return _data;
}

template <typename T>
inline const T* TypedArrayOf<T>::Data() const {
  return _data;
}

////////////////////////////////////////////////////////////////////////////////
// Function class
////////////////////////////////////////////////////////////////////////////////

namespace details
{
  template <typename Callable, typename Return>
  struct Function {
    static inline jsi::Value Callback(napi_env env, const jsi::Value& thisVal, const jsi::Value* args, size_t count, void* data, Callable cb) {
      CallbackInfo callbackInfo{env, thisVal, args, count, {}, data};
      return {env->rt, cb(callbackInfo)};
    }
  };

  template <typename Callable>
  struct Function<Callable, void> {
    static inline jsi::Value Callback(napi_env env, const jsi::Value& thisVal, const jsi::Value* args, size_t count, void* data, Callable cb) {
      CallbackInfo callbackInfo{env, thisVal, args, count, {}, data};
      cb(callbackInfo);
      return {};
    }
  };
}

template <typename Callable>
inline Function Function::New(napi_env env,
                              Callable cb,
                              const char* utf8name,
                              void* data) {
  auto function{jsi::Function::createFromHostFunction(env->rt, jsi::PropNameID::forUtf8(env->rt, utf8name), 0,
    [env, cb{std::move(cb)}, data](jsi::Runtime&, const jsi::Value& thisVal, const jsi::Value* args, size_t count) -> jsi::Value {
      typedef decltype(cb(CallbackInfo({}, {}, {}, {}, {}, {}))) ReturnType;
      return details::Function<Callable, ReturnType>::Callback(env, thisVal, args, count, data, cb);
    })};

  return {env, std::move(function)};
}

template <typename Callable>
inline Function Function::New(napi_env env,
                              Callable cb,
                              const std::string& utf8name,
                              void* data) {
  return New(env, cb, utf8name.c_str(), data);
}

inline Function::Function() {
}

inline Function::Function(napi_env env, jsi::Value value)
  : Object{env, std::move(value)}, _function{_object->getFunction(env->rt)} {
}

inline Function::operator jsi::Function&&() && {
  return std::move(*_function);
}

inline Function::Function(const Function& other)
  : Function{other._env, {other._env->rt, other._value}} {
}

inline Function& Function::operator =(const Function& other){
  Object::operator =(other);
  _function = _object->getFunction(_env->rt);
  return *this;
}

inline Function::operator jsi::Function&() & {
  return *_function;
}

inline Function::operator const jsi::Function&() const & {
  return *_function;
}

inline Value Function::operator ()(const std::initializer_list<Value>& args) const {
  return Call(Env().Undefined(), args);
}

inline Value Function::Call(const std::initializer_list<Value>& args) const {
  return Call(Env().Undefined(), args);
}

inline Value Function::Call(const std::vector<Value>& args) const {
  return Call(Env().Undefined(), args);
}

inline Value Function::Call(size_t argc, const Value* args) const {
  return Call(Env().Undefined(), argc, args);
}

inline Value Function::Call(const Value& recv, const std::initializer_list<Value>& args) const {
  return Call(recv, args.size(), args.begin());
}

inline Value Function::Call(const Value& recv, const std::vector<Value>& args) const {
  return Call(recv, args.size(), args.data());
}

inline Value Function::Call(const Value& recv, size_t argc, const Value* args) const {
  jsi::Value stackArgs[6];
  std::vector<jsi::Value> heapArgs;
  jsi::Value* argv;
  if (argc <= std::size(stackArgs)) {
    argv = stackArgs;
  } else {
    heapArgs.resize(argc);
    argv = heapArgs.data();
  }

  for (size_t i = 0; i < argc; ++i) {
    argv[i] = {_env->rt, args[i]};
  }

  jsi::Value result{
    recv.IsUndefined()
      ? _function->call(_env->rt, static_cast<const jsi::Value*>(argv), argc)
      : _function->callWithThis(_env->rt, static_cast<const jsi::Object&>(recv.ToObject()), static_cast<const jsi::Value*>(argv), argc)};

  return {_env, std::move(result)};
}

inline Object Function::New(const std::initializer_list<Value>& args) const {
  return New(args.size(), args.begin());
}

inline Object Function::New(const std::vector<Value>& args) const {
  return New(args.size(), args.data());
}

inline Object Function::New(size_t argc, const Value* args) const {
  jsi::Value stackArgs[6];
  std::vector<jsi::Value> heapArgs;
  jsi::Value* argv;
  if (argc <= std::size(stackArgs)) {
    argv = stackArgs;
  } else {
    heapArgs.resize(argc);
    argv = heapArgs.data();
  }

  for (size_t i = 0; i < argc; ++i) {
    argv[i] = {_env->rt, args[i]};
  }

  jsi::Value instance{_function->callAsConstructor(
    _env->rt, static_cast<const jsi::Value*>(argv), argc)};

  return {_env, std::move(instance)};
}

////////////////////////////////////////////////////////////////////////////////
// Promise class
////////////////////////////////////////////////////////////////////////////////

inline Promise::Deferred Promise::Deferred::New(napi_env env) {
  return {env};
}

inline Promise::Deferred::Deferred(napi_env env)
  : _env{env} {
  jsi::Function executor{jsi::Function::createFromHostFunction(env->rt, jsi::PropNameID::forAscii(env->rt, "executor"), 0,
    [this](jsi::Runtime& rt, const jsi::Value&, const jsi::Value* args, size_t) -> jsi::Value {
      _resolve = args[0].asObject(rt).asFunction(rt);
      _reject = args[1].asObject(rt).asFunction(rt);
      return {};
    })};

  _promise = env->promise_ctor.callAsConstructor(env->rt, executor);
}

inline Promise::Deferred::Deferred(const Deferred& other)
  : _env{other._env}
  , _promise{_env->rt, other._promise}
  , _resolve{jsi::Value{_env->rt, *other._resolve}.asObject(_env->rt).asFunction(_env->rt)}
  , _reject{jsi::Value{_env->rt, *other._reject}.asObject(_env->rt).asFunction(_env->rt)} {
}

inline Promise::Deferred& Promise::Deferred::operator =(const Deferred& other) {
  _env = other._env;
  _promise = {_env->rt, other._promise};
  _resolve = jsi::Value{_env->rt, *other._resolve}.asObject(_env->rt).asFunction(_env->rt);
  _reject = jsi::Value{_env->rt, *other._reject}.asObject(_env->rt).asFunction(_env->rt);
  return *this;
}

inline Promise Promise::Deferred::Promise() const {
  return {_env, {_env->rt, _promise}};
}

inline Napi::Env Promise::Deferred::Env() const {
  return {_env};
}

inline void Promise::Deferred::Resolve(const jsi::Value& value) const {
  _resolve->call(_env->rt, value);
}

inline void Promise::Deferred::Reject(const jsi::Value& value) const {
  _reject->call(_env->rt, value);
}

inline Promise::Promise(napi_env env, jsi::Value value)
  : Object(env, std::move(value)) {
}

////////////////////////////////////////////////////////////////////////////////
// Error class
////////////////////////////////////////////////////////////////////////////////

inline Error Error::New(napi_env env) {
  (void)env;
  //napi_status status;
  //jsi::Value error = nullptr;

  //const napi_extended_error_info* info;
  //status = napi_get_last_error_info(env, &info);
  //NAPI_FATAL_IF_FAILED(status, "Error::New", "napi_get_last_error_info");

  //if (info->error_code == napi_pending_exception) {
  //  status = napi_get_and_clear_last_exception(env, &error);
  //  NAPI_FATAL_IF_FAILED(status, "Error::New", "napi_get_and_clear_last_exception");
  //}
  //else {
  //  const char* error_message = info->error_message != nullptr ?
  //    info->error_message : "Error in native callback";

  //  bool isExceptionPending;
  //  status = napi_is_exception_pending(env, &isExceptionPending);
  //  NAPI_FATAL_IF_FAILED(status, "Error::New", "napi_is_exception_pending");

  //  if (isExceptionPending) {
  //    status = napi_get_and_clear_last_exception(env, &error);
  //    NAPI_FATAL_IF_FAILED(status, "Error::New", "napi_get_and_clear_last_exception");
  //  }

  //  jsi::Value message;
  //  status = napi_create_string_utf8(
  //    env,
  //    error_message,
  //    std::strlen(error_message),
  //    &message);
  //  NAPI_FATAL_IF_FAILED(status, "Error::New", "napi_create_string_utf8");

  //  switch (info->error_code) {
  //  case napi_object_expected:
  //  case napi_string_expected:
  //  case napi_boolean_expected:
  //  case napi_number_expected:
  //    status = napi_create_type_error(env, nullptr, message, &error);
  //    break;
  //  default:
  //    status = napi_create_error(env, nullptr,  message, &error);
  //    break;
  //  }
  //  NAPI_FATAL_IF_FAILED(status, "Error::New", "napi_create_error");
  //}

  //return Error(env, error);
  throw std::runtime_error{"TODO"};
}

inline Error Error::New(napi_env env, const char* message) {
  (void)env;
  (void)message;
  //return Error::New<Error>(env, message, std::strlen(message), napi_create_error);
  throw std::runtime_error{"TODO"};
}

inline Error Error::New(napi_env env, const std::string& message) {
  (void)env;
  (void)message;
  //return Error::New<Error>(env, message.c_str(), message.size(), napi_create_error);
  throw std::runtime_error{"TODO"};
}

inline /*NAPI_NO_RETURN*/ void Error::Fatal(const char* location, const char* message) {
  (void)location;
  (void)message;
  // $HACK
  //napi_fatal_error(location, NAPI_AUTO_LENGTH, message, NAPI_AUTO_LENGTH);
  throw std::exception();
}

inline Error::Error(napi_env env, jsi::Object object)
  : ObjectReference{env, std::move(object)} {
  //if (value != nullptr) {
  //  napi_status status = napi_create_reference(env, value, 1, &_object);

  //  // Avoid infinite recursion in the failure case.
  //  // Don't try to construct & throw another Error instance.
  //  NAPI_FATAL_IF_FAILED(status, "Error::Error", "napi_create_reference");
  //}
  //throw std::runtime_error{"TODO"};
}

inline Error::Error(Error&& other) : ObjectReference(other) {
}

inline Error& Error::operator =(Error&& other) {
  static_cast<Reference<Object>*>(this)->operator=(std::move(other));
  return *this;
}

inline Error::Error(const Error& other) : ObjectReference(other) {
}

inline Error& Error::operator =(Error& other) {
  (void)other;
  //Reset();

  //_env = other.Env();
  //HandleScope scope{_env};

  //jsi::Value value = other.Value();
  //if (value != nullptr) {
  //  napi_status status = napi_create_reference(_env, value, 1, &_object);
  //  NAPI_THROW_IF_FAILED(_env, status, *this);
  //}

  //return *this;
  throw std::runtime_error{"TODO"};
}

inline const std::string& Error::Message() const NAPI_NOEXCEPT {
  if (_message.empty()) {
#ifdef NAPI_CPP_EXCEPTIONS
    try {
      _message = Get("message").As<String>();
    }
    catch (...) {
      // Catch all errors here, to include e.g. a std::bad_alloc from
      // the std::string::operator=, because this method may not throw.
    }
#else // NAPI_CPP_EXCEPTIONS
    _message = Get("message").As<String>();
#endif // NAPI_CPP_EXCEPTIONS
  }
  return _message;
}

inline void Error::ThrowAsJavaScriptException() const {
//  HandleScope scope{_env};
//  if (!IsEmpty()) {
//
//    // We intentionally don't use `NAPI_THROW_*` macros here to ensure
//    // that there is no possible recursion as `ThrowAsJavaScriptException`
//    // is part of `NAPI_THROW_*` macro definition for noexcept.
//
//    napi_status status = napi_throw(_env, Value());
//
//#ifdef NAPI_CPP_EXCEPTIONS
//    if (status != napi_ok) {
//      throw Error::New(_env);
//    }
//#else // NAPI_CPP_EXCEPTIONS
//    NAPI_FATAL_IF_FAILED(status, "Error::ThrowAsJavaScriptException", "napi_throw");
//#endif // NAPI_CPP_EXCEPTIONS
//  }
  throw std::runtime_error{"TODO"};
}

#ifdef NAPI_CPP_EXCEPTIONS

inline const char* Error::what() const NAPI_NOEXCEPT {
  return Message().c_str();
}

#endif // NAPI_CPP_EXCEPTIONS

// TODO
//template <typename TError>
//inline TError Error::New(napi_env env,
//                         const char* message,
//                         size_t length,
//                         create_error_fn create_error) {
//  jsi::Value str;
//  napi_status status = napi_create_string_utf8(env, message, length, &str);
//  NAPI_THROW_IF_FAILED(env, status, TError());
//
//  jsi::Value error;
//  status = create_error(env, nullptr, str, &error);
//  NAPI_THROW_IF_FAILED(env, status, TError());
//
//  return TError(env, error);
//}

inline TypeError TypeError::New(napi_env env, const char* message) {
  (void)env;
  (void)message;
  //return Error::New<TypeError>(env, message, std::strlen(message), napi_create_type_error);
  throw std::runtime_error{"TODO"};
}

inline TypeError TypeError::New(napi_env env, const std::string& message) {
  (void)env;
  (void)message;
  //return Error::New<TypeError>(env, message.c_str(), message.size(), napi_create_type_error);
  throw std::runtime_error{"TODO"};
}

inline TypeError::TypeError(napi_env env, jsi::Object object)
  : Error{env, std::move(object)} {
}

inline RangeError RangeError::New(napi_env env, const char* message) {
  (void)env;
  (void)message;
  //return Error::New<RangeError>(env, message, std::strlen(message), napi_create_range_error);
  throw std::runtime_error{"TODO"};
}

inline RangeError RangeError::New(napi_env env, const std::string& message) {
  (void)env;
  (void)message;
  //return Error::New<RangeError>(env, message.c_str(), message.size(), napi_create_range_error);
  throw std::runtime_error{"TODO"};
}

inline RangeError::RangeError(napi_env env, jsi::Object object)
  : Error{env, std::move(object)} {
}

////////////////////////////////////////////////////////////////////////////////
// Reference<T> class
////////////////////////////////////////////////////////////////////////////////

template <typename T>
inline Reference<T> Reference<T>::New(const T& object, uint32_t initialRefcount) {
  (void)object;
  (void)initialRefcount;
  //napi_env env = value.Env();
  //jsi::Value val = value;

  //if (val == nullptr) {
  //  return Reference<T>(env, nullptr);
  //}

  //napi_ref ref;
  //napi_status status = napi_create_reference(env, value, initialRefcount, &ref);
  //NAPI_THROW_IF_FAILED(env, status, Reference<T>());

  //return Reference<T>(env, ref);
  //throw std::runtime_error{"TODO"};
  return {object.Env(), object};
}

template <typename T>
inline Reference<T>::Reference()
  : _env{nullptr}, _suppressDestruct{false} {
}

template <typename T>
inline Reference<T>::Reference(napi_env env, T object)
  : _env{env}, _object{std::move(object)}, _suppressDestruct{false} {
}

template <typename T>
inline Reference<T>::~Reference() {
  //if (_object != nullptr) {
  //  if (!_suppressDestruct) {
  //    napi_delete_reference(_env, _object);
  //  }

  //  _object = nullptr;
  //}
  //throw std::runtime_error{"TODO"};
}

template <typename T>
inline Reference<T>::Reference(Reference<T>&& other)
  : _env(other._env), _object(std::move(other._object)), _suppressDestruct(other._suppressDestruct) {
  other._env = nullptr;
  other._suppressDestruct = false;
}

template <typename T>
inline Reference<T>::Reference(const Reference<T>& other)
  : _env{other._env}, _object{other._object}, _suppressDestruct{false} {
}

template <typename T>
inline Reference<T>& Reference<T>::operator =(Reference<T>&& other) {
  Reset();
  _env = other._env;
  _object = std::move(other._object);
  _suppressDestruct = other._suppressDestruct;
  return *this;
}

template <typename T>
inline bool Reference<T>::operator ==(const Reference<T> &other) const {
  HandleScope scope{_env};
  return this->Value().StrictEquals(other.Value());
}

template <typename T>
inline bool Reference<T>::operator !=(const Reference<T> &other) const {
  return !this->operator ==(other);
}

template <typename T>
inline Napi::Env Reference<T>::Env() const {
  return {_env};
}

template <typename T>
inline bool Reference<T>::IsEmpty() const {
  return _env == nullptr;
}

template <typename T>
inline const T& Reference<T>::Value() const {
  //if (_object == nullptr) {
  //  return T(_env, nullptr);
  //}

  //jsi::Value value;
  //napi_status status = napi_get_reference_value(_env, _object, &value);
  //NAPI_THROW_IF_FAILED(_env, status, T());
  //return T(_env, value);
  //throw std::runtime_error{"TODO"};
  return _object;
}

template <typename T>
inline T& Reference<T>::Value() {
  //if (_object == nullptr) {
  //  return T(_env, nullptr);
  //}

  //jsi::Value value;
  //napi_status status = napi_get_reference_value(_env, _object, &value);
  //NAPI_THROW_IF_FAILED(_env, status, T());
  //return T(_env, value);
  //throw std::runtime_error{"TODO"};
  return _object;
}

template <typename T>
inline uint32_t Reference<T>::Ref() {
  //uint32_t result;
  //napi_status status = napi_reference_ref(_env, _object, &result);
  //NAPI_THROW_IF_FAILED(_env, status, 1);
  //return result;
  //throw std::runtime_error{"TODO"};
}

template <typename T>
inline uint32_t Reference<T>::Unref() {
  //uint32_t result;
  //napi_status status = napi_reference_unref(_env, _object, &result);
  //NAPI_THROW_IF_FAILED(_env, status, 1);
  //return result;
  //throw std::runtime_error{"TODO"};
}

template <typename T>
inline void Reference<T>::Reset() {
  //if (_object != nullptr) {
  //  napi_status status = napi_delete_reference(_env, _object);
  //  NAPI_THROW_IF_FAILED_VOID(_env, status);
  //  _object = nullptr;
  //}
  //throw std::runtime_error{"TODO"};
}

template <typename T>
inline void Reference<T>::Reset(const T& value, uint32_t refcount) {
  (void)value;
  (void)refcount;
  //Reset();
  //_env = value.Env();

  //jsi::Value val = value;
  //if (val != nullptr) {
  //  napi_status status = napi_create_reference(_env, value, refcount, &_object);
  //  NAPI_THROW_IF_FAILED_VOID(_env, status);
  //}
  throw std::runtime_error{"TODO"};
}

template <typename T>
inline void Reference<T>::SuppressDestruct() {
  _suppressDestruct = true;
}

template <typename T>
inline Reference<T> Weak(T value) {
  return Reference<T>::New(value, 0);
}

inline ObjectReference Weak(Object value) {
  return Reference<Object>::New(value, 0);
}

inline FunctionReference Weak(Function value) {
  return Reference<Function>::New(value, 0);
}

template <typename T>
inline Reference<T> Persistent(T value) {
  return Reference<T>::New(value, 1);
}

inline ObjectReference Persistent(Object value) {
  return Reference<Object>::New(value, 1);
}

inline FunctionReference Persistent(Function value) {
  return Reference<Function>::New(value, 1);
}

////////////////////////////////////////////////////////////////////////////////
// ObjectReference class
////////////////////////////////////////////////////////////////////////////////

inline ObjectReference::ObjectReference() {
}

inline ObjectReference::ObjectReference(napi_env env, jsi::Object object)
  : Reference<Object>{env, {env, std::move(object)}} {
}

inline ObjectReference::ObjectReference(Reference<Object>&& other)
  : Reference<Object>(other) {
}

inline ObjectReference& ObjectReference::operator =(Reference<Object>&& other) {
  static_cast<Reference<Object>*>(this)->operator=(std::move(other));
  return *this;
}

inline ObjectReference::ObjectReference(ObjectReference&& other)
  : Reference<Object>(other) {
}

inline ObjectReference& ObjectReference::operator =(ObjectReference&& other) {
  static_cast<Reference<Object>*>(this)->operator=(std::move(other));
  return *this;
}

inline ObjectReference::ObjectReference(const ObjectReference& other)
  : Reference<Object>(other) {
}

inline Napi::Value ObjectReference::Get(const char* utf8name) const {
  (void)utf8name;
  //EscapableHandleScope scope{_env};
  //return scope.Escape(Value().Get(utf8name));
  throw std::runtime_error{"TODO"};
}

inline Napi::Value ObjectReference::Get(const std::string& utf8name) const {
  (void)utf8name;
  //EscapableHandleScope scope{_env};
  //return scope.Escape(Value().Get(utf8name));
  throw std::runtime_error{"TODO"};
}

inline void ObjectReference::Set(const char* utf8name, Napi::Value value) {
  HandleScope scope{_env};
  Value().Set(utf8name, value);
}

inline void ObjectReference::Set(const char* utf8name, const char* utf8value) {
  HandleScope scope{_env};
  Value().Set(utf8name, utf8value);
}

inline void ObjectReference::Set(const char* utf8name, bool boolValue) {
  HandleScope scope{_env};
  Value().Set(utf8name, boolValue);
}

inline void ObjectReference::Set(const char* utf8name, double numberValue) {
  HandleScope scope{_env};
  Value().Set(utf8name, numberValue);
}

inline void ObjectReference::Set(const std::string& utf8name, Napi::Value value) {
  HandleScope scope{_env};
  Value().Set(utf8name, value);
}

inline void ObjectReference::Set(const std::string& utf8name, std::string& utf8value) {
  HandleScope scope{_env};
  Value().Set(utf8name, utf8value);
}

inline void ObjectReference::Set(const std::string& utf8name, bool boolValue) {
  HandleScope scope{_env};
  Value().Set(utf8name, boolValue);
}

inline void ObjectReference::Set(const std::string& utf8name, double numberValue) {
  HandleScope scope{_env};
  Value().Set(utf8name, numberValue);
}

inline Napi::Value ObjectReference::Get(uint32_t index) const {
  (void)index;
  //EscapableHandleScope scope{_env};
  //return scope.Escape(Value().Get(index));
  throw std::runtime_error{"TODO"};
}

inline void ObjectReference::Set(uint32_t index, Napi::Value value) {
  HandleScope scope{_env};
  Value().Set(index, value);
}

inline void ObjectReference::Set(uint32_t index, const char* utf8value) {
  HandleScope scope{_env};
  Value().Set(index, utf8value);
}

inline void ObjectReference::Set(uint32_t index, const std::string& utf8value) {
  HandleScope scope{_env};
  Value().Set(index, utf8value);
}

inline void ObjectReference::Set(uint32_t index, bool boolValue) {
  HandleScope scope{_env};
  Value().Set(index, boolValue);
}

inline void ObjectReference::Set(uint32_t index, double numberValue) {
  HandleScope scope{_env};
  Value().Set(index, numberValue);
}

////////////////////////////////////////////////////////////////////////////////
// FunctionReference class
////////////////////////////////////////////////////////////////////////////////

inline FunctionReference::FunctionReference() {
}

inline FunctionReference::FunctionReference(napi_env env, jsi::Function function)
  : Reference<Function>{env, {env, std::move(function)}} {
}

inline FunctionReference::FunctionReference(Reference<Function>&& other)
  : Reference<Function>(other) {
}

inline FunctionReference& FunctionReference::operator =(Reference<Function>&& other) {
  static_cast<Reference<Function>*>(this)->operator=(std::move(other));
  return *this;
}

inline FunctionReference::FunctionReference(FunctionReference&& other)
  : Reference<Function>(other) {
}

inline FunctionReference& FunctionReference::operator =(FunctionReference&& other) {
  static_cast<Reference<Function>*>(this)->operator=(std::move(other));
  return *this;
}

inline Napi::Value FunctionReference::operator ()(
    const std::initializer_list<Napi::Value>& args) const {
  return Value().Call(args);
}

inline Napi::Value FunctionReference::Call(const std::initializer_list<Napi::Value>& args) const {
  //EscapableHandleScope scope{_env};
  //Napi::Value result = Value().Call(args);
  //if (scope.Env().IsExceptionPending()) {
  //  return Value();
  //}
  //return scope.Escape(result);
  //throw std::runtime_error{"TODO"};
  return Value().Call(args);
}

inline Napi::Value FunctionReference::Call(const std::vector<Napi::Value>& args) const {
  //EscapableHandleScope scope{_env};
  //Napi::Value result = Value().Call(args);
  //if (scope.Env().IsExceptionPending()) {
  //  return Value();
  //}
  //return scope.Escape(result);
  //throw std::runtime_error{"TODO"};
  return Value().Call(args);
}

inline Napi::Value FunctionReference::Call(
    const Napi::Value& recv, const std::initializer_list<Napi::Value>& args) const {
  //EscapableHandleScope scope{_env};
  //Napi::Value result = Value().Call(recv, args);
  //if (scope.Env().IsExceptionPending()) {
  //  return Value();
  //}
  //return scope.Escape(result);
  //throw std::runtime_error{"TODO"};
  return Value().Call(recv, args);
}

inline Napi::Value FunctionReference::Call(
    const Napi::Value& recv, const std::vector<Napi::Value>& args) const {
  //EscapableHandleScope scope{_env};
  //Napi::Value result = Value().Call(recv, args);
  //if (scope.Env().IsExceptionPending()) {
  //  return Value();
  //}
  //return scope.Escape(result);
  //throw std::runtime_error{"TODO"};
  return Value().Call(recv, args);
}

inline Napi::Value FunctionReference::Call(
    const Napi::Value& recv, size_t argc, const Napi::Value* args) const {
  //EscapableHandleScope scope{_env};
  //Napi::Value result = Value().Call(recv, argc, args);
  //if (scope.Env().IsExceptionPending()) {
  //  return Value();
  //}
  //return scope.Escape(result);
  //throw std::runtime_error{"TODO"};
  return Value().Call(recv, argc, args);
}

inline Object FunctionReference::New(const std::initializer_list<Napi::Value>& args) const {
  return Value().New(args);
}

inline Object FunctionReference::New(const std::vector<Napi::Value>& args) const {
  return Value().New(args);
}

////////////////////////////////////////////////////////////////////////////////
// CallbackInfo class
////////////////////////////////////////////////////////////////////////////////

inline CallbackInfo::CallbackInfo(napi_env env, const jsi::Value& thisVal, const jsi::Value* args, size_t argc, const jsi::Value& newTarget, void* data)
    : _env{env}, _this{_env->rt, thisVal}, _argc{argc}, _dynamicArgs{nullptr}, _newTarget{_env->rt, newTarget}, _data{data} {
  if (_argc <= std::size(_staticArgs)) {
    _argv = _staticArgs;
  } else {
    _dynamicArgs = new jsi::Value[_argc];
    _argv = _dynamicArgs;
  }

  for (size_t i = 0; i < _argc; ++i) {
    _argv[i] = {_env->rt, args[i]};
  }
}

inline CallbackInfo::~CallbackInfo() {
  if (_dynamicArgs != nullptr) {
    delete[] _dynamicArgs;
  }
}

inline Value CallbackInfo::NewTarget() const {
  return {_env, jsi::Value{_env->rt, _newTarget}};
}

inline bool CallbackInfo::IsConstructCall() const {
  return !NewTarget().IsUndefined();
}

inline Napi::Env CallbackInfo::Env() const {
  return {_env};
}

inline size_t CallbackInfo::Length() const {
  return _argc;
}

inline const Value CallbackInfo::operator [](size_t index) const {
  return index < _argc ? Value{_env, {_env->rt, _argv[index]}} : Env().Undefined();
}

inline Value CallbackInfo::This() const {
  return {_env, {_env->rt, _this}};
}

inline void* CallbackInfo::Data() const {
  return _data;
}

inline void CallbackInfo::SetData(void* data) {
  _data = data;
}

////////////////////////////////////////////////////////////////////////////////
// PropertyDescriptor class
////////////////////////////////////////////////////////////////////////////////

//template <typename Getter>
//inline PropertyDescriptor
//PropertyDescriptor::Accessor(Napi::Env env,
//                             Napi::Object object,
//                             const char* utf8name,
//                             Getter getter,
//                             napi_property_attributes attributes,
//                             void* data) {
//  //typedef details::CallbackData<Getter, Napi::Value> CbData;
//  //auto callbackData = new CbData({ getter, data });
//
//  //napi_status status = AttachData(env, object, callbackData);
//  //if (status != napi_ok) {
//  //  delete callbackData;
//  //  NAPI_THROW_IF_FAILED(env, status, napi_property_descriptor());
//  //}
//
//  //return PropertyDescriptor({
//  //  utf8name,
//  //  nullptr,
//  //  nullptr,
//  //  CbData::Wrapper,
//  //  nullptr,
//  //  nullptr,
//  //  attributes,
//  //  callbackData
//  //});
//  throw std::runtime_error{"TODO"};
//}
//
//template <typename Getter>
//inline PropertyDescriptor PropertyDescriptor::Accessor(Napi::Env env,
//                                                       Napi::Object object,
//                                                       const std::string& utf8name,
//                                                       Getter getter,
//                                                       napi_property_attributes attributes,
//                                                       void* data) {
//  return Accessor(env, object, utf8name.c_str(), getter, attributes, data);
//}
//
//template <typename Getter>
//inline PropertyDescriptor PropertyDescriptor::Accessor(Napi::Env env,
//                                                       Napi::Object object,
//                                                       Name name,
//                                                       Getter getter,
//                                                       napi_property_attributes attributes,
//                                                       void* data) {
//  //typedef details::CallbackData<Getter, Napi::Value> CbData;
//  //auto callbackData = new CbData({ getter, data });
//
//  //napi_status status = AttachData(env, object, callbackData);
//  //if (status != napi_ok) {
//  //  delete callbackData;
//  //  NAPI_THROW_IF_FAILED(env, status, napi_property_descriptor());
//  //}
//
//  //return PropertyDescriptor({
//  //  nullptr,
//  //  name,
//  //  nullptr,
//  //  CbData::Wrapper,
//  //  nullptr,
//  //  nullptr,
//  //  attributes,
//  //  callbackData
//  //});
//  throw std::runtime_error{"TODO"};
//}
//
//template <typename Getter, typename Setter>
//inline PropertyDescriptor PropertyDescriptor::Accessor(Napi::Env env,
//                                                       Napi::Object object,
//                                                       const char* utf8name,
//                                                       Getter getter,
//                                                       Setter setter,
//                                                       napi_property_attributes attributes,
//                                                       void* data) {
//  //typedef details::AccessorCallbackData<Getter, Setter> CbData;
//  //auto callbackData = new CbData({ getter, setter, data });
//
//  //napi_status status = AttachData(env, object, callbackData);
//  //if (status != napi_ok) {
//  //  delete callbackData;
//  //  NAPI_THROW_IF_FAILED(env, status, napi_property_descriptor());
//  //}
//
//  //return PropertyDescriptor({
//  //  utf8name,
//  //  nullptr,
//  //  nullptr,
//  //  CbData::GetterWrapper,
//  //  CbData::SetterWrapper,
//  //  nullptr,
//  //  attributes,
//  //  callbackData
//  //});
//  throw std::runtime_error{"TODO"};
//}
//
//template <typename Getter, typename Setter>
//inline PropertyDescriptor PropertyDescriptor::Accessor(Napi::Env env,
//                                                       Napi::Object object,
//                                                       const std::string& utf8name,
//                                                       Getter getter,
//                                                       Setter setter,
//                                                       napi_property_attributes attributes,
//                                                       void* data) {
//  return Accessor(env, object, utf8name.c_str(), getter, setter, attributes, data);
//}
//
//template <typename Getter, typename Setter>
//inline PropertyDescriptor PropertyDescriptor::Accessor(Napi::Env env,
//                                                       Napi::Object object,
//                                                       Name name,
//                                                       Getter getter,
//                                                       Setter setter,
//                                                       napi_property_attributes attributes,
//                                                       void* data) {
//  //typedef details::AccessorCallbackData<Getter, Setter> CbData;
//  //auto callbackData = new CbData({ getter, setter, data });
//
//  //napi_status status = AttachData(env, object, callbackData);
//  //if (status != napi_ok) {
//  //  delete callbackData;
//  //  NAPI_THROW_IF_FAILED(env, status, napi_property_descriptor());
//  //}
//
//  //return PropertyDescriptor({
//  //  nullptr,
//  //  name,
//  //  nullptr,
//  //  CbData::GetterWrapper,
//  //  CbData::SetterWrapper,
//  //  nullptr,
//  //  attributes,
//  //  callbackData
//  //});
//  throw std::runtime_error{"TODO"};
//}
//
//template <typename Callable>
//inline PropertyDescriptor PropertyDescriptor::Function(Napi::Env env,
//                                                       Napi::Object /*object*/,
//                                                       const char* utf8name,
//                                                       Callable cb,
//                                                       napi_property_attributes attributes,
//                                                       void* data) {
//  return PropertyDescriptor({
//    utf8name,
//    nullptr,
//    nullptr,
//    nullptr,
//    nullptr,
//    Napi::Function::New(env, cb, utf8name, data),
//    attributes,
//    nullptr
//  });
//}
//
//template <typename Callable>
//inline PropertyDescriptor PropertyDescriptor::Function(Napi::Env env,
//                                                       Napi::Object object,
//                                                       const std::string& utf8name,
//                                                       Callable cb,
//                                                       napi_property_attributes attributes,
//                                                       void* data) {
//  return Function(env, object, utf8name.c_str(), cb, attributes, data);
//}
//
//template <typename Callable>
//inline PropertyDescriptor PropertyDescriptor::Function(Napi::Env env,
//                                                       Napi::Object /*object*/,
//                                                       Name name,
//                                                       Callable cb,
//                                                       napi_property_attributes attributes,
//                                                       void* data) {
//  return PropertyDescriptor({
//    nullptr,
//    name,
//    nullptr,
//    nullptr,
//    nullptr,
//    Napi::Function::New(env, cb, nullptr, data),
//    attributes,
//    nullptr
//  });
//}
//
//inline PropertyDescriptor PropertyDescriptor::Value(const char* utf8name,
//                                                    jsi::Value value,
//                                                    napi_property_attributes attributes) {
//  //return PropertyDescriptor({
//  //  utf8name, {}, nullptr, nullptr, nullptr, value, attributes, nullptr
//  //});
//  throw std::runtime_error{"TODO"};
//}
//
//inline PropertyDescriptor PropertyDescriptor::Value(const std::string& utf8name,
//                                                    jsi::Value value,
//                                                    napi_property_attributes attributes) {
//  return Value(utf8name.c_str(), std::move(value), attributes);
//}
//
//inline PropertyDescriptor PropertyDescriptor::Value(jsi::Value name,
//                                                    jsi::Value value,
//                                                    napi_property_attributes attributes) {
//  //return PropertyDescriptor({
//  //  nullptr, name, nullptr, nullptr, nullptr, value, attributes, nullptr
//  //});
//  throw std::runtime_error{"TODO"};
//}
//
//inline PropertyDescriptor PropertyDescriptor::Value(Name name,
//                                                    Napi::Value value,
//                                                    napi_property_attributes attributes) {
//  //return PropertyDescriptor::Value(jsi::Value{name}, jsi::Value{value}, attributes);
//  throw std::runtime_error{"TODO"};
//}
//
//inline PropertyDescriptor::PropertyDescriptor(napi_property_descriptor desc)
//  /*: _desc(desc)*/ {
//  throw std::runtime_error{"TODO"};
//}
//
//inline PropertyDescriptor::operator napi_property_descriptor&() {
//  return _desc;
//}
//
//inline PropertyDescriptor::operator const napi_property_descriptor&() const {
//  return _desc;
//}

////////////////////////////////////////////////////////////////////////////////
// ObjectWrap<T> class
////////////////////////////////////////////////////////////////////////////////

template <typename T>
inline ObjectWrap<T>::ObjectWrap(const Napi::CallbackInfo& callbackInfo)
  : _env{callbackInfo.Env()} {
  //: Reference<Object>(callbackInfo.Env(), {}) {
  //napi_env env = callbackInfo.Env();
  //jsi::Value wrapper = callbackInfo.This();
  //napi_status status;
  //napi_ref ref;
  //T* instance = static_cast<T*>(this);
  //status = napi_wrap(env, wrapper, instance, FinalizeCallback, nullptr, &ref);
  //NAPI_THROW_IF_FAILED_VOID(env, status);

  //Reference<Object>* instanceRef = instance;
  //*instanceRef = Reference<Object>(env, ref);
  //throw std::runtime_error{"TODO"};
}

template<typename T>
inline ObjectWrap<T>::~ObjectWrap() {}

template<typename T>
Napi::Env ObjectWrap<T>::Env() const {
  return {_env};
}

template<typename T>
inline T* ObjectWrap<T>::Unwrap(napi_env env, const jsi::Object& wrapper) {
  auto native{wrapper.getProperty(env->rt, env->native_name)};
  return native.isUndefined() ? nullptr : native.getObject(env->rt).getHostObject<T>(env->rt).get();
}

template<typename T>
inline T* ObjectWrap<T>::Unwrap(Object wrapper) {
  return Unwrap(wrapper.Env(), wrapper);
}

template <typename T>
inline Function
ObjectWrap<T>::DefineClass(napi_env env,
                           const char* utf8name,
                           const size_t props_count,
                           const PropertyDescriptor* descriptors,
                           void* data) {
  jsi::Runtime& rt{env->rt};

  // TODO: Cache this somewhere?
  auto constructorFactoryCode{std::make_shared<const jsi::StringBuffer>("(function(constructor) { return function() { return constructor.apply(this, arguments); }; })")};
  auto createConstructor{rt.evaluateJavaScript(std::move(constructorFactoryCode), "").asObject(rt).asFunction(rt)};

  auto newTarget{std::make_shared<jsi::Value>()};

  jsi::Function constructor{createConstructor.call(rt,
    jsi::Function::createFromHostFunction(rt, jsi::PropNameID::forUtf8(rt, utf8name), 0,
      [env, newTarget, data](jsi::Runtime& rt, const jsi::Value& thisVal, const jsi::Value* args, size_t count) -> jsi::Value {
        CallbackInfo callbackInfo{env, thisVal, args, count, *newTarget, data};
        // TODO: use prototype to wrap object?
        thisVal.getObject(rt).setProperty(rt, env->native_name, jsi::Object::createFromHostObject(rt, std::make_shared<T>(callbackInfo)));
        return {};
      })).getObject(rt).getFunction(rt)};

  *newTarget = {rt, static_cast<const jsi::Object&>(constructor)};

  jsi::Object prototype{constructor.getProperty(rt, "prototype").getObject(rt)};

  for (size_t i = 0; i < props_count; ++i) {
    const PropertyDescriptor& p{descriptors[i]};
    jsi::PropNameID name{jsi::PropNameID::forUtf8(rt, p.utf8name)};
    jsi::Object descriptor{rt};
    descriptor.setProperty(rt, "configurable", jsi::Value{(p.attributes & napi_configurable) != 0});
    descriptor.setProperty(rt, "enumerable", jsi::Value{(p.attributes & napi_enumerable) != 0});

    if (p.staticVoidMethod != nullptr) {
      //throw std::runtime_error{"TODO"};
    } else if (p.staticMethod != nullptr) {
      //throw std::runtime_error{"TODO"};
    } else if (p.staticGetter != nullptr || p.staticSetter != nullptr) {
      //throw std::runtime_error{"TODO"};
    } else if (!p.staticValue.IsEmpty()) {
    } else if (p.instanceVoidMethod != nullptr) {
      descriptor.setProperty(rt, "value", jsi::Function::createFromHostFunction(rt, name, 0,
        [env, method{p.instanceVoidMethod}, data{p.data}](jsi::Runtime& rt, const jsi::Value& thisVal, const jsi::Value* args, size_t count) -> jsi::Value {
          T* nativeObject{Unwrap(env, thisVal.getObject(rt))};
          (nativeObject->*method)({env, thisVal, args, count, nullptr, data});
          return {};
        }));
    } else if (p.instanceMethod != nullptr) {
      descriptor.setProperty(rt, "value", jsi::Function::createFromHostFunction(rt, name, 0,
        [env, method{p.instanceMethod}, data{p.data}](jsi::Runtime& rt, const jsi::Value& thisVal, const jsi::Value* args, size_t count) -> jsi::Value {
          T* nativeObject{Unwrap(env, thisVal.getObject(rt))};
          return {rt, (nativeObject->*method)({env, thisVal, args, count, nullptr, data})};
        }));
    } else if (p.instanceGetter != nullptr || p.instanceSetter != nullptr) {
      if (p.instanceGetter != nullptr) {
        descriptor.setProperty(rt, "get", jsi::Function::createFromHostFunction(rt, name, 0,
          [env, getter{p.instanceGetter}, data{p.data}](jsi::Runtime& rt, const jsi::Value& thisVal, const jsi::Value* args, size_t count) -> jsi::Value {
            T* nativeObject{Unwrap(env, thisVal.getObject(rt))};
            return {rt, (nativeObject->*getter)({env, thisVal, args, count, nullptr, data})};
          }));
      }
      if (p.instanceSetter != nullptr) {
        descriptor.setProperty(rt, "set", jsi::Function::createFromHostFunction(rt, name, 0,
          [env, setter{p.instanceSetter}, data{p.data}](jsi::Runtime& rt, const jsi::Value& thisVal, const jsi::Value* args, size_t count) -> jsi::Value {
            T* nativeObject{Unwrap(env, thisVal.getObject(rt))};
            (nativeObject->*setter)({env, thisVal, args, count, nullptr, data}, {env, {rt, args[0]}});
            return {};
          }));
      }
    } else if (!p.instanceValue.IsEmpty()) {
      descriptor.setProperty(rt, "writable", jsi::Value{(p.attributes & napi_writable) != 0});
      descriptor.setProperty(rt, "value", static_cast<const jsi::Value&>(p.instanceValue));
    }

    jsi::Object object{rt.global().getPropertyAsObject(rt, "Object")};
    jsi::Function defineProperty{object.getPropertyAsFunction(rt, "defineProperty")};
    defineProperty.callWithThis(rt, object, prototype, p.utf8name, descriptor);
  }

  return {env, std::move(constructor)};
}

template <typename T>
inline Function ObjectWrap<T>::DefineClass(
    napi_env env,
    const char* utf8name,
    const std::initializer_list<PropertyDescriptor>& properties,
    void* data) {
  return DefineClass(env,
          utf8name,
          properties.size(),
          properties.begin(),
          data);
}

template <typename T>
inline Function ObjectWrap<T>::DefineClass(
    napi_env env,
    const char* utf8name,
    const std::vector<PropertyDescriptor>& properties,
    void* data) {
  return DefineClass(env,
           utf8name,
           properties.size(),
           properties.data(),
           data);
}

template <typename T>
inline typename ObjectWrap<T>::PropertyDescriptor ObjectWrap<T>::StaticMethod(
    const char* utf8name,
    StaticVoidMethodCallback method,
    napi_property_attributes attributes,
    void* data) {
  (void)utf8name;
  (void)method;
  (void)attributes;
  (void)data;
  //StaticVoidMethodCallbackData* callbackData = new StaticVoidMethodCallbackData({ method, data });

  //PropertyDescriptor desc = napi_property_descriptor();
  //desc.utf8name = utf8name;
  //desc.method = T::StaticVoidMethodCallbackWrapper;
  //desc.data = callbackData;
  //desc.attributes = static_cast<napi_property_attributes>(attributes | napi_static);
  //return desc;
  throw std::runtime_error{"TODO"};
}

template <typename T>
inline typename ObjectWrap<T>::PropertyDescriptor ObjectWrap<T>::StaticMethod(
    const char* utf8name,
    StaticMethodCallback method,
    napi_property_attributes attributes,
    void* data) {
  (void)utf8name;
  (void)method;
  (void)attributes;
  (void)data;
  //StaticMethodCallbackData* callbackData = new StaticMethodCallbackData({ method, data });

  //PropertyDescriptor desc = napi_property_descriptor();
  //desc.utf8name = utf8name;
  //desc.method = T::StaticMethodCallbackWrapper;
  //desc.data = callbackData;
  //desc.attributes = static_cast<napi_property_attributes>(attributes | napi_static);
  //return desc;
  throw std::runtime_error{"TODO"};
}

template <typename T>
inline typename ObjectWrap<T>::PropertyDescriptor ObjectWrap<T>::StaticMethod(
    Symbol name,
    StaticVoidMethodCallback method,
    napi_property_attributes attributes,
    void* data) {
  (void)name;
  (void)method;
  (void)attributes;
  (void)data;
  //StaticVoidMethodCallbackData* callbackData = new StaticVoidMethodCallbackData({ method, data });

  //PropertyDescriptor desc = napi_property_descriptor();
  //desc.name = name;
  //desc.method = T::StaticVoidMethodCallbackWrapper;
  //desc.data = callbackData;
  //desc.attributes = static_cast<napi_property_attributes>(attributes | napi_static);
  //return desc;
  throw std::runtime_error{"TODO"};
}

template <typename T>
inline typename ObjectWrap<T>::PropertyDescriptor ObjectWrap<T>::StaticMethod(
    Symbol name,
    StaticMethodCallback method,
    napi_property_attributes attributes,
    void* data) {
  (void)name;
  (void)method;
  (void)attributes;
  (void)data;
  //StaticMethodCallbackData* callbackData = new StaticMethodCallbackData({ method, data });

  //PropertyDescriptor desc = napi_property_descriptor();
  //desc.name = name;
  //desc.method = T::StaticMethodCallbackWrapper;
  //desc.data = callbackData;
  //desc.attributes = static_cast<napi_property_attributes>(attributes | napi_static);
  //return desc;
  throw std::runtime_error{"TODO"};
}

template <typename T>
inline typename ObjectWrap<T>::PropertyDescriptor ObjectWrap<T>::StaticAccessor(
    const char* utf8name,
    StaticGetterCallback getter,
    StaticSetterCallback setter,
    napi_property_attributes attributes,
    void* data) {
  (void)utf8name;
  (void)getter;
  (void)setter;
  (void)attributes;
  (void)data;
  //StaticAccessorCallbackData* callbackData =
  //  new StaticAccessorCallbackData({ getter, setter, data });

  //PropertyDescriptor desc = napi_property_descriptor();
  //desc.utf8name = utf8name;
  //desc.getter = getter != nullptr ? T::StaticGetterCallbackWrapper : nullptr;
  //desc.setter = setter != nullptr ? T::StaticSetterCallbackWrapper : nullptr;
  //desc.data = callbackData;
  //desc.attributes = static_cast<napi_property_attributes>(attributes | napi_static);
  //return desc;
  throw std::runtime_error{"TODO"};
}

template <typename T>
inline typename ObjectWrap<T>::PropertyDescriptor ObjectWrap<T>::StaticAccessor(
    Symbol name,
    StaticGetterCallback getter,
    StaticSetterCallback setter,
    napi_property_attributes attributes,
    void* data) {
  (void)name;
  (void)getter;
  (void)setter;
  (void)attributes;
  (void)data;
  //StaticAccessorCallbackData* callbackData =
  //  new StaticAccessorCallbackData({ getter, setter, data });

  //PropertyDescriptor desc = napi_property_descriptor();
  //desc.name = name;
  //desc.getter = getter != nullptr ? T::StaticGetterCallbackWrapper : nullptr;
  //desc.setter = setter != nullptr ? T::StaticSetterCallbackWrapper : nullptr;
  //desc.data = callbackData;
  //desc.attributes = static_cast<napi_property_attributes>(attributes | napi_static);
  //return desc;
  throw std::runtime_error{"TODO"};
}

template <typename T>
inline typename ObjectWrap<T>::PropertyDescriptor ObjectWrap<T>::InstanceMethod(
    const char* utf8name,
    InstanceVoidMethodCallback method,
    napi_property_attributes attributes,
    void* data) {
  PropertyDescriptor desc{};
  desc.utf8name = utf8name;
  desc.instanceVoidMethod = method;
  desc.attributes = attributes;
  desc.data = data;
  return desc;
}

template <typename T>
inline typename ObjectWrap<T>::PropertyDescriptor ObjectWrap<T>::InstanceMethod(
    const char* utf8name,
    InstanceMethodCallback method,
    napi_property_attributes attributes,
    void* data) {
  PropertyDescriptor desc{};
  desc.utf8name = utf8name;
  desc.instanceMethod = method;
  desc.attributes = attributes;
  desc.data = data;
  return desc;
}

template <typename T>
inline typename ObjectWrap<T>::PropertyDescriptor ObjectWrap<T>::InstanceMethod(
    Symbol name,
    InstanceVoidMethodCallback method,
    napi_property_attributes attributes,
    void* data) {
  (void)name;
  (void)method;
  (void)attributes;
  (void)data;
  //InstanceVoidMethodCallbackData* callbackData =
  //  new InstanceVoidMethodCallbackData({ method, data});

  //napi_property_descriptor desc = napi_property_descriptor();
  //desc.name = name;
  //desc.method = T::InstanceVoidMethodCallbackWrapper;
  //desc.data = data;
  //desc.attributes = attributes;
  //return desc;
  throw std::runtime_error{"TODO"};
}

template <typename T>
inline typename ObjectWrap<T>::PropertyDescriptor ObjectWrap<T>::InstanceMethod(
    Symbol name,
    InstanceMethodCallback method,
    napi_property_attributes attributes,
    void* data) {
  (void)name;
  (void)method;
  (void)attributes;
  (void)data;
  //InstanceMethodCallbackData* callbackData = new InstanceMethodCallbackData({ method, data });

  //napi_property_descriptor desc = napi_property_descriptor();
  //desc.name = name;
  //desc.method = T::InstanceMethodCallbackWrapper;
  //desc.data = callbackData;
  //desc.attributes = attributes;
  //return desc;
  throw std::runtime_error{"TODO"};
}

template <typename T>
inline typename ObjectWrap<T>::PropertyDescriptor ObjectWrap<T>::InstanceAccessor(
    const char* utf8name,
    InstanceGetterCallback getter,
    InstanceSetterCallback setter,
    napi_property_attributes attributes,
    void* data) {
  PropertyDescriptor desc{};
  desc.utf8name = utf8name;
  desc.instanceGetter = getter;
  desc.instanceSetter = setter;
  desc.attributes = attributes;
  desc.data = data;
  return desc;
}

template <typename T>
inline typename ObjectWrap<T>::PropertyDescriptor ObjectWrap<T>::InstanceAccessor(
    Symbol name,
    InstanceGetterCallback getter,
    InstanceSetterCallback setter,
    napi_property_attributes attributes,
    void* data) {
  (void)name;
  (void)getter;
  (void)setter;
  (void)attributes;
  (void)data;
  //InstanceAccessorCallbackData* callbackData =
  //  new InstanceAccessorCallbackData({ getter, setter, data });

  //PropertyDescriptor desc = napi_property_descriptor();
  //desc.name = name;
  //desc.getter = getter != nullptr ? T::InstanceGetterCallbackWrapper : nullptr;
  //desc.setter = setter != nullptr ? T::InstanceSetterCallbackWrapper : nullptr;
  //desc.data = callbackData;
  //desc.attributes = attributes;
  //return desc;
  throw std::runtime_error{"TODO"};
}

template <typename T>
inline typename ObjectWrap<T>::PropertyDescriptor ObjectWrap<T>::StaticValue(const char* utf8name,
    Napi::Value value, napi_property_attributes attributes) {
  PropertyDescriptor desc{};
  desc.utf8name = utf8name;
  desc.staticValue = std::move(value);
  desc.attributes = static_cast<napi_property_attributes>(attributes);
  return desc;
}

template <typename T>
inline typename ObjectWrap<T>::PropertyDescriptor ObjectWrap<T>::StaticValue(Symbol name,
    Napi::Value value, napi_property_attributes attributes) {
  (void)name;
  (void)value;
  (void)attributes;
  //PropertyDescriptor desc = napi_property_descriptor();
  //desc.name = name;
  //desc.value = value;
  //desc.attributes = static_cast<napi_property_attributes>(attributes | napi_static);
  //return desc;
  throw std::runtime_error{"TODO"};
}

template <typename T>
inline typename ObjectWrap<T>::PropertyDescriptor ObjectWrap<T>::InstanceValue(
    const char* utf8name,
    Napi::Value value,
    napi_property_attributes attributes) {
  PropertyDescriptor desc{};
  desc.utf8name = utf8name;
  desc.instanceValue = std::move(value);
  desc.attributes = attributes;
  return desc;
}

template <typename T>
inline typename ObjectWrap<T>::PropertyDescriptor ObjectWrap<T>::InstanceValue(
    Symbol name,
    Napi::Value value,
    napi_property_attributes attributes) {
  (void)name;
  (void)value;
  (void)attributes;
  //PropertyDescriptor desc = napi_property_descriptor();
  //desc.name = name;
  //desc.value = value;
  //desc.attributes = attributes;
  //return desc;
  throw std::runtime_error{"TODO"};
}

template <typename T>
inline void ObjectWrap<T>::Finalize(napi_env /*env*/) {}

////////////////////////////////////////////////////////////////////////////////
// HandleScope class
////////////////////////////////////////////////////////////////////////////////

inline HandleScope::HandleScope(napi_env env) : _env{env} {
  // no-op
}

inline HandleScope::~HandleScope() {
  // no-op
}

inline Napi::Env HandleScope::Env() const {
  return {_env};
}

////////////////////////////////////////////////////////////////////////////////
// EscapableHandleScope class
////////////////////////////////////////////////////////////////////////////////

inline EscapableHandleScope::EscapableHandleScope(napi_env env) : _env{env} {
  // no-op
}

inline EscapableHandleScope::~EscapableHandleScope() {
  // no-op
}

inline Napi::Env EscapableHandleScope::Env() const {
  return {_env};
}

inline Value EscapableHandleScope::Escape(jsi::Value escapee) {
  // no-op
  return {_env, std::move(escapee)};
}

} // namespace Napi
