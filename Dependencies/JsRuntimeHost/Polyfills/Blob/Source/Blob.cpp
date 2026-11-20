#include "Blob.h"
#include <Babylon/JsRuntime.h>
#include <Babylon/Polyfills/Blob.h>
#include <Babylon/Polyfills/BlobInternal.h>

#include <algorithm>
#include <cmath>
#include <cstring>

namespace
{
    constexpr auto JS_BLOB_CONSTRUCTOR_NAME = "Blob";
    // Hidden global holding the polyfill's own constructor (see Babylon::Polyfills::Blob::TryGetData).
    constexpr auto JS_BLOB_CONSTRUCTOR_KEY = "__jsRuntimeHostBlob";
}

namespace Babylon::Polyfills::Internal
{
    struct Blob::Segment
    {
        std::shared_ptr<const std::vector<std::byte>> Bytes;
        size_t Offset{};
        size_t Length{};
    };

    struct Blob::Storage
    {
        std::vector<Segment> Segments;
        size_t Size{};

        void Append(const Segment& segment)
        {
            if (segment.Length == 0)
            {
                return;
            }

            Segments.emplace_back(segment);
            Size += segment.Length;
        }

        void CopyTo(size_t position, std::byte* destination, size_t length) const
        {
            size_t segmentStart{};
            for (const auto& segment : Segments)
            {
                const auto segmentEnd = segmentStart + segment.Length;
                if (position < segmentEnd && length > 0)
                {
                    const auto localOffset = position > segmentStart ? position - segmentStart : 0;
                    const auto copyLength = std::min(segment.Length - localOffset, length);
                    std::memcpy(destination, segment.Bytes->data() + segment.Offset + localOffset, copyLength);
                    destination += copyLength;
                    position += copyLength;
                    length -= copyLength;
                }
                segmentStart = segmentEnd;
            }
        }
    };

    class Blob::StreamSource final : public Napi::ObjectWrap<Blob::StreamSource>
    {
    public:
        static Napi::Function Define(Napi::Env env)
        {
            return DefineClass(
                env,
                "BlobStreamSource",
                {
                    InstanceMethod("pull", &StreamSource::Pull),
                    InstanceMethod("cancel", &StreamSource::Cancel),
                });
        }

        explicit StreamSource(const Napi::CallbackInfo& info)
            : Napi::ObjectWrap<StreamSource>{info}
        {
            if (info.Length() != 1 || !info[0].IsObject())
            {
                throw Napi::TypeError::New(info.Env(), "Blob stream source requires a Blob.");
            }

            const auto blob = Napi::ObjectWrap<Blob>::Unwrap(info[0].As<Napi::Object>());
            if (blob == nullptr)
            {
                throw Napi::TypeError::New(info.Env(), "Blob stream source requires a Blob.");
            }
            m_blobData = blob->m_data;
        }

