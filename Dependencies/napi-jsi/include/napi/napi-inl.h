#include <stdexcept>

namespace Napi {

using namespace facebook;

namespace details {

template <typename T>
class External : public facebook::jsi::HostObject {
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

inline Env::Env(napi_env env) : _env(env) {
}

inline Env::operator napi_env() const {
  return _env;
}

inline Object Env::Global() const {
  return {_env, {_env, _env->global()}};
}

inline Value Env::Undefined() const {
  return {_env, {_env, facebook::jsi::Value::undefined()}};
}

inline Value Env::Null() const {
  return {_env, {_env, facebook::jsi::Value::null()}};
}

////////////////////////////////////////////////////////////////////////////////
// Value class
////////////////////////////////////////////////////////////////////////////////

inline Value::Value() : _env(nullptr) {
}

inline Value::Value(napi_env env, napi_value value) : _env(env), _value(value) {
}

inline Value::operator napi_value() const {
  return _value;
}

inline bool Value::operator ==(const Value& other) const {
  return StrictEquals(other);
}

inline bool Value::operator !=(const Value& other) const {
  return !this->operator ==(other);
}

inline bool Value::StrictEquals(const Value& other) const {
  return facebook::jsi::Value::strictEquals(*_env, *_value, *other._value);
}

inline Napi::Env Value::Env() const {
  return Napi::Env(_env);
}

inline bool Value::IsEmpty() const {
  return _env == nullptr;
}

inline napi_valuetype Value::Type() const {
  if (_value->isUndefined()) {
    return napi_undefined;
  } else if (_value->isNull()) {
    return napi_null;
  } else if (_value->isBool()) {
    return napi_boolean;
  } else if (_value->isNumber()) {
    return napi_number;
  } else if (_value->isString()) {
    return napi_string;
  } else if (_value->isSymbol()) {
    return napi_symbol;
  } else if (_value->isObject()) {
    auto object{_value->getObject(*_env)};
    if (object.isFunction(*_env)) {
        return napi_function;
    } else if (object.isHostObject(*_env)) {
        return napi_external;
    }
    return napi_object;
  }

  assert(false);
  return napi_undefined;
}

inline bool Value::IsUndefined() const {
  return _value->isUndefined();
}

inline bool Value::IsNull() const {
  return _value->isNull();
}

inline bool Value::IsBoolean() const {
  return _value->isBool();
}

inline bool Value::IsNumber() const {
  return _value->isNumber();
}

inline bool Value::IsString() const {
  return _value->isString();
}

inline bool Value::IsSymbol() const {
  return _value->isSymbol();
}

inline bool Value::IsArray() const {
  if (!_value->isObject()) {
      return false;
  }

  auto object{_value->getObject(*_env)};
  return object.isArray(*_env);
}

inline bool Value::IsArrayBuffer() const {
  if (!_value->isObject()) {
      return false;
  }

  auto object{_value->getObject(*_env)};
  return object.isArrayBuffer(*_env);
}

inline bool Value::IsTypedArray() const {
  throw std::runtime_error{"TODO"};
}

inline bool Value::IsObject() const {
  return _value->isObject();
}

inline bool Value::IsFunction() const {
  return _value->isObject() && _value->getObject(*_env).isFunction(*_env);
}

inline bool Value::IsPromise() const {
  throw std::runtime_error{"TODO"};
}

inline bool Value::IsDataView() const {
  throw std::runtime_error{"TODO"};
}

inline bool Value::IsExternal() const {
  return _value->isObject() && _value->getObject(*_env).isHostObject(*_env);
}

template <typename T>
inline T Value::As() const {
  return T(_env, _value);
}

inline Boolean Value::ToBoolean() const {
  // TODO: need to coerce if not the right type
  return Boolean::New(_env, _value->getBool());
}

inline Number Value::ToNumber() const {
  // TODO: need to coerce if not the right type
  return Number::New(_env, _value->getNumber());
}

inline String Value::ToString() const {
  return {_env, {_env, _value->toString(*_env)}};
}

inline Object Value::ToObject() const {
  return {_env, {_env, _value->asObject(*_env)}};
}

////////////////////////////////////////////////////////////////////////////////
// Boolean class
////////////////////////////////////////////////////////////////////////////////

inline Boolean Boolean::New(napi_env env, bool val) {
  return Boolean(env, {env, facebook::jsi::Value{val}});
}

inline Boolean::Boolean() : Napi::Value() {
}

inline Boolean::Boolean(napi_env env, napi_value value) : Napi::Value(env, value) {
}

inline Boolean::operator bool() const {
  return Value();
}

inline bool Boolean::Value() const {
  return _value->getBool();
}

////////////////////////////////////////////////////////////////////////////////
// Number class
////////////////////////////////////////////////////////////////////////////////

inline Number Number::New(napi_env env, double val) {
  return Number(env, {env, facebook::jsi::Value(val)});
}

inline Number::Number() : Value() {
}

inline Number::Number(napi_env env, napi_value value) : Value(env, value) {
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
  return static_cast<int32_t>(_value->getNumber());
}

inline uint32_t Number::Uint32Value() const {
  return static_cast<uint32_t>(_value->getNumber());
}

inline int64_t Number::Int64Value() const {
  return static_cast<int64_t>(_value->getNumber());
}

inline float Number::FloatValue() const {
  return static_cast<float>(_value->getNumber());
}

inline double Number::DoubleValue() const {
  return _value->getNumber();
}

////////////////////////////////////////////////////////////////////////////////
// Name class
////////////////////////////////////////////////////////////////////////////////

inline Name::Name() : Value() {
}

inline Name::Name(napi_env env, napi_value value) : Value(env, value) {
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
  return String(env, {env, facebook::jsi::String::createFromUtf8(*env, val)});
}

inline String String::New(napi_env env, const char16_t* val) {
  throw std::runtime_error("TODO");
}

inline String String::New(napi_env env, const char* val, size_t length) {
  return String(env, {env, facebook::jsi::String::createFromUtf8(*env, reinterpret_cast<const uint8_t*>(val), length)});
}

inline String String::New(napi_env env, const char16_t* val, size_t length) {
  throw std::runtime_error("TODO");
}

inline String::String() : Name() {
}

inline String::String(napi_env env, napi_value value) : Name(env, value) {
}

inline String::operator std::string() const {
  return Utf8Value();
}

inline String::operator std::u16string() const {
  return Utf16Value();
}

inline std::string String::Utf8Value() const {
  return _value->getString(*_env).utf8(*_env);
}

inline std::u16string String::Utf16Value() const {
  throw std::runtime_error("TODO");
}

////////////////////////////////////////////////////////////////////////////////
// Symbol class
////////////////////////////////////////////////////////////////////////////////

inline Symbol Symbol::New(napi_env env, const char* description) {
  throw std::runtime_error("TODO");
}

inline Symbol Symbol::New(napi_env env, const std::string& description) {
  throw std::runtime_error("TODO");
}

inline Symbol Symbol::New(napi_env env, String description) {
  throw std::runtime_error("TODO");
}

inline Symbol Symbol::New(napi_env env, napi_value description) {
  throw std::runtime_error("TODO");
}

inline Symbol Symbol::WellKnown(napi_env env, const std::string& name) {
  return Napi::Env(env).Global().Get("Symbol").As<Object>().Get(name).As<Symbol>();
}

inline Symbol::Symbol() : Name() {
}

inline Symbol::Symbol(napi_env env, napi_value value) : Name(env, value) {
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

template <typename T>
struct vf_fallback {
  static Value From(napi_env env, const T& value) {
    return Value(env, value);
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
  return Object(_env, _object).Get(_key);
}

template <typename Key> template <typename ValueType>
inline Object::PropertyLValue<Key>& Object::PropertyLValue<Key>::operator =(ValueType value) {
  Object(_env, _object).Set(_key, value);
  return *this;
}

template <typename Key>
inline Object::PropertyLValue<Key>::PropertyLValue(Object object, Key key)
  : _env(object.Env()), _object(object), _key(key) {}

inline Object Object::New(napi_env env) {
  return {env, {env, facebook::jsi::Object(*env)}};
}

inline Object::Object() : Value() {
}

inline Object::Object(napi_env env, napi_value value) : Value(env, value) {
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

inline bool Object::Has(napi_value key) const {
  throw std::runtime_error("TODO");
}

inline bool Object::Has(const char* utf8name) const {
  return _value->getObject(*_env).hasProperty(*_env, utf8name);
}

inline bool Object::Has(const std::string& utf8name) const {
  return Has(utf8name.c_str());
}

inline bool Object::HasOwnProperty(napi_value key) const {
  throw std::runtime_error("TODO");
}

inline bool Object::HasOwnProperty(const char* utf8name) const {
  throw std::runtime_error("TODO");
}

inline bool Object::HasOwnProperty(const std::string& utf8name) const {
  return HasOwnProperty(utf8name.c_str());
}

inline Value Object::Get(napi_value key) const {
  throw std::runtime_error("TODO");
}

inline Value Object::Get(const char* utf8name) const {
  return {_env, {_env, _value->getObject(*_env).getProperty(*_env, utf8name)}};
}

inline Value Object::Get(const std::string& utf8name) const {
  return Get(utf8name.c_str());
}

template <typename ValueType>
inline void Object::Set(napi_value key, const ValueType& value) {
  throw std::runtime_error("TODO");
}

template <typename ValueType>
inline void Object::Set(const char* utf8name, const ValueType& value) {
  _value->getObject(*_env).setProperty(*_env, utf8name, *static_cast<napi_value>(Value::From(_env, value)));
}

template <typename ValueType>
inline void Object::Set(const std::string& utf8name, const ValueType& value) {
  Set(utf8name.c_str(), value);
}

inline bool Object::Delete(napi_value key) {
  throw std::runtime_error("TODO");
}

inline bool Object::Delete(const char* utf8name) {
  throw std::runtime_error("TODO");
}

inline bool Object::Delete(const std::string& utf8name) {
  return Delete(utf8name.c_str());
}

inline bool Object::Has(uint32_t index) const {
  throw std::runtime_error{"TODO"};
}

inline Value Object::Get(uint32_t index) const {
  return {_env, {_env, _value->getObject(*_env).getArray(*_env).getValueAtIndex(*_env, static_cast<size_t>(index))}};
}

template <typename ValueType>
inline void Object::Set(uint32_t index, const ValueType& value) {
  _value->getObject(*_env).getArray(*_env).setValueAtIndex(*_env, static_cast<size_t>(index), *napi_value{Value::From(_env, value)});
}

inline bool Object::Delete(uint32_t index) {
  throw std::runtime_error{"TODO"};
}

inline Array Object::GetPropertyNames() const {
  throw std::runtime_error{"TODO"};
}

inline void Object::DefineProperty(const PropertyDescriptor& property) {
  throw std::runtime_error{"TODO"};
}

inline void Object::DefineProperties(const std::initializer_list<PropertyDescriptor>& properties) {
  throw std::runtime_error{"TODO"};
}

inline void Object::DefineProperties(const std::vector<PropertyDescriptor>& properties) {
  throw std::runtime_error{"TODO"};
}

inline bool Object::InstanceOf(const Function& constructor) const {
  throw std::runtime_error{"TODO"};
}

////////////////////////////////////////////////////////////////////////////////
// External class
////////////////////////////////////////////////////////////////////////////////

template <typename T>
inline External<T> External<T>::New(napi_env env, T* data) {
  return {env, {env, facebook::jsi::Object::createFromHostObject(*env, std::make_shared<details::External<T>>(env, data))}};
}

template <typename T>
template <typename Finalizer>
inline External<T> External<T>::New(napi_env env,
                                    T* data,
                                    Finalizer finalizeCallback) {
  return {env, {env, facebook::jsi::Object::createFromHostObject(*env, std::make_shared<details::ExternalWithFinalizer<T, Finalizer>>(env, data, std::move(finalizeCallback)))}};
}

template <typename T>
template <typename Finalizer, typename Hint>
inline External<T> External<T>::New(napi_env env,
                                    T* data,
                                    Finalizer finalizeCallback,
                                    Hint* finalizeHint) {
  return {env, {env, facebook::jsi::Object::createFromHostObject(*env, std::make_shared<details::ExternalWithFinalizerAndHint<T, Finalizer, Hint>>(env, data, std::move(finalizeCallback), finalizeHint))}};
}

template <typename T>
inline External<T>::External() : Value() {
}

template <typename T>
inline External<T>::External(napi_env env, napi_value value) : Value(env, value) {
}

template <typename T>
inline T* External<T>::Data() const {
  return _value->getObject(*_env).template getHostObject<details::External<T>>(*_env)->Data();
}

////////////////////////////////////////////////////////////////////////////////
// Array class
////////////////////////////////////////////////////////////////////////////////

inline Array Array::New(napi_env env) {
  return New(env, 0);
}

inline Array Array::New(napi_env env, size_t length) {
  return {env, {env, jsi::Array{*env, length}}};
}

inline Array::Array() : Object() {
}

inline Array::Array(napi_env env, napi_value value) : Object(env, value) {
}

inline uint32_t Array::Length() const {
  return static_cast<uint32_t>(_value->getObject(*_env).getArray(*_env).length(*_env));
}

////////////////////////////////////////////////////////////////////////////////
// ArrayBuffer class
////////////////////////////////////////////////////////////////////////////////

inline ArrayBuffer ArrayBuffer::New(napi_env env, size_t byteLength) {
  throw std::runtime_error{"TODO"};
}

inline ArrayBuffer ArrayBuffer::New(napi_env env,
                                    void* externalData,
                                    size_t byteLength) {
  throw std::runtime_error{"TODO"};
}

template <typename Finalizer>
inline ArrayBuffer ArrayBuffer::New(napi_env env,
                                    void* externalData,
                                    size_t byteLength,
                                    Finalizer finalizeCallback) {
  throw std::runtime_error{"TODO"};
}

template <typename Finalizer, typename Hint>
inline ArrayBuffer ArrayBuffer::New(napi_env env,
                                    void* externalData,
                                    size_t byteLength,
                                    Finalizer finalizeCallback,
                                    Hint* finalizeHint) {
  throw std::runtime_error{"TODO"};
}

inline ArrayBuffer::ArrayBuffer() : Object(), _data(nullptr), _length(0) {
}

inline ArrayBuffer::ArrayBuffer(napi_env env, napi_value value)
  : Object(env, value), _data(nullptr), _length(0) {
}

inline ArrayBuffer::ArrayBuffer(napi_env env, napi_value value, void* data, size_t length)
  : Object(env, value), _data(data), _length(length) {
}

inline void* ArrayBuffer::Data() const {
  EnsureInfo();
  return _data;
}

inline size_t ArrayBuffer::ByteLength() const {
  EnsureInfo();
  return _length;
}

inline void ArrayBuffer::EnsureInfo() const {
  //// The ArrayBuffer instance may have been constructed from a napi_value whose
  //// length/data are not yet known. Fetch and cache these values just once,
  //// since they can never change during the lifetime of the ArrayBuffer.
  //if (_data == nullptr) {
  //  napi_status status = napi_get_arraybuffer_info(_env, _value, &_data, &_length);
  //  NAPI_THROW_IF_FAILED_VOID(_env, status);
  //}
  throw std::runtime_error{"TODO"};
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
  //if (byteOffset + byteLength > arrayBuffer.ByteLength()) {
  //  NAPI_THROW(RangeError::New(env, "Invalid DataView length"),
  //             DataView());
  //}
  //napi_value value;
  //napi_status status = napi_create_dataview(
  //  env, byteLength, arrayBuffer, byteOffset, &value);
  //NAPI_THROW_IF_FAILED(env, status, DataView());
  //return DataView(env, value);
  throw std::runtime_error{"TODO"};
}

inline DataView::DataView() : Object() {
}

inline DataView::DataView(napi_env env, napi_value value) : Object(env, value) {
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
  //napi_value arrayBuffer;
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

inline TypedArray::TypedArray()
  : Object(), _type(TypedArray::unknown_array_type), _length(0) {
}

inline TypedArray::TypedArray(napi_env env, napi_value value)
  : Object(env, value), _type(TypedArray::unknown_array_type), _length(0) {
}

inline TypedArray::TypedArray(napi_env env,
                              napi_value value,
                              napi_typedarray_type type,
                              size_t length)
  : Object(env, value), _type(type), _length(length) {
}

inline napi_typedarray_type TypedArray::TypedArrayType() const {
  if (_type == TypedArray::unknown_array_type) {
    GetTypedArrayInfo(_env, _value, &_type, &_length, nullptr, nullptr, nullptr);
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
    GetTypedArrayInfo(_env, _value, &_type, &_length, nullptr, nullptr, nullptr);
  }

  return _length;
}

inline size_t TypedArray::ByteOffset() const {
  size_t byteOffset;
  GetTypedArrayInfo(_env, _value, nullptr, nullptr, nullptr, nullptr, &byteOffset);
  return byteOffset;
}

inline size_t TypedArray::ByteLength() const {
  return ElementSize() * ElementLength();
}

inline Napi::ArrayBuffer TypedArray::ArrayBuffer() const {
  napi_value arrayBuffer;
  GetTypedArrayInfo(_env, _value, nullptr, nullptr, nullptr, &arrayBuffer, nullptr);
  return Napi::ArrayBuffer(_env, std::move(arrayBuffer));
}

inline void TypedArray::GetTypedArrayInfo(napi_env env,
                                          const napi_value& value,
                                          napi_typedarray_type* type,
                                          size_t* length,
                                          void** data,
                                          napi_value* buffer,
                                          size_t* byteOffset) {
  jsi::Object typedArray{value->getObject(*env)};

  if (type != nullptr) {
    std::string name{typedArray.getPropertyAsObject(*env, "constructor").getProperty(*env, "name").getString(*env).utf8(*env)};
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
    *length = static_cast<size_t>(typedArray.getProperty(*env, "length").asNumber());
  }

  if (byteOffset != nullptr) {
    *byteOffset = static_cast<size_t>(typedArray.getProperty(*env, "byteOffset").asNumber());
  }

  if (data != nullptr || buffer != nullptr) {
    jsi::ArrayBuffer _buffer{typedArray.getPropertyAsObject(*env, "buffer").getArrayBuffer(*env)};

    if (data != nullptr) {
      size_t _byteOffset{byteOffset ? *byteOffset : static_cast<size_t>(typedArray.getProperty(*env, "byteOffset").asNumber())};
      *data = _buffer.data(*env) + _byteOffset;
    }

    if (buffer != nullptr) {
      *buffer = {env, {*env, _buffer}};
    }
  }
}


////////////////////////////////////////////////////////////////////////////////
// TypedArrayOf<T> class
////////////////////////////////////////////////////////////////////////////////

template <typename T>
inline TypedArrayOf<T> TypedArrayOf<T>::New(napi_env env,
                                            size_t elementLength,
                                            napi_typedarray_type type) {
  Napi::ArrayBuffer arrayBuffer = Napi::ArrayBuffer::New(env, elementLength * sizeof (T));
  return New(env, elementLength, arrayBuffer, 0, type);
}

template <typename T>
inline TypedArrayOf<T> TypedArrayOf<T>::New(napi_env env,
                                            size_t elementLength,
                                            Napi::ArrayBuffer arrayBuffer,
                                            size_t bufferOffset,
                                            napi_typedarray_type type) {
  //napi_value value;
  //napi_status status = napi_create_typedarray(
  //  env, type, elementLength, arrayBuffer, bufferOffset, &value);
  //NAPI_THROW_IF_FAILED(env, status, TypedArrayOf<T>());

  //return TypedArrayOf<T>(
  //  env, value, type, elementLength,
  //  reinterpret_cast<T*>(reinterpret_cast<uint8_t*>(arrayBuffer.Data()) + bufferOffset));
  throw std::runtime_error{"TODO"};
}

template <typename T>
inline TypedArrayOf<T>::TypedArrayOf() : TypedArray(), _data(nullptr) {
}

template <typename T>
inline TypedArrayOf<T>::TypedArrayOf(napi_env env, napi_value value)
  : TypedArray(env, value) {
  GetTypedArrayInfo(env, value, &_type, &_length, reinterpret_cast<void**>(&_data), nullptr, nullptr);
}

template <typename T>
inline TypedArrayOf<T>::TypedArrayOf(napi_env env,
                                     napi_value value,
                                     napi_typedarray_type type,
                                     size_t length,
                                     T* data)
  : TypedArray(env, value, type, length), _data(data) {
  //if (!(type == TypedArrayTypeForPrimitiveType<T>() ||
  //    (type == napi_uint8_clamped_array && std::is_same<T, uint8_t>::value))) {
  //  NAPI_THROW_VOID(TypeError::New(env, "Array type must match the template parameter. "
  //    "(Uint8 arrays may optionally have the \"clamped\" array type.)"));
  //}
  throw std::runtime_error{"TODO"};
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

// TODO
//template <typename CbData>
//static inline napi_status
//CreateFunction(napi_env env,
//               const char* utf8name,
//               napi_callback cb,
//               CbData* data,
//               napi_value* result) {
//  napi_status status =
//      napi_create_function(env, utf8name, NAPI_AUTO_LENGTH, cb, data, result);
//  if (status == napi_ok) {
//    status = Napi::details::AttachData(env, *result, data);
//  }
//
//  return status;
//}

template <typename Callable>
inline Function Function::New(napi_env env,
                              Callable cb,
                              const char* utf8name,
                              void* data) {
  auto function{jsi::Function::createFromHostFunction(*env, jsi::PropNameID::forUtf8(*env, utf8name), 0,
    [cb{std::move(cb)}, data](jsi::Runtime& rt, const jsi::Value& thisVal, const jsi::Value* args, size_t count) -> jsi::Value {
      CallbackInfo callbackInfo{&rt, thisVal, args, count, {}, data};
      cb(callbackInfo);
      return {};
    })};

  return {env, {env, std::move(function)}};
}

template <typename Callable>
inline Function Function::New(napi_env env,
                              Callable cb,
                              const std::string& utf8name,
                              void* data) {
  return New(env, cb, utf8name.c_str(), data);
}

inline Function::Function() : Object() {
}

inline Function::Function(napi_env env, napi_value value) : Object(env, value) {
}

inline Value Function::operator ()(const std::initializer_list<napi_value>& args) const {
  return Call(Env().Undefined(), args);
}

inline Value Function::Call(const std::initializer_list<napi_value>& args) const {
  return Call(Env().Undefined(), args);
}

inline Value Function::Call(const std::vector<napi_value>& args) const {
  return Call(Env().Undefined(), args);
}

inline Value Function::Call(size_t argc, const napi_value* args) const {
  return Call(Env().Undefined(), argc, args);
}

inline Value Function::Call(napi_value recv, const std::initializer_list<napi_value>& args) const {
  return Call(recv, args.size(), args.begin());
}

inline Value Function::Call(napi_value recv, const std::vector<napi_value>& args) const {
  return Call(recv, args.size(), args.data());
}

inline Value Function::Call(napi_value recv, size_t argc, const napi_value* args) const {
  jsi::Value stackArgs[6];
  std::vector<jsi::Value> heapArgs;
  jsi::Value* argv;
  if (argc <= std::size(stackArgs)) {
    argv = stackArgs;
  } else {
    heapArgs.resize(argc);
    argv = heapArgs.data();
  }

  for (int i = 0; i < argc; ++i) {
    argv[i] = {*_env, *args[i]};
  }

  jsi::Function function{_value->getObject(*_env).getFunction(*_env)};

  jsi::Value result{
    recv->isUndefined()
      ? function.call(*_env, static_cast<const jsi::Value*>(argv), argc)
      : function.callWithThis(*_env, recv->asObject(*_env), static_cast<const jsi::Value*>(argv), argc)};

  return {_env, {_env, result}};
}

inline Object Function::New(const std::initializer_list<napi_value>& args) const {
  return New(args.size(), args.begin());
}

inline Object Function::New(const std::vector<napi_value>& args) const {
  return New(args.size(), args.data());
}

inline Object Function::New(size_t argc, const napi_value* args) const {
  jsi::Value stackArgs[6];
  std::vector<jsi::Value> heapArgs;
  jsi::Value* argv;
  if (argc <= std::size(stackArgs)) {
    argv = stackArgs;
  } else {
    heapArgs.resize(argc);
    argv = heapArgs.data();
  }

  for (int i = 0; i < argc; ++i) {
    argv[i] = {*_env, *args[i]};
  }

  jsi::Value instance{_value->getObject(*_env).getFunction(*_env).callAsConstructor(*_env, static_cast<const jsi::Value*>(argv), argc)};
  return {_env, {_env, instance}};
}

////////////////////////////////////////////////////////////////////////////////
// Promise class
////////////////////////////////////////////////////////////////////////////////

inline Promise::Deferred Promise::Deferred::New(napi_env env) {
  return Promise::Deferred(env);
}

inline Promise::Deferred::Deferred(napi_env env) : _env(env) {
  //napi_status status = napi_create_promise(_env, &_deferred, &_promise);
  //NAPI_THROW_IF_FAILED_VOID(_env, status);
  throw std::runtime_error{"TODO"};
}

inline Promise Promise::Deferred::Promise() const {
  //return Napi::Promise(_env, _promise);
  throw std::runtime_error{"TODO"};
}

inline Napi::Env Promise::Deferred::Env() const {
  return Napi::Env(_env);
}

inline void Promise::Deferred::Resolve(napi_value value) const {
  //napi_status status = napi_resolve_deferred(_env, _deferred, value);
  //NAPI_THROW_IF_FAILED_VOID(_env, status);
  throw std::runtime_error{"TODO"};
}

inline void Promise::Deferred::Reject(napi_value value) const {
  //napi_status status = napi_reject_deferred(_env, _deferred, value);
  //NAPI_THROW_IF_FAILED_VOID(_env, status);
  throw std::runtime_error{"TODO"};
}

inline Promise::Promise(napi_env env, napi_value value) : Object(env, value) {
}

////////////////////////////////////////////////////////////////////////////////
// Error class
////////////////////////////////////////////////////////////////////////////////

inline Error Error::New(napi_env env) {
  //napi_status status;
  //napi_value error = nullptr;

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

  //  napi_value message;
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
  //return Error::New<Error>(env, message, std::strlen(message), napi_create_error);
  throw std::runtime_error{"TODO"};
}

inline Error Error::New(napi_env env, const std::string& message) {
  //return Error::New<Error>(env, message.c_str(), message.size(), napi_create_error);
  throw std::runtime_error{"TODO"};
}

// TODO
//inline NAPI_NO_RETURN void Error::Fatal(const char* location, const char* message) {
//  // $HACK
//  //napi_fatal_error(location, NAPI_AUTO_LENGTH, message, NAPI_AUTO_LENGTH);
//  throw std::exception();
//}

inline Error::Error() : ObjectReference() {
}

inline Error::Error(napi_env env, napi_value value) : ObjectReference(env, {value}) {
  //if (value != nullptr) {
  //  napi_status status = napi_create_reference(env, value, 1, &_ref);

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
  //Reset();

  //_env = other.Env();
  //HandleScope scope(_env);

  //napi_value value = other.Value();
  //if (value != nullptr) {
  //  napi_status status = napi_create_reference(_env, value, 1, &_ref);
  //  NAPI_THROW_IF_FAILED(_env, status, *this);
  //}

  //return *this;
  throw std::runtime_error{"TODO"};
}

inline const std::string& Error::Message() const NAPI_NOEXCEPT {
  if (_message.size() == 0 && _env != nullptr) {
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
//  HandleScope scope(_env);
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
//  napi_value str;
//  napi_status status = napi_create_string_utf8(env, message, length, &str);
//  NAPI_THROW_IF_FAILED(env, status, TError());
//
//  napi_value error;
//  status = create_error(env, nullptr, str, &error);
//  NAPI_THROW_IF_FAILED(env, status, TError());
//
//  return TError(env, error);
//}

inline TypeError TypeError::New(napi_env env, const char* message) {
  //return Error::New<TypeError>(env, message, std::strlen(message), napi_create_type_error);
  throw std::runtime_error{"TODO"};
}

inline TypeError TypeError::New(napi_env env, const std::string& message) {
  //return Error::New<TypeError>(env, message.c_str(), message.size(), napi_create_type_error);
  throw std::runtime_error{"TODO"};
}

inline TypeError::TypeError() : Error() {
}

inline TypeError::TypeError(napi_env env, napi_value value) : Error(env, value) {
}

inline RangeError RangeError::New(napi_env env, const char* message) {
  //return Error::New<RangeError>(env, message, std::strlen(message), napi_create_range_error);
  throw std::runtime_error{"TODO"};
}

inline RangeError RangeError::New(napi_env env, const std::string& message) {
  //return Error::New<RangeError>(env, message.c_str(), message.size(), napi_create_range_error);
  throw std::runtime_error{"TODO"};
}

inline RangeError::RangeError() : Error() {
}

inline RangeError::RangeError(napi_env env, napi_value value) : Error(env, value) {
}

////////////////////////////////////////////////////////////////////////////////
// Reference<T> class
////////////////////////////////////////////////////////////////////////////////

template <typename T>
inline Reference<T> Reference<T>::New(const T& value, uint32_t initialRefcount) {
  //napi_env env = value.Env();
  //napi_value val = value;

  //if (val == nullptr) {
  //  return Reference<T>(env, nullptr);
  //}

  //napi_ref ref;
  //napi_status status = napi_create_reference(env, value, initialRefcount, &ref);
  //NAPI_THROW_IF_FAILED(env, status, Reference<T>());

  //return Reference<T>(env, ref);
  //throw std::runtime_error{"TODO"};
  return {value.Env(), {value}};
}


template <typename T>
inline Reference<T>::Reference() : _env(nullptr), _ref{}, _suppressDestruct(false) {
}

template <typename T>
inline Reference<T>::Reference(napi_env env, napi_ref ref)
  : _env(env), _ref(ref), _suppressDestruct(false) {
}

template <typename T>
inline Reference<T>::~Reference() {
  //if (_ref != nullptr) {
  //  if (!_suppressDestruct) {
  //    napi_delete_reference(_env, _ref);
  //  }

  //  _ref = nullptr;
  //}
  //throw std::runtime_error{"TODO"};
}

template <typename T>
inline Reference<T>::Reference(Reference<T>&& other)
  : _env(other._env), _ref(std::move(other._ref)), _suppressDestruct(other._suppressDestruct) {
  other._env = nullptr;
  other._suppressDestruct = false;
}

template <typename T>
inline Reference<T>& Reference<T>::operator =(Reference<T>&& other) {
  Reset();
  _env = other._env;
  _ref = std::move(other._ref);
  _suppressDestruct = other._suppressDestruct;
  other._env = nullptr;
  other._suppressDestruct = false;
  return *this;
}

template <typename T>
inline Reference<T>::Reference(const Reference<T>& other)
  : _env(other._env), _ref(other._ref), _suppressDestruct(false) {
  //HandleScope scope(_env);

  //napi_value value = other.Value();
  //if (value != nullptr) {
  //  // Copying is a limited scenario (currently only used for Error object) and always creates a
  //  // strong reference to the given value even if the incoming reference is weak.
  //  napi_status status = napi_create_reference(_env, value, 1, &_ref);
  //  NAPI_FATAL_IF_FAILED(status, "Reference<T>::Reference", "napi_create_reference");
  //}
  //throw std::runtime_error{"TODO"};
}

template <typename T>
inline Reference<T>::operator napi_ref() const {
  return _ref;
}

template <typename T>
inline bool Reference<T>::operator ==(const Reference<T> &other) const {
  HandleScope scope(_env);
  return this->Value().StrictEquals(other.Value());
}

template <typename T>
inline bool Reference<T>::operator !=(const Reference<T> &other) const {
  return !this->operator ==(other);
}

template <typename T>
inline Napi::Env Reference<T>::Env() const {
  return Napi::Env(_env);
}

template <typename T>
inline bool Reference<T>::IsEmpty() const {
  return _ref.value->isUndefined();
}

template <typename T>
inline T Reference<T>::Value() const {
  //if (_ref == nullptr) {
  //  return T(_env, nullptr);
  //}

  //napi_value value;
  //napi_status status = napi_get_reference_value(_env, _ref, &value);
  //NAPI_THROW_IF_FAILED(_env, status, T());
  //return T(_env, value);
  //throw std::runtime_error{"TODO"};
  return T{_env, _ref.value};
}

template <typename T>
inline uint32_t Reference<T>::Ref() {
  //uint32_t result;
  //napi_status status = napi_reference_ref(_env, _ref, &result);
  //NAPI_THROW_IF_FAILED(_env, status, 1);
  //return result;
  //throw std::runtime_error{"TODO"};
}

template <typename T>
inline uint32_t Reference<T>::Unref() {
  //uint32_t result;
  //napi_status status = napi_reference_unref(_env, _ref, &result);
  //NAPI_THROW_IF_FAILED(_env, status, 1);
  //return result;
  //throw std::runtime_error{"TODO"};
}

template <typename T>
inline void Reference<T>::Reset() {
  //if (_ref != nullptr) {
  //  napi_status status = napi_delete_reference(_env, _ref);
  //  NAPI_THROW_IF_FAILED_VOID(_env, status);
  //  _ref = nullptr;
  //}
  //throw std::runtime_error{"TODO"};
}

template <typename T>
inline void Reference<T>::Reset(const T& value, uint32_t refcount) {
  //Reset();
  //_env = value.Env();

  //napi_value val = value;
  //if (val != nullptr) {
  //  napi_status status = napi_create_reference(_env, value, refcount, &_ref);
  //  NAPI_THROW_IF_FAILED_VOID(_env, status);
  //}
  //throw std::runtime_error{"TODO"};
  _env = value.Env();
  _ref.value = value;
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

inline ObjectReference::ObjectReference(): Reference<Object>() {
}

inline ObjectReference::ObjectReference(napi_env env, napi_ref ref): Reference<Object>(env, ref) {
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
  //EscapableHandleScope scope(_env);
  //return scope.Escape(Value().Get(utf8name));
  throw std::runtime_error{"TODO"};
}

inline Napi::Value ObjectReference::Get(const std::string& utf8name) const {
  //EscapableHandleScope scope(_env);
  //return scope.Escape(Value().Get(utf8name));
  throw std::runtime_error{"TODO"};
}

inline void ObjectReference::Set(const char* utf8name, napi_value value) {
  HandleScope scope(_env);
  Value().Set(utf8name, value);
}

inline void ObjectReference::Set(const char* utf8name, Napi::Value value) {
  HandleScope scope(_env);
  Value().Set(utf8name, value);
}

inline void ObjectReference::Set(const char* utf8name, const char* utf8value) {
  HandleScope scope(_env);
  Value().Set(utf8name, utf8value);
}

inline void ObjectReference::Set(const char* utf8name, bool boolValue) {
  HandleScope scope(_env);
  Value().Set(utf8name, boolValue);
}

inline void ObjectReference::Set(const char* utf8name, double numberValue) {
  HandleScope scope(_env);
  Value().Set(utf8name, numberValue);
}

inline void ObjectReference::Set(const std::string& utf8name, napi_value value) {
  HandleScope scope(_env);
  Value().Set(utf8name, value);
}

inline void ObjectReference::Set(const std::string& utf8name, Napi::Value value) {
  HandleScope scope(_env);
  Value().Set(utf8name, value);
}

inline void ObjectReference::Set(const std::string& utf8name, std::string& utf8value) {
  HandleScope scope(_env);
  Value().Set(utf8name, utf8value);
}

inline void ObjectReference::Set(const std::string& utf8name, bool boolValue) {
  HandleScope scope(_env);
  Value().Set(utf8name, boolValue);
}

inline void ObjectReference::Set(const std::string& utf8name, double numberValue) {
  HandleScope scope(_env);
  Value().Set(utf8name, numberValue);
}

inline Napi::Value ObjectReference::Get(uint32_t index) const {
  //EscapableHandleScope scope(_env);
  //return scope.Escape(Value().Get(index));
  throw std::runtime_error{"TODO"};
}

inline void ObjectReference::Set(uint32_t index, napi_value value) {
  HandleScope scope(_env);
  Value().Set(index, value);
}

inline void ObjectReference::Set(uint32_t index, Napi::Value value) {
  HandleScope scope(_env);
  Value().Set(index, value);
}

inline void ObjectReference::Set(uint32_t index, const char* utf8value) {
  HandleScope scope(_env);
  Value().Set(index, utf8value);
}

inline void ObjectReference::Set(uint32_t index, const std::string& utf8value) {
  HandleScope scope(_env);
  Value().Set(index, utf8value);
}

inline void ObjectReference::Set(uint32_t index, bool boolValue) {
  HandleScope scope(_env);
  Value().Set(index, boolValue);
}

inline void ObjectReference::Set(uint32_t index, double numberValue) {
  HandleScope scope(_env);
  Value().Set(index, numberValue);
}

////////////////////////////////////////////////////////////////////////////////
// FunctionReference class
////////////////////////////////////////////////////////////////////////////////

inline FunctionReference::FunctionReference(): Reference<Function>() {
}

inline FunctionReference::FunctionReference(napi_env env, napi_ref ref)
  : Reference<Function>(env, ref) {
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
    const std::initializer_list<napi_value>& args) const {
  //EscapableHandleScope scope(_env);
  //return scope.Escape(Value()(args));
  throw std::runtime_error{"TODO"};
}

inline Napi::Value FunctionReference::Call(const std::initializer_list<napi_value>& args) const {
  //EscapableHandleScope scope(_env);
  //Napi::Value result = Value().Call(args);
  //if (scope.Env().IsExceptionPending()) {
  //  return Value();
  //}
  //return scope.Escape(result);
  //throw std::runtime_error{"TODO"};
  return Value().Call(args);
}

inline Napi::Value FunctionReference::Call(const std::vector<napi_value>& args) const {
  //EscapableHandleScope scope(_env);
  //Napi::Value result = Value().Call(args);
  //if (scope.Env().IsExceptionPending()) {
  //  return Value();
  //}
  //return scope.Escape(result);
  throw std::runtime_error{"TODO"};
}

inline Napi::Value FunctionReference::Call(
    napi_value recv, const std::initializer_list<napi_value>& args) const {
  //EscapableHandleScope scope(_env);
  //Napi::Value result = Value().Call(recv, args);
  //if (scope.Env().IsExceptionPending()) {
  //  return Value();
  //}
  //return scope.Escape(result);
  throw std::runtime_error{"TODO"};
}

inline Napi::Value FunctionReference::Call(
    napi_value recv, const std::vector<napi_value>& args) const {
  //EscapableHandleScope scope(_env);
  //Napi::Value result = Value().Call(recv, args);
  //if (scope.Env().IsExceptionPending()) {
  //  return Value();
  //}
  //return scope.Escape(result);
  throw std::runtime_error{"TODO"};
}

inline Napi::Value FunctionReference::Call(
    napi_value recv, size_t argc, const napi_value* args) const {
  //EscapableHandleScope scope(_env);
  //Napi::Value result = Value().Call(recv, argc, args);
  //if (scope.Env().IsExceptionPending()) {
  //  return Value();
  //}
  //return scope.Escape(result);
  throw std::runtime_error{"TODO"};
}

inline Object FunctionReference::New(const std::initializer_list<napi_value>& args) const {
  //EscapableHandleScope scope(_env);
  //return scope.Escape(Value().New(args)).As<Object>();
  throw std::runtime_error{"TODO"};
}

inline Object FunctionReference::New(const std::vector<napi_value>& args) const {
  //EscapableHandleScope scope(_env);
  //return scope.Escape(Value().New(args)).As<Object>();
  throw std::runtime_error{"TODO"};
}

////////////////////////////////////////////////////////////////////////////////
// CallbackInfo class
////////////////////////////////////////////////////////////////////////////////

inline CallbackInfo::CallbackInfo(napi_env env, const facebook::jsi::Value& thisVal, const facebook::jsi::Value* args, size_t argc, const facebook::jsi::Value& newTarget, void* data)
    : _env{env}, _this{_env, thisVal}, _argc{argc}, _dynamicArgs{nullptr}, _newTarget{_env, newTarget}, _data{data} {
  if (_argc <= std::size(_staticArgs)) {
    _argv = _staticArgs;
  } else {
    _dynamicArgs = new napi_value[_argc];
    _argv = _dynamicArgs;
  }

  for (int i = 0; i < _argc; ++i) {
    _argv[i] = {_env, args[i]};
  }
}

inline CallbackInfo::~CallbackInfo() {
  if (_dynamicArgs != nullptr) {
    delete[] _dynamicArgs;
  }
}

inline Value CallbackInfo::NewTarget() const {
  return {_env, _newTarget};
}

inline bool CallbackInfo::IsConstructCall() const {
  return !NewTarget().IsEmpty();
}

inline Napi::Env CallbackInfo::Env() const {
  return {_env};
}

inline size_t CallbackInfo::Length() const {
  return _argc;
}

inline const Value CallbackInfo::operator [](size_t index) const {
  return index < _argc ? Value{_env, _argv[index]} : Env().Undefined();
}

inline Value CallbackInfo::This() const {
  return {_env, _this};
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

template <typename Getter>
inline PropertyDescriptor
PropertyDescriptor::Accessor(Napi::Env env,
                             Napi::Object object,
                             const char* utf8name,
                             Getter getter,
                             napi_property_attributes attributes,
                             void* data) {
  //typedef details::CallbackData<Getter, Napi::Value> CbData;
  //auto callbackData = new CbData({ getter, data });

  //napi_status status = AttachData(env, object, callbackData);
  //if (status != napi_ok) {
  //  delete callbackData;
  //  NAPI_THROW_IF_FAILED(env, status, napi_property_descriptor());
  //}

  //return PropertyDescriptor({
  //  utf8name,
  //  nullptr,
  //  nullptr,
  //  CbData::Wrapper,
  //  nullptr,
  //  nullptr,
  //  attributes,
  //  callbackData
  //});
  throw std::runtime_error{"TODO"};
}

template <typename Getter>
inline PropertyDescriptor PropertyDescriptor::Accessor(Napi::Env env,
                                                       Napi::Object object,
                                                       const std::string& utf8name,
                                                       Getter getter,
                                                       napi_property_attributes attributes,
                                                       void* data) {
  return Accessor(env, object, utf8name.c_str(), getter, attributes, data);
}

template <typename Getter>
inline PropertyDescriptor PropertyDescriptor::Accessor(Napi::Env env,
                                                       Napi::Object object,
                                                       Name name,
                                                       Getter getter,
                                                       napi_property_attributes attributes,
                                                       void* data) {
  //typedef details::CallbackData<Getter, Napi::Value> CbData;
  //auto callbackData = new CbData({ getter, data });

  //napi_status status = AttachData(env, object, callbackData);
  //if (status != napi_ok) {
  //  delete callbackData;
  //  NAPI_THROW_IF_FAILED(env, status, napi_property_descriptor());
  //}

  //return PropertyDescriptor({
  //  nullptr,
  //  name,
  //  nullptr,
  //  CbData::Wrapper,
  //  nullptr,
  //  nullptr,
  //  attributes,
  //  callbackData
  //});
  throw std::runtime_error{"TODO"};
}

template <typename Getter, typename Setter>
inline PropertyDescriptor PropertyDescriptor::Accessor(Napi::Env env,
                                                       Napi::Object object,
                                                       const char* utf8name,
                                                       Getter getter,
                                                       Setter setter,
                                                       napi_property_attributes attributes,
                                                       void* data) {
  //typedef details::AccessorCallbackData<Getter, Setter> CbData;
  //auto callbackData = new CbData({ getter, setter, data });

  //napi_status status = AttachData(env, object, callbackData);
  //if (status != napi_ok) {
  //  delete callbackData;
  //  NAPI_THROW_IF_FAILED(env, status, napi_property_descriptor());
  //}

  //return PropertyDescriptor({
  //  utf8name,
  //  nullptr,
  //  nullptr,
  //  CbData::GetterWrapper,
  //  CbData::SetterWrapper,
  //  nullptr,
  //  attributes,
  //  callbackData
  //});
  throw std::runtime_error{"TODO"};
}

template <typename Getter, typename Setter>
inline PropertyDescriptor PropertyDescriptor::Accessor(Napi::Env env,
                                                       Napi::Object object,
                                                       const std::string& utf8name,
                                                       Getter getter,
                                                       Setter setter,
                                                       napi_property_attributes attributes,
                                                       void* data) {
  return Accessor(env, object, utf8name.c_str(), getter, setter, attributes, data);
}

template <typename Getter, typename Setter>
inline PropertyDescriptor PropertyDescriptor::Accessor(Napi::Env env,
                                                       Napi::Object object,
                                                       Name name,
                                                       Getter getter,
                                                       Setter setter,
                                                       napi_property_attributes attributes,
                                                       void* data) {
  //typedef details::AccessorCallbackData<Getter, Setter> CbData;
  //auto callbackData = new CbData({ getter, setter, data });

  //napi_status status = AttachData(env, object, callbackData);
  //if (status != napi_ok) {
  //  delete callbackData;
  //  NAPI_THROW_IF_FAILED(env, status, napi_property_descriptor());
  //}

  //return PropertyDescriptor({
  //  nullptr,
  //  name,
  //  nullptr,
  //  CbData::GetterWrapper,
  //  CbData::SetterWrapper,
  //  nullptr,
  //  attributes,
  //  callbackData
  //});
  throw std::runtime_error{"TODO"};
}

template <typename Callable>
inline PropertyDescriptor PropertyDescriptor::Function(Napi::Env env,
                                                       Napi::Object /*object*/,
                                                       const char* utf8name,
                                                       Callable cb,
                                                       napi_property_attributes attributes,
                                                       void* data) {
  return PropertyDescriptor({
    utf8name,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    Napi::Function::New(env, cb, utf8name, data),
    attributes,
    nullptr
  });
}

template <typename Callable>
inline PropertyDescriptor PropertyDescriptor::Function(Napi::Env env,
                                                       Napi::Object object,
                                                       const std::string& utf8name,
                                                       Callable cb,
                                                       napi_property_attributes attributes,
                                                       void* data) {
  return Function(env, object, utf8name.c_str(), cb, attributes, data);
}

template <typename Callable>
inline PropertyDescriptor PropertyDescriptor::Function(Napi::Env env,
                                                       Napi::Object /*object*/,
                                                       Name name,
                                                       Callable cb,
                                                       napi_property_attributes attributes,
                                                       void* data) {
  return PropertyDescriptor({
    nullptr,
    name,
    nullptr,
    nullptr,
    nullptr,
    Napi::Function::New(env, cb, nullptr, data),
    attributes,
    nullptr
  });
}

inline PropertyDescriptor PropertyDescriptor::Value(const char* utf8name,
                                                    napi_value value,
                                                    napi_property_attributes attributes) {
  //return PropertyDescriptor({
  //  utf8name, {}, nullptr, nullptr, nullptr, value, attributes, nullptr
  //});
  throw std::runtime_error{"TODO"};
}

inline PropertyDescriptor PropertyDescriptor::Value(const std::string& utf8name,
                                                    napi_value value,
                                                    napi_property_attributes attributes) {
  return Value(utf8name.c_str(), value, attributes);
}

inline PropertyDescriptor PropertyDescriptor::Value(napi_value name,
                                                    napi_value value,
                                                    napi_property_attributes attributes) {
  //return PropertyDescriptor({
  //  nullptr, name, nullptr, nullptr, nullptr, value, attributes, nullptr
  //});
  throw std::runtime_error{"TODO"};
}

inline PropertyDescriptor PropertyDescriptor::Value(Name name,
                                                    Napi::Value value,
                                                    napi_property_attributes attributes) {
  napi_value nameValue = name;
  napi_value valueValue = value;
  return PropertyDescriptor::Value(nameValue, valueValue, attributes);
}

inline PropertyDescriptor::PropertyDescriptor(napi_property_descriptor desc)
  /*: _desc(desc)*/ {
  throw std::runtime_error{"TODO"};
}

inline PropertyDescriptor::operator napi_property_descriptor&() {
  return _desc;
}

inline PropertyDescriptor::operator const napi_property_descriptor&() const {
  return _desc;
}

////////////////////////////////////////////////////////////////////////////////
// ObjectWrap<T> class
////////////////////////////////////////////////////////////////////////////////

template <typename T>
inline ObjectWrap<T>::ObjectWrap(const Napi::CallbackInfo& callbackInfo)
  : Reference<Object>(callbackInfo.Env(), {}) {
  //napi_env env = callbackInfo.Env();
  //napi_value wrapper = callbackInfo.This();
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
inline T* ObjectWrap<T>::Unwrap(Object wrapper) {
  facebook::jsi::Runtime& rt{*static_cast<napi_env>(wrapper.Env())};
  auto native{napi_value{wrapper}->getObject(rt).getProperty(rt, "__native__")};
  return native.isUndefined() ? nullptr : native.getObject(rt).getHostObject<T>(rt).get();
}

template <typename T>
inline Function
ObjectWrap<T>::DefineClass(Napi::Env env,
                           const char* utf8name,
                           const size_t props_count,
                           const napi_property_descriptor* descriptors,
                           void* data) {
  jsi::Runtime& rt{*static_cast<napi_env>(env)};

  auto newTarget{std::make_shared<jsi::Value>()};

  // TODO: Cache this somewhere?
  auto constructorFactoryCode = std::make_shared<const jsi::StringBuffer>("(function(constructor) { return function() { return constructor.apply(this, arguments); }; })");
  auto createConstructor = rt.evaluateJavaScript(std::move(constructorFactoryCode), "").asObject(rt).asFunction(rt);

  jsi::Function constructor{createConstructor.call(rt,
    jsi::Function::createFromHostFunction(rt, jsi::PropNameID::forUtf8(rt, utf8name), 0,
      [newTarget, data](jsi::Runtime& rt, const jsi::Value& thisVal, const jsi::Value* args, size_t count) -> jsi::Value {
        CallbackInfo callbackInfo{&rt, thisVal, args, count, *newTarget, data};
        // TODO: use prototype to wrap object? or return new host object?
        thisVal.getObject(rt).setProperty(rt, "__native__", jsi::Object::createFromHostObject(rt, std::make_shared<T>(callbackInfo)));
        return {};
      })).getObject(rt).getFunction(rt)};

  *newTarget = {rt, constructor};

  jsi::Object prototype{constructor.getProperty(rt, "prototype").getObject(rt)};

  for (int i = 0; i < props_count; ++i) {
    const napi_property_descriptor& p{descriptors[i]};
    jsi::PropNameID name{jsi::PropNameID::forUtf8(rt, p.utf8name)};
    if (p.attributes & napi_static) {
      //throw std::runtime_error{"TODO"};
    } else {
      jsi::Object descriptor{rt};
      descriptor.setProperty(rt, "configurable", jsi::Value{(p.attributes & napi_configurable) != 0});
      descriptor.setProperty(rt, "enumerable", jsi::Value{(p.attributes & napi_enumerable) != 0});
      if (p.getter != nullptr || p.setter != nullptr) {
        if (p.getter != nullptr) {
          descriptor.setProperty(rt, "get", jsi::Function::createFromHostFunction(rt, name, 0, p.getter));
        }
        if (p.setter != nullptr) {
          descriptor.setProperty(rt, "set", jsi::Function::createFromHostFunction(rt, name, 0, p.setter));
        }
      } else if (p.method != nullptr) {
        descriptor.setProperty(rt, "value", jsi::Function::createFromHostFunction(rt, name, 0, p.method));
      } else {
        descriptor.setProperty(rt, "writable", jsi::Value{(p.attributes & napi_writable) != 0});
        descriptor.setProperty(rt, "value", *p.value);
      }

      jsi::Object object{rt.global().getPropertyAsObject(rt, "Object")};
      jsi::Function defineProperty{object.getPropertyAsFunction(rt, "defineProperty")};
      defineProperty.callWithThis(rt, object, prototype, p.utf8name, descriptor);
    }
  }

  return {&rt, {&rt, {rt, constructor}}};
}

template <typename T>
inline Function ObjectWrap<T>::DefineClass(
    Napi::Env env,
    const char* utf8name,
    const std::initializer_list<ClassPropertyDescriptor<T>>& properties,
    void* data) {
  return DefineClass(env,
          utf8name,
          properties.size(),
          reinterpret_cast<const napi_property_descriptor*>(properties.begin()),
          data);
}

template <typename T>
inline Function ObjectWrap<T>::DefineClass(
    Napi::Env env,
    const char* utf8name,
    const std::vector<ClassPropertyDescriptor<T>>& properties,
    void* data) {
  return DefineClass(env,
           utf8name,
           properties.size(),
           reinterpret_cast<const napi_property_descriptor*>(properties.data()),
           data);
}

template <typename T>
inline ClassPropertyDescriptor<T> ObjectWrap<T>::StaticMethod(
    const char* utf8name,
    StaticVoidMethodCallback method,
    napi_property_attributes attributes,
    void* data) {
  //StaticVoidMethodCallbackData* callbackData = new StaticVoidMethodCallbackData({ method, data });

  //napi_property_descriptor desc = napi_property_descriptor();
  //desc.utf8name = utf8name;
  //desc.method = T::StaticVoidMethodCallbackWrapper;
  //desc.data = callbackData;
  //desc.attributes = static_cast<napi_property_attributes>(attributes | napi_static);
  //return desc;
  throw std::runtime_error{"TODO"};
}

template <typename T>
inline ClassPropertyDescriptor<T> ObjectWrap<T>::StaticMethod(
    const char* utf8name,
    StaticMethodCallback method,
    napi_property_attributes attributes,
    void* data) {
  //StaticMethodCallbackData* callbackData = new StaticMethodCallbackData({ method, data });

  //napi_property_descriptor desc = napi_property_descriptor();
  //desc.utf8name = utf8name;
  //desc.method = T::StaticMethodCallbackWrapper;
  //desc.data = callbackData;
  //desc.attributes = static_cast<napi_property_attributes>(attributes | napi_static);
  //return desc;
  throw std::runtime_error{"TODO"};
}

template <typename T>
inline ClassPropertyDescriptor<T> ObjectWrap<T>::StaticMethod(
    Symbol name,
    StaticVoidMethodCallback method,
    napi_property_attributes attributes,
    void* data) {
  //StaticVoidMethodCallbackData* callbackData = new StaticVoidMethodCallbackData({ method, data });

  //napi_property_descriptor desc = napi_property_descriptor();
  //desc.name = name;
  //desc.method = T::StaticVoidMethodCallbackWrapper;
  //desc.data = callbackData;
  //desc.attributes = static_cast<napi_property_attributes>(attributes | napi_static);
  //return desc;
  throw std::runtime_error{"TODO"};
}

template <typename T>
inline ClassPropertyDescriptor<T> ObjectWrap<T>::StaticMethod(
    Symbol name,
    StaticMethodCallback method,
    napi_property_attributes attributes,
    void* data) {
  //StaticMethodCallbackData* callbackData = new StaticMethodCallbackData({ method, data });

  //napi_property_descriptor desc = napi_property_descriptor();
  //desc.name = name;
  //desc.method = T::StaticMethodCallbackWrapper;
  //desc.data = callbackData;
  //desc.attributes = static_cast<napi_property_attributes>(attributes | napi_static);
  //return desc;
  throw std::runtime_error{"TODO"};
}

template <typename T>
inline ClassPropertyDescriptor<T> ObjectWrap<T>::StaticAccessor(
    const char* utf8name,
    StaticGetterCallback getter,
    StaticSetterCallback setter,
    napi_property_attributes attributes,
    void* data) {
  //StaticAccessorCallbackData* callbackData =
  //  new StaticAccessorCallbackData({ getter, setter, data });

  //napi_property_descriptor desc = napi_property_descriptor();
  //desc.utf8name = utf8name;
  //desc.getter = getter != nullptr ? T::StaticGetterCallbackWrapper : nullptr;
  //desc.setter = setter != nullptr ? T::StaticSetterCallbackWrapper : nullptr;
  //desc.data = callbackData;
  //desc.attributes = static_cast<napi_property_attributes>(attributes | napi_static);
  //return desc;
  throw std::runtime_error{"TODO"};
}

template <typename T>
inline ClassPropertyDescriptor<T> ObjectWrap<T>::StaticAccessor(
    Symbol name,
    StaticGetterCallback getter,
    StaticSetterCallback setter,
    napi_property_attributes attributes,
    void* data) {
  //StaticAccessorCallbackData* callbackData =
  //  new StaticAccessorCallbackData({ getter, setter, data });

  //napi_property_descriptor desc = napi_property_descriptor();
  //desc.name = name;
  //desc.getter = getter != nullptr ? T::StaticGetterCallbackWrapper : nullptr;
  //desc.setter = setter != nullptr ? T::StaticSetterCallbackWrapper : nullptr;
  //desc.data = callbackData;
  //desc.attributes = static_cast<napi_property_attributes>(attributes | napi_static);
  //return desc;
  throw std::runtime_error{"TODO"};
}

template <typename T>
inline ClassPropertyDescriptor<T> ObjectWrap<T>::InstanceMethod(
    const char* utf8name,
    InstanceVoidMethodCallback method,
    napi_property_attributes attributes,
    void* data) {
  napi_property_descriptor desc{};
  desc.utf8name = utf8name;
  desc.method =
    [method, data](jsi::Runtime& rt, const jsi::Value& thisVal, const jsi::Value* args, size_t count) -> jsi::Value {
      auto instance{Unwrap({&rt, {&rt, thisVal.getObject(rt)}})};
      CallbackInfo callbackInfo{&rt, thisVal, args, count, nullptr, data};
      (instance->*method)(callbackInfo);
      return {};
    };
  desc.attributes = attributes;
  return desc;
}

template <typename T>
inline ClassPropertyDescriptor<T> ObjectWrap<T>::InstanceMethod(
    const char* utf8name,
    InstanceMethodCallback method,
    napi_property_attributes attributes,
    void* data) {
  napi_property_descriptor desc{};
  desc.utf8name = utf8name;
  desc.method =
    [method, data](jsi::Runtime& rt, const jsi::Value& thisVal, const jsi::Value* args, size_t count) -> jsi::Value {
      auto instance{Unwrap({&rt, {&rt, thisVal.getObject(rt)}})};
      CallbackInfo callbackInfo{&rt, thisVal, args, count, nullptr, data};
      return {*napi_value{(instance->*method)(callbackInfo)}};
    };
  desc.attributes = attributes;
  return desc;
}

template <typename T>
inline ClassPropertyDescriptor<T> ObjectWrap<T>::InstanceMethod(
    Symbol name,
    InstanceVoidMethodCallback method,
    napi_property_attributes attributes,
    void* data) {
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
inline ClassPropertyDescriptor<T> ObjectWrap<T>::InstanceMethod(
    Symbol name,
    InstanceMethodCallback method,
    napi_property_attributes attributes,
    void* data) {
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
inline ClassPropertyDescriptor<T> ObjectWrap<T>::InstanceAccessor(
    const char* utf8name,
    InstanceGetterCallback getter,
    InstanceSetterCallback setter,
    napi_property_attributes attributes,
    void* data) {
  napi_property_descriptor desc{};
  desc.utf8name = utf8name;
  if (getter != nullptr) {
    desc.getter =
      [getter, data](jsi::Runtime& rt, const jsi::Value& thisVal, const jsi::Value* args, size_t count) -> jsi::Value {
        auto instance{Unwrap({&rt, {&rt, thisVal.getObject(rt)}})};
        CallbackInfo callbackInfo{&rt, thisVal, args, count, nullptr, data};
        return {*napi_value{(instance->*getter)(callbackInfo)}};
      };
  }
  if (setter != nullptr) {
    desc.setter =
      [setter, data](jsi::Runtime& rt, const jsi::Value& thisVal, const jsi::Value* args, size_t count) -> jsi::Value {
        auto instance{Unwrap({&rt, {&rt, thisVal.getObject(rt)}})};
        CallbackInfo callbackInfo{&rt, thisVal, args, count, nullptr, data};
        (instance->*setter)(callbackInfo, {&rt, {&rt, args[0]}});
        return {};
      };
  }
  desc.attributes = attributes;
  return desc;
}

template <typename T>
inline ClassPropertyDescriptor<T> ObjectWrap<T>::InstanceAccessor(
    Symbol name,
    InstanceGetterCallback getter,
    InstanceSetterCallback setter,
    napi_property_attributes attributes,
    void* data) {
  //InstanceAccessorCallbackData* callbackData =
  //  new InstanceAccessorCallbackData({ getter, setter, data });

  //napi_property_descriptor desc = napi_property_descriptor();
  //desc.name = name;
  //desc.getter = getter != nullptr ? T::InstanceGetterCallbackWrapper : nullptr;
  //desc.setter = setter != nullptr ? T::InstanceSetterCallbackWrapper : nullptr;
  //desc.data = callbackData;
  //desc.attributes = attributes;
  //return desc;
  throw std::runtime_error{"TODO"};
}

template <typename T>
inline ClassPropertyDescriptor<T> ObjectWrap<T>::StaticValue(const char* utf8name,
    Napi::Value value, napi_property_attributes attributes) {
  napi_property_descriptor desc{};
  desc.utf8name = utf8name;
  desc.value = value;
  desc.attributes = static_cast<napi_property_attributes>(attributes | napi_static);
  return desc;
}

template <typename T>
inline ClassPropertyDescriptor<T> ObjectWrap<T>::StaticValue(Symbol name,
    Napi::Value value, napi_property_attributes attributes) {
  //napi_property_descriptor desc = napi_property_descriptor();
  //desc.name = name;
  //desc.value = value;
  //desc.attributes = static_cast<napi_property_attributes>(attributes | napi_static);
  //return desc;
  throw std::runtime_error{"TODO"};
}

template <typename T>
inline ClassPropertyDescriptor<T> ObjectWrap<T>::InstanceValue(
    const char* utf8name,
    Napi::Value value,
    napi_property_attributes attributes) {
  napi_property_descriptor desc = napi_property_descriptor();
  desc.utf8name = utf8name;
  desc.value = value;
  desc.attributes = attributes;
  return desc;
}

template <typename T>
inline ClassPropertyDescriptor<T> ObjectWrap<T>::InstanceValue(
    Symbol name,
    Napi::Value value,
    napi_property_attributes attributes) {
  //napi_property_descriptor desc = napi_property_descriptor();
  //desc.name = name;
  //desc.value = value;
  //desc.attributes = attributes;
  //return desc;
  throw std::runtime_error{"TODO"};
}

template <typename T>
inline void ObjectWrap<T>::Finalize(Napi::Env /*env*/) {}

//template <typename T>
//inline napi_value ObjectWrap<T>::ConstructorCallbackWrapper(
//    napi_env env,
//    napi_callback_info info) {
//  napi_value new_target;
//  napi_status status = napi_get_new_target(env, info, &new_target);
//  if (status != napi_ok) return nullptr;
//
//  bool isConstructCall = (new_target != nullptr);
//  if (!isConstructCall) {
//    napi_throw_type_error(env, nullptr, "Class constructors cannot be invoked without 'new'");
//    return nullptr;
//  }
//
//  T* instance;
//  napi_value wrapper = details::WrapCallback([&] {
//    CallbackInfo callbackInfo(env, info);
//    instance = new T(callbackInfo);
//    return callbackInfo.This();
//  });
//
//  return wrapper;
//}
//
//template <typename T>
//inline napi_value ObjectWrap<T>::StaticVoidMethodCallbackWrapper(
//    napi_env env,
//    napi_callback_info info) {
//  return details::WrapCallback([&] {
//    CallbackInfo callbackInfo(env, info);
//    StaticVoidMethodCallbackData* callbackData =
//      reinterpret_cast<StaticVoidMethodCallbackData*>(callbackInfo.Data());
//    callbackInfo.SetData(callbackData->data);
//    callbackData->callback(callbackInfo);
//    return nullptr;
//  });
//}
//
//template <typename T>
//inline napi_value ObjectWrap<T>::StaticMethodCallbackWrapper(
//    napi_env env,
//    napi_callback_info info) {
//  return details::WrapCallback([&] {
//    CallbackInfo callbackInfo(env, info);
//    StaticMethodCallbackData* callbackData =
//      reinterpret_cast<StaticMethodCallbackData*>(callbackInfo.Data());
//    callbackInfo.SetData(callbackData->data);
//    return callbackData->callback(callbackInfo);
//  });
//}
//
//template <typename T>
//inline napi_value ObjectWrap<T>::StaticGetterCallbackWrapper(
//    napi_env env,
//    napi_callback_info info) {
//  return details::WrapCallback([&] {
//    CallbackInfo callbackInfo(env, info);
//    StaticAccessorCallbackData* callbackData =
//      reinterpret_cast<StaticAccessorCallbackData*>(callbackInfo.Data());
//    callbackInfo.SetData(callbackData->data);
//    return callbackData->getterCallback(callbackInfo);
//  });
//}
//
//template <typename T>
//inline napi_value ObjectWrap<T>::StaticSetterCallbackWrapper(
//    napi_env env,
//    napi_callback_info info) {
//  return details::WrapCallback([&] {
//    CallbackInfo callbackInfo(env, info);
//    StaticAccessorCallbackData* callbackData =
//      reinterpret_cast<StaticAccessorCallbackData*>(callbackInfo.Data());
//    callbackInfo.SetData(callbackData->data);
//    callbackData->setterCallback(callbackInfo, callbackInfo[0]);
//    return nullptr;
//  });
//}
//
//template <typename T>
//inline napi_value ObjectWrap<T>::InstanceVoidMethodCallbackWrapper(
//    napi_env env,
//    napi_callback_info info) {
//  return details::WrapCallback([&] {
//    CallbackInfo callbackInfo(env, info);
//    InstanceVoidMethodCallbackData* callbackData =
//      reinterpret_cast<InstanceVoidMethodCallbackData*>(callbackInfo.Data());
//    callbackInfo.SetData(callbackData->data);
//    T* instance = Unwrap(callbackInfo.This().As<Object>());
//    auto cb = callbackData->callback;
//    (instance->*cb)(callbackInfo);
//    return nullptr;
//  });
//}
//
//template <typename T>
//inline napi_value ObjectWrap<T>::InstanceMethodCallbackWrapper(
//    napi_env env,
//    napi_callback_info info) {
//  return details::WrapCallback([&] {
//    CallbackInfo callbackInfo(env, info);
//    InstanceMethodCallbackData* callbackData =
//      reinterpret_cast<InstanceMethodCallbackData*>(callbackInfo.Data());
//    callbackInfo.SetData(callbackData->data);
//    T* instance = Unwrap(callbackInfo.This().As<Object>());
//    auto cb = callbackData->callback;
//    return (instance->*cb)(callbackInfo);
//  });
//}
//
//template <typename T>
//inline napi_value ObjectWrap<T>::InstanceGetterCallbackWrapper(
//    napi_env env,
//    napi_callback_info info) {
//  return details::WrapCallback([&] {
//    CallbackInfo callbackInfo(env, info);
//    InstanceAccessorCallbackData* callbackData =
//      reinterpret_cast<InstanceAccessorCallbackData*>(callbackInfo.Data());
//    callbackInfo.SetData(callbackData->data);
//    T* instance = Unwrap(callbackInfo.This().As<Object>());
//    auto cb = callbackData->getterCallback;
//    return (instance->*cb)(callbackInfo);
//  });
//}
//
//template <typename T>
//inline napi_value ObjectWrap<T>::InstanceSetterCallbackWrapper(
//    napi_env env,
//    napi_callback_info info) {
//  return details::WrapCallback([&] {
//    CallbackInfo callbackInfo(env, info);
//    InstanceAccessorCallbackData* callbackData =
//      reinterpret_cast<InstanceAccessorCallbackData*>(callbackInfo.Data());
//    callbackInfo.SetData(callbackData->data);
//    T* instance = Unwrap(callbackInfo.This().As<Object>());
//    auto cb = callbackData->setterCallback;
//    (instance->*cb)(callbackInfo, callbackInfo[0]);
//    return nullptr;
//  });
//}
//
//template <typename T>
//inline void ObjectWrap<T>::FinalizeCallback(napi_env env, void* data, void* /*hint*/) {
//  T* instance = reinterpret_cast<T*>(data);
//  instance->Finalize(Napi::Env(env));
//  delete instance;
//}

////////////////////////////////////////////////////////////////////////////////
// HandleScope class
////////////////////////////////////////////////////////////////////////////////

inline HandleScope::HandleScope(napi_env env, napi_handle_scope scope)
    : _env(env), _scope(scope) {
}

inline HandleScope::HandleScope(Napi::Env env) : _env(env) {
  //napi_status status = napi_open_handle_scope(_env, &_scope);
  //NAPI_THROW_IF_FAILED_VOID(_env, status);
  //throw std::runtime_error{"TODO"};
}

inline HandleScope::~HandleScope() {
  //napi_close_handle_scope(_env, _scope);
  //throw std::runtime_error{"TODO"};
}

inline HandleScope::operator napi_handle_scope() const {
  return _scope;
}

inline Napi::Env HandleScope::Env() const {
  return Napi::Env(_env);
}

////////////////////////////////////////////////////////////////////////////////
// EscapableHandleScope class
////////////////////////////////////////////////////////////////////////////////

inline EscapableHandleScope::EscapableHandleScope(
  napi_env env, napi_escapable_handle_scope scope) : _env(env), _scope(scope) {
}

inline EscapableHandleScope::EscapableHandleScope(Napi::Env env) : _env(env) {
  //napi_status status = napi_open_escapable_handle_scope(_env, &_scope);
  //NAPI_THROW_IF_FAILED_VOID(_env, status);
  throw std::runtime_error{"TODO"};
}

inline EscapableHandleScope::~EscapableHandleScope() {
  //napi_close_escapable_handle_scope(_env, _scope);
  //throw std::runtime_error{"TODO"};
}

inline EscapableHandleScope::operator napi_escapable_handle_scope() const {
  return _scope;
}

inline Napi::Env EscapableHandleScope::Env() const {
  return Napi::Env(_env);
}

inline Value EscapableHandleScope::Escape(napi_value escapee) {
  //napi_value result;
  //napi_status status = napi_escape_handle(_env, _scope, escapee, &result);
  //NAPI_THROW_IF_FAILED(_env, status, Value());
  //return Value(_env, result);
  throw std::runtime_error{"TODO"};
}

} // namespace Napi
