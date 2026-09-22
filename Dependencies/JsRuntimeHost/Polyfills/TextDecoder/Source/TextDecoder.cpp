#include <Babylon/Polyfills/TextDecoder.h>

#include <napi/napi.h>
#include <cstring>
#include <string>

namespace
{
    // Normalize an encoding label per the WHATWG Encoding Standard "get an encoding"
    // algorithm: strip leading/trailing ASCII whitespace and ASCII-lowercase the result.
    std::string NormalizeEncodingLabel(const std::string& encoding)
    {
        const auto isAsciiWhitespace = [](char c) {
            return c == '\t' || c == '\n' || c == '\f' || c == '\r' || c == ' ';
        };

        size_t begin = 0;
        size_t end = encoding.size();
        while (begin < end && isAsciiWhitespace(encoding[begin]))
        {
            ++begin;
        }
        while (end > begin && isAsciiWhitespace(encoding[end - 1]))
        {
            --end;
        }

        std::string label = encoding.substr(begin, end - begin);
        for (auto& c : label)
        {
            if (c >= 'A' && c <= 'Z')
            {
                c = static_cast<char>(c - 'A' + 'a');
            }
        }
        return label;
    }

    class TextDecoder final : public Napi::ObjectWrap<TextDecoder>
    {
        // Only the encodings the runtime actually needs. UTF-16 shows up in Emscripten output
        // (UTF16ToString creates a `new TextDecoder('utf-16le')` at module scope), so refusing
        // it makes whole WebAssembly modules unloadable.
        enum class Encoding
        {
            Utf8,
            Utf16LittleEndian,
            Utf16BigEndian
        };

    public:
        static void Initialize(Napi::Env env)
        {
            Napi::HandleScope scope{env};

            static constexpr auto JS_TEXTDECODER_CONSTRUCTOR_NAME = "TextDecoder";
            if (env.Global().Get(JS_TEXTDECODER_CONSTRUCTOR_NAME).IsUndefined())
            {
                Napi::Function func = DefineClass(
                    env,
                    JS_TEXTDECODER_CONSTRUCTOR_NAME,
                    {
                        InstanceMethod("decode", &TextDecoder::Decode),
                    });

                env.Global().Set(JS_TEXTDECODER_CONSTRUCTOR_NAME, func);
            }
        }

        explicit TextDecoder(const Napi::CallbackInfo& info)
            : Napi::ObjectWrap<TextDecoder>{info}
        {
            if (info.Length() > 0 && info[0].IsString())
            {
                auto encoding = info[0].As<Napi::String>().Utf8Value();

                // Several labels (e.g. "utf8", "unicode-1-1-utf-8") all map to UTF-8 after
                // normalization; callers such as the glTF/Draco loader pass "utf8".
                const std::string label = NormalizeEncodingLabel(encoding);
                if (label == "utf-8" ||
                    label == "utf8" ||
                    label == "unicode-1-1-utf-8" ||
                    label == "unicode11utf8" ||
                    label == "unicode20utf8" ||
                    label == "x-unicode20utf8")
                {
                    m_encoding = Encoding::Utf8;
                }
                else if (label == "utf-16" ||
                         label == "utf-16le" ||
                         label == "ucs-2" ||
                         label == "unicode" ||
                         label == "unicodefeff" ||
                         label == "csunicode" ||
                         label == "iso-10646-ucs-2")
                {
                    m_encoding = Encoding::Utf16LittleEndian;
                }
                else if (label == "utf-16be" ||
                         label == "unicodefffe")
                {
                    m_encoding = Encoding::Utf16BigEndian;
                }
                else
                {
                    throw Napi::Error::New(Env(), "TextDecoder: unsupported encoding '" + encoding + "', only UTF-8 and UTF-16 are supported");
                }
            }
        }

    private:
        Encoding m_encoding{Encoding::Utf8};

        Napi::Value DecodeUtf16(Napi::Env env, const std::string& data) const
        {
            // Trailing odd byte is dropped: the WHATWG decoder would emit U+FFFD for it, but
            // every producer we care about hands over whole code units.
            const size_t unitCount = data.size() / 2;
            std::u16string units(unitCount, u'\0');
            for (size_t index = 0; index < unitCount; ++index)
            {
                const auto first = static_cast<unsigned char>(data[index * 2]);
                const auto second = static_cast<unsigned char>(data[index * 2 + 1]);
                units[index] = m_encoding == Encoding::Utf16LittleEndian
                    ? static_cast<char16_t>(first | (second << 8))
                    : static_cast<char16_t>(second | (first << 8));
            }

            if (!units.empty() && units.front() == u'\uFEFF')
            {
                units.erase(0, 1);
            }

            return Napi::String::New(env, units);
        }

        Napi::Value Decode(const Napi::CallbackInfo& info)
        {
            if (info.Length() < 1 || info[0].IsUndefined())
            {
                return Napi::String::New(info.Env(), "");
            }

            std::string data;

            if (info[0].IsTypedArray())
            {
                auto typedArray = info[0].As<Napi::TypedArray>();
                auto arrayBuffer = typedArray.ArrayBuffer();
                auto byteOffset = typedArray.ByteOffset();
                auto byteLength = typedArray.ByteLength();
                data.resize(byteLength);
                if (byteLength > 0)
                {
                    std::memcpy(data.data(), static_cast<uint8_t*>(arrayBuffer.Data()) + byteOffset, byteLength);
                }
            }
            else if (info[0].IsArrayBuffer())
            {
                auto arrayBuffer = info[0].As<Napi::ArrayBuffer>();
                auto byteLength = arrayBuffer.ByteLength();
                data.resize(byteLength);
                if (byteLength > 0)
                {
                    std::memcpy(data.data(), arrayBuffer.Data(), byteLength);
                }
            }
            else
            {
                throw Napi::TypeError::New(Env(), "TextDecoder.decode: input must be a BufferSource (ArrayBuffer or TypedArray)");
            }

            return m_encoding == Encoding::Utf8 ? Napi::String::New(info.Env(), data) : DecodeUtf16(info.Env(), data);
        }
    };
}

namespace Babylon::Polyfills::TextDecoder
{
    void BABYLON_API Initialize(Napi::Env env)
    {
        ::TextDecoder::Initialize(env);
    }
}