    private:
        Napi::Value Pull(const Napi::CallbackInfo& info)
        {
            auto env = info.Env();
            auto controller = info[0].As<Napi::Object>();
            if (!m_blobData || m_position >= m_blobData->Size)
            {
                controller.Get("close").As<Napi::Function>().Call(controller, {});
                m_blobData.reset();
                return env.Undefined();
            }

            constexpr size_t CHUNK_SIZE = 64 * 1024;
            const auto copyInto = [this](std::byte* destination, size_t outputLength) {
                auto segmentIndex = m_segmentIndex;
                auto segmentOffset = m_segmentOffset;
                size_t remaining = outputLength;

                while (remaining > 0)
                {
                    const auto& segment = m_blobData->Segments[segmentIndex];
                    const auto copyLength = std::min(segment.Length - segmentOffset, remaining);
                    std::memcpy(destination, segment.Bytes->data() + segment.Offset + segmentOffset, copyLength);
                    destination += copyLength;
                    remaining -= copyLength;
                    segmentOffset += copyLength;
                    if (segmentOffset == segment.Length)
                    {
                        ++segmentIndex;
                        segmentOffset = 0;
                    }
                }

                m_position += outputLength;
                m_segmentIndex = segmentIndex;
                m_segmentOffset = segmentOffset;
            };

            const auto byobRequestValue = controller.Get("byobRequest");
            if (!byobRequestValue.IsUndefined() && !byobRequestValue.IsNull())
            {
                const auto byobRequest = byobRequestValue.As<Napi::Object>();
                const auto viewValue = byobRequest.Get("view");
                if (!viewValue.IsTypedArray())
                {
                    throw Napi::TypeError::New(env, "Blob.stream() received an invalid BYOB request view.");
                }

                const auto view = viewValue.As<Napi::TypedArray>();
                const auto outputLength = std::min({CHUNK_SIZE, view.ByteLength(), m_blobData->Size - m_position});
                const auto outputBuffer = view.ArrayBuffer();
                auto destination = static_cast<std::byte*>(outputBuffer.Data()) + view.ByteOffset();
                copyInto(destination, outputLength);
                byobRequest.Get("respond").As<Napi::Function>().Call(byobRequest, {Napi::Number::New(env, static_cast<double>(outputLength))});
            }
            else
            {
                const auto outputLength = std::min(CHUNK_SIZE, m_blobData->Size - m_position);
                auto outputBuffer = Napi::ArrayBuffer::New(env, outputLength);
                copyInto(static_cast<std::byte*>(outputBuffer.Data()), outputLength);
                auto output = Napi::Uint8Array::New(env, outputLength, outputBuffer, 0);
                controller.Get("enqueue").As<Napi::Function>().Call(controller, {output});
            }

            if (m_position == m_blobData->Size)
            {
                controller.Get("close").As<Napi::Function>().Call(controller, {});
                m_blobData.reset();
            }

            return env.Undefined();
        }

        Napi::Value Cancel(const Napi::CallbackInfo& info)
        {
            m_blobData.reset();
            return info.Env().Undefined();
        }

        std::shared_ptr<const Storage> m_blobData;
        size_t m_position{};
        size_t m_segmentIndex{};
        size_t m_segmentOffset{};
    };

    void Blob::Initialize(Napi::Env env)
    {
        if (env.Global().Get(JS_BLOB_CONSTRUCTOR_NAME).IsUndefined())
        {
            Napi::Function func = DefineClass(
                env,
                JS_BLOB_CONSTRUCTOR_NAME,
                {
                    InstanceAccessor("size", &Blob::GetSize, nullptr),
                    InstanceAccessor("type", &Blob::GetType, nullptr),
                    InstanceMethod("text", &Blob::Text),
                    InstanceMethod("arrayBuffer", &Blob::ArrayBuffer),
                    InstanceMethod("bytes", &Blob::Bytes),
                    InstanceMethod("slice", &Blob::Slice),
                    InstanceMethod("stream", &Blob::Stream),
                });

            auto descriptor = Napi::Object::New(env);
            descriptor.Set("configurable", true);
            descriptor.Set("value", JS_BLOB_CONSTRUCTOR_NAME);
            env.Global().Get("Object").As<Napi::Object>().Get("defineProperty").As<Napi::Function>().Call(env.Global().Get("Object"), {func.Get("prototype"), Napi::Symbol::WellKnown(env, "toStringTag"), descriptor});

            // Underlying stream sources are native ObjectWrap instances so
            // pull/cancel live on one shared prototype. Creating two native
            // functions for every Blob.stream() caused avoidable JSC Function
            // structure transitions and exposed a WebKitGTK LeakSanitizer
            // allocation during Worker teardown.
            auto sourceDescriptor = Napi::Object::New(env);
            sourceDescriptor.Set("value", StreamSource::Define(env));
            env.Global().Get("Object").As<Napi::Object>().Get("defineProperty").As<Napi::Function>().Call(
                env.Global().Get("Object"),
                {func, Napi::String::New(env, "__jsRuntimeHostBlobStreamSource"), sourceDescriptor});
            // TryGetData identifies our instances through this hidden, non-writable, non-configurable
            // global, so a script that later replaces the global `Blob` (a test shim, another
            // polyfill) cannot make a foreign object look like one of ours. A plain global rather
            // than the JsRuntime native object: worker environments have no JsRuntime.
            auto constructorDescriptor = Napi::Object::New(env);
            constructorDescriptor.Set("value", func);
            env.Global().Get("Object").As<Napi::Object>().Get("defineProperty").As<Napi::Function>().Call(
                env.Global().Get("Object"),
                {env.Global(), Napi::String::New(env, JS_BLOB_CONSTRUCTOR_KEY), constructorDescriptor});
            env.Global().Set(JS_BLOB_CONSTRUCTOR_NAME, func);
        }
    }

    Blob::Blob(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<Blob>(info)
    {
        auto env = Env();
        auto data = std::make_shared<Storage>();
        std::vector<size_t> stringSegmentIndices;
        bool convertLineEndings{};

        if (info.Length() > 0 && !info[0].IsUndefined())
        {
            if (!info[0].IsObject() && !info[0].IsFunction())
            {
                throw Napi::TypeError::New(env, "Blob parts must be an iterable object.");
            }

            const auto blobParts = info[0].As<Napi::Object>();
            if (blobParts.IsArray())
            {
                data->Segments.reserve(blobParts.As<Napi::Array>().Length());
            }

            const auto reflect = env.Global().Get("Reflect").As<Napi::Object>();
            const auto iteratorMethod = reflect.Get("get").As<Napi::Function>().Call(
                reflect,
                {blobParts, Napi::Symbol::WellKnown(env, "iterator")});
            if (!iteratorMethod.IsFunction())
            {
                throw Napi::TypeError::New(env, "Blob parts must be iterable.");
            }

            const auto iterator = iteratorMethod.As<Napi::Function>().Call(blobParts, {}).As<Napi::Object>();
            const auto next = iterator.Get("next").As<Napi::Function>();
            try
            {
                while (true)
                {
                    const auto result = next.Call(iterator, {}).As<Napi::Object>();
                    if (result.Get("done").ToBoolean().Value())
                    {
                        break;
                    }

                    if (AppendBlobPart(*data, result.Get("value")))
                    {
                        stringSegmentIndices.emplace_back(data->Segments.size() - 1);
                    }
                }
            }
            catch (...)
            {
                try
                {
                    const auto returnMethod = iterator.Get("return");
                    if (returnMethod.IsFunction())
                    {
                        returnMethod.As<Napi::Function>().Call(iterator, {});
                    }
                }
                catch (...)
                {
                    // Preserve the exception that interrupted part conversion.
                }
                throw;
            }
        }

        if (info.Length() > 1 && !info[1].IsUndefined() && !info[1].IsNull())
        {
            if (!info[1].IsObject() && !info[1].IsFunction())
            {
                throw Napi::TypeError::New(env, "Blob options must be an object.");
            }

            const auto options = info[1].As<Napi::Object>();
            const auto endings = options.Get("endings");
            if (!endings.IsUndefined())
            {
                const auto value = endings.ToString().Utf8Value();
                if (value != "transparent" && value != "native")
                {
                    throw Napi::TypeError::New(env, "Blob endings must be 'transparent' or 'native'.");
                }
                convertLineEndings = value == "native";
            }

            const auto type = options.Get("type");
            if (!type.IsUndefined())
            {
                m_type = NormalizeType(type.ToString().Utf8Value());
            }
        }

        if (convertLineEndings)
        {
            for (const auto segmentIndex : stringSegmentIndices)
            {
                auto& segment = data->Segments[segmentIndex];
                auto value = std::string{
                    reinterpret_cast<const char*>(segment.Bytes->data() + segment.Offset),
                    segment.Length};
                value = NormalizeLineEndings(std::move(value));

                auto bytes = std::make_shared<std::vector<std::byte>>(value.size());
                if (!value.empty())
                {
                    std::memcpy(bytes->data(), value.data(), value.size());
                }
                data->Size -= segment.Length;
                segment = {std::move(bytes), 0, value.size()};
                data->Size += segment.Length;
            }
        }

        m_data = std::move(data);
    }

    Napi::Value Blob::GetSize(const Napi::CallbackInfo&)
    {
        return Napi::Value::From(Env(), m_data->Size);
    }

    Napi::Value Blob::GetType(const Napi::CallbackInfo&)
    {
        return Napi::String::From(Env(), m_type);
    }

    Napi::Value Blob::Text(const Napi::CallbackInfo&)
    {
        // NOTE: This will not check for UTF-8 validity
        std::string text(m_data->Size, '\0');
        if (!text.empty())
        {
            m_data->CopyTo(0, reinterpret_cast<std::byte*>(text.data()), text.size());
        }

        const auto deferred = Napi::Promise::Deferred::New(Env());
        deferred.Resolve(Napi::String::New(Env(), text));
        return deferred.Promise();
    }

    Napi::Value Blob::ArrayBuffer(const Napi::CallbackInfo&)
    {
        const auto deferred = Napi::Promise::Deferred::New(Env());
        deferred.Resolve(CreateArrayBuffer());
        return deferred.Promise();
    }

    Napi::Value Blob::Bytes(const Napi::CallbackInfo&)
    {
        const auto arrayBuffer = CreateArrayBuffer();
        const auto uint8Array = Napi::Uint8Array::New(Env(), m_data->Size, arrayBuffer, 0);

        const auto deferred = Napi::Promise::Deferred::New(Env());
        deferred.Resolve(uint8Array);
        return deferred.Promise();
    }

    Napi::Value Blob::Slice(const Napi::CallbackInfo& info)
    {
        const auto start = NormalizeSliceIndex(
            info.Length() > 0 && !info[0].IsUndefined() ? info[0].ToNumber().DoubleValue() : 0,
            m_data->Size);
        const auto end = NormalizeSliceIndex(
            info.Length() > 1 && !info[1].IsUndefined() ? info[1].ToNumber().DoubleValue() : static_cast<double>(m_data->Size),
            m_data->Size);
        const auto sliceEnd = std::max(start, end);

        auto sliceData = std::make_shared<Storage>();
        size_t segmentStart{};
        for (const auto& segment : m_data->Segments)
        {
            const auto segmentEnd = segmentStart + segment.Length;
            const auto overlapStart = std::max(start, segmentStart);
            const auto overlapEnd = std::min(sliceEnd, segmentEnd);
            if (overlapStart < overlapEnd)
            {
                sliceData->Append({segment.Bytes, segment.Offset + overlapStart - segmentStart, overlapEnd - overlapStart});
            }
            segmentStart = segmentEnd;
            if (segmentStart >= sliceEnd)
            {
                break;
            }
        }

        std::string contentType;
        if (info.Length() > 2 && !info[2].IsUndefined())
        {
            contentType = NormalizeType(info[2].ToString().Utf8Value());
        }

        const auto blobConstructor = Env().Global().Get("Blob").As<Napi::Function>();
        const auto result = blobConstructor.New({Napi::Array::New(Env())});
        auto resultBlob = Napi::ObjectWrap<Blob>::Unwrap(result);
        resultBlob->m_data = std::move(sliceData);
        resultBlob->m_type = std::move(contentType);
        return result;
    }

    Napi::Value Blob::Stream(const Napi::CallbackInfo& info)
    {
        auto env = info.Env();
        const auto readableStreamValue = env.Global().Get("ReadableStream");
        if (!readableStreamValue.IsFunction())
        {
            throw Napi::TypeError::New(env, "Blob.stream() requires ReadableStream to be installed.");
        }

        const auto blobConstructor = env.Global().Get("Blob").As<Napi::Object>();
        const auto sourceConstructor = blobConstructor.Get("__jsRuntimeHostBlobStreamSource").As<Napi::Function>();
        auto source = sourceConstructor.New({info.This()});
        source.Set("type", "bytes");

        return readableStreamValue.As<Napi::Function>().New({source});
    }

    bool Blob::AppendBlobPart(Storage& data, const Napi::Value& blobPart)
    {
        const std::byte* source{};
        size_t length{};
        bool isBufferSource{};

        if (blobPart.IsArrayBuffer())
        {
            const auto buffer = blobPart.As<Napi::ArrayBuffer>();
            source = static_cast<const std::byte*>(buffer.Data());
            length = buffer.ByteLength();
            isBufferSource = true;
        }
        else if (blobPart.IsTypedArray())
        {
            const auto array = blobPart.As<Napi::TypedArray>();
            const auto buffer = array.ArrayBuffer();
            const auto bufferData = static_cast<const std::byte*>(buffer.Data());
            source = bufferData == nullptr ? nullptr : bufferData + array.ByteOffset();
            length = array.ByteLength();
            isBufferSource = true;
        }
        else if (blobPart.IsDataView())
        {
            // Read the view through its JS properties rather than Napi::DataView: the JSI
            // backend's DataView wrapper is an unimplemented stub, and merely instantiating it
            // fails the UWP build (its `throw "TODO"` leaves unreachable code, C4702 as error).
            const auto view = blobPart.As<Napi::Object>();
            const auto buffer = view.Get("buffer").As<Napi::ArrayBuffer>();
            const auto bufferData = static_cast<const std::byte*>(buffer.Data());
            const auto byteOffset = static_cast<size_t>(view.Get("byteOffset").As<Napi::Number>().Int64Value());
            source = bufferData == nullptr ? nullptr : bufferData + byteOffset;
            length = static_cast<size_t>(view.Get("byteLength").As<Napi::Number>().Int64Value());
            isBufferSource = true;
        }
        else if (blobPart.IsObject())
        {
            const auto object = blobPart.As<Napi::Object>();
            const auto blobConstructor = Env().Global().Get("Blob").As<Napi::Function>();
            if (object.InstanceOf(blobConstructor))
            {
                auto nativeBlob = object;
                if (!object.Get("constructor").StrictEquals(blobConstructor))
                {
                    nativeBlob = object.Get("slice").As<Napi::Function>().Call(object, {Napi::Number::New(Env(), 0), object.Get("size")}).As<Napi::Object>();
                }

                const auto blob = Napi::ObjectWrap<Blob>::Unwrap(nativeBlob);
                data.Segments.reserve(data.Segments.size() + blob->m_data->Segments.size());
                for (const auto& segment : blob->m_data->Segments)
                {
                    data.Append(segment);
                }
                return false;
            }
        }

        if (isBufferSource)
        {
            auto bytes = std::make_shared<std::vector<std::byte>>(length);
            if (length > 0)
            {
                std::memcpy(bytes->data(), source, length);
            }
            data.Append({std::move(bytes), 0, length});
            return false;
        }

        auto value = blobPart.ToString().Utf8Value();
        auto bytes = std::make_shared<std::vector<std::byte>>(value.size());
        if (!value.empty())
        {
            std::memcpy(bytes->data(), value.data(), value.size());
        }
        data.Append({std::move(bytes), 0, value.size()});
        return !value.empty();
    }

    Napi::ArrayBuffer Blob::CreateArrayBuffer() const
    {
        auto arrayBuffer = Napi::ArrayBuffer::New(Env(), m_data->Size);
        if (m_data->Size > 0)
        {
            m_data->CopyTo(0, static_cast<std::byte*>(arrayBuffer.Data()), m_data->Size);
        }
        return arrayBuffer;
    }

    const std::shared_ptr<const std::vector<std::byte>>& Blob::Data() const
    {
        if (!m_contiguous)
        {
            const auto& segments = m_data->Segments;
            if (segments.size() == 1 && segments[0].Offset == 0 && segments[0].Length == segments[0].Bytes->size())
            {
                m_contiguous = segments[0].Bytes;
            }
            else
            {
                auto bytes = std::make_shared<std::vector<std::byte>>(m_data->Size);
                if (m_data->Size > 0)
                {
                    m_data->CopyTo(0, bytes->data(), m_data->Size);
                }
                m_contiguous = std::move(bytes);
            }
        }
        return m_contiguous;
    }

    std::string Blob::NormalizeType(std::string type)
    {
        for (auto& character : type)
        {
            const auto value = static_cast<unsigned char>(character);
            if (value < 0x20 || value > 0x7E)
            {
                return {};
            }
            if (character >= 'A' && character <= 'Z')
            {
                character = static_cast<char>(character - 'A' + 'a');
            }
        }
        return type;
    }

    std::string Blob::NormalizeLineEndings(std::string value)
    {
#ifdef _WIN32
        static constexpr auto nativeNewline = "\r\n";
#else
        static constexpr auto nativeNewline = "\n";
#endif
        size_t outputLength{};
        for (size_t index{}; index < value.size(); ++index)
        {
            if (value[index] == '\r')
            {
                if (index + 1 < value.size() && value[index + 1] == '\n')
                {
                    ++index;
                }
                outputLength += std::char_traits<char>::length(nativeNewline);
            }
            else if (value[index] == '\n')
            {
                outputLength += std::char_traits<char>::length(nativeNewline);
            }
            else
            {
                ++outputLength;
            }
        }

        std::string result;
        result.reserve(outputLength);
        for (size_t index{}; index < value.size(); ++index)
        {
            if (value[index] == '\r')
            {
                if (index + 1 < value.size() && value[index + 1] == '\n')
                {
                    ++index;
                }
                result.append(nativeNewline);
            }
            else if (value[index] == '\n')
            {
                result.append(nativeNewline);
            }
            else
            {
                result.push_back(value[index]);
            }
        }
        return result;
    }

    size_t Blob::NormalizeSliceIndex(double value, size_t size)
    {
        if (std::isnan(value))
        {
            return 0;
        }

        const auto magnitude = std::abs(value);
        const auto lower = std::floor(magnitude);
        const auto fraction = magnitude - lower;
        auto rounded = lower;
        if (fraction > 0.5 || (fraction == 0.5 && std::fmod(lower, 2.0) != 0.0))
        {
            rounded += 1.0;
        }

        if (value < 0)
        {
            return !std::isfinite(rounded) || rounded >= static_cast<double>(size)
                       ? 0
                       : size - static_cast<size_t>(rounded);
        }

        return !std::isfinite(rounded) || rounded >= static_cast<double>(size)
                   ? size
                   : static_cast<size_t>(rounded);
    }
}

namespace Babylon::Polyfills::Blob
{
    void BABYLON_API Initialize(Napi::Env env)
    {
        Internal::Blob::Initialize(env);
    }

    std::optional<BlobData> BABYLON_API TryGetData(const Napi::Object& object)
    {
        const auto env = object.Env();
        const auto global = env.Global();

        // Verify `object` is a Blob (or a subclass such as File) by walking its prototype chain and
        // comparing each link against Blob.prototype, using the JS-level Object.getPrototypeOf.
        //
        // We use Object.getPrototypeOf rather than the raw napi_get_prototype C API because the
        // latter is not exposed by every adapter (e.g. JSI) and, on JSC, returns the raw
        // [[Prototype]] which differs from the JS-visible prototype of a DefineClass instance.
        // This keeps the check portable across QuickJS, V8, JavaScriptCore, Chakra and JSI, and it
        // also accepts Blob subclasses (e.g. File). We deliberately avoid napi_instanceof, whose
        // node-addon-api wrapper requires a Napi::Function.
        // Our constructor is the one Initialize pinned under the hidden key, not the global `Blob`
        // binding: a script may have replaced that with its own class whose instances wrap nothing.
        const auto blobConstructor = global.Get(JS_BLOB_CONSTRUCTOR_KEY);
        if (!blobConstructor.IsFunction())
        {
            return std::nullopt;
        }

        const auto blobPrototype = blobConstructor.As<Napi::Object>().Get("prototype");
        if (!blobPrototype.IsObject())
        {
            return std::nullopt;
        }

        const auto objectConstructor = global.Get("Object");
        if (!objectConstructor.IsObject())
        {
            return std::nullopt;
        }

        const auto getPrototypeOf = objectConstructor.As<Napi::Object>().Get("getPrototypeOf");
        if (!getPrototypeOf.IsFunction())
        {
            return std::nullopt;
        }

        const auto getPrototypeOfFn = getPrototypeOf.As<Napi::Function>();

        bool isBlob = false;
        Napi::Value current = getPrototypeOfFn.Call({object});
        while (current.IsObject())
        {
            if (current.StrictEquals(blobPrototype))
            {
                isBlob = true;
                break;
            }
            current = getPrototypeOfFn.Call({current});
        }

        if (!isBlob)
        {
            return std::nullopt;
        }

        const auto* blob = Internal::Blob::Unwrap(object);
        if (blob == nullptr)
        {
            return std::nullopt;
        }
        return BlobData{blob->Data(), blob->Type()};
    }
}
