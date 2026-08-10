#include <regex>
#include <sstream>

#include <cstring>

#include "Font.h"

namespace
{
    auto STYLE_REGEX = std::regex(R"(^\s*(normal|italic)\s)");
    auto WEIGHT_REGEX = std::regex(R"(^\s*(normal|bold|\d+)\s)");
    auto SIZE_REGEX = std::regex(R"(^\s*((?:\d+(?:\.\d+)?|\.\d+)(?:[eE][+-]?\d+)?)px\s)");
    auto FAMILY_IDENT_REGEX = std::regex(R"(^\s*((?:[\w-]|\\.)+))");
    auto FAMILY_STRING_REGEX = std::regex(R"(^\s*(["'])((?:[^\\]|\\.)*?)\1)");

    std::string DescribeValue(const Napi::Value& value)
    {
        if (value.IsUndefined()) return "undefined";
        if (value.IsNull()) return "null";
        if (value.IsString()) return "a string";
        if (value.IsNumber()) return "a number";
        if (value.IsBoolean()) return "a boolean";
        if (value.IsArray()) return "an array";
        if (value.IsFunction()) return "a function";
        if (value.IsPromise()) return "a promise";
        if (value.IsObject()) return "an object";
        return "an unsupported value";
    }
}

namespace Babylon::Polyfills::Internal
{
    std::vector<uint8_t> GetFontDataArgument(const Napi::CallbackInfo& info, size_t index, const char* methodName)
    {
        const auto env = info.Env();

        if (info.Length() <= index)
        {
            throw Napi::TypeError::New(env, std::string{methodName} + " expects the font data as argument " +
                std::to_string(index + 1) + ", but only " + std::to_string(info.Length()) + " argument(s) were passed.");
        }

        const auto value = info[index];

        const uint8_t* data{};
        size_t byteLength{};

        if (value.IsArrayBuffer())
        {
            const auto buffer = value.As<Napi::ArrayBuffer>();
            data = static_cast<const uint8_t*>(buffer.Data());
            byteLength = buffer.ByteLength();
        }
        else if (value.IsTypedArray())
        {
            const auto view = value.As<Napi::TypedArray>();
            data = static_cast<const uint8_t*>(view.ArrayBuffer().Data()) + view.ByteOffset();
            byteLength = view.ByteLength();
        }
        else if (value.IsDataView())
        {
            // Deliberately NOT via Napi::DataView: the Chakra Node-API implementation backs
            // napi_get_dataview_info with JsGetExternalData, which only succeeds for DataViews
            // created natively through napi_create_dataview. A DataView constructed in JS fails
            // with napi_invalid_arg even though napi_is_dataview reports true. Reading the
            // standard buffer/byteOffset/byteLength properties works on every engine.
            const auto view = value.As<Napi::Object>();
            const auto bufferValue = view.Get("buffer");
            if (!bufferValue.IsArrayBuffer())
            {
                throw Napi::TypeError::New(env, std::string{methodName} + " was given a DataView whose 'buffer' is not an ArrayBuffer.");
            }

            const auto buffer = bufferValue.As<Napi::ArrayBuffer>();
            const auto byteOffset = static_cast<size_t>(view.Get("byteOffset").As<Napi::Number>().Int64Value());
            byteLength = static_cast<size_t>(view.Get("byteLength").As<Napi::Number>().Int64Value());

            if (byteOffset > buffer.ByteLength() || byteLength > buffer.ByteLength() - byteOffset)
            {
                throw Napi::RangeError::New(env, std::string{methodName} + " was given a DataView that extends past the end of its buffer.");
            }

            data = static_cast<const uint8_t*>(buffer.Data()) + byteOffset;
        }
        else
        {
            throw Napi::TypeError::New(env, std::string{methodName} + " expects the font data to be an ArrayBuffer or "
                "an ArrayBuffer view, but got " + DescribeValue(value) + ". If the font was fetched with "
                "Tools.LoadFileAsync, request it as binary -- the signature is LoadFileAsync(url, useArrayBuffer), "
                "and a text response decodes the font to an unusable string.");
        }

        if (byteLength == 0)
        {
            throw Napi::TypeError::New(env, std::string{methodName} + " was given an empty font buffer. The font file "
                "was most likely fetched as text rather than binary, or the request returned no data.");
        }

        std::vector<uint8_t> fontBuffer(byteLength);
        std::memcpy(fontBuffer.data(), data, byteLength);
        return fontBuffer;
    }

    std::optional<Font> Font::Parse(const std::string& fontString)
    {
        Font font;
        auto begin = fontString.cbegin();
        auto end = fontString.cend();
        std::smatch match;

        // The style and weight can be in any order
        bool foundStyle = false;
        bool foundWeight = false;
        while (!foundStyle || !foundWeight)
        {
            if (!foundStyle && std::regex_search(begin, end, match, STYLE_REGEX))
            {
                begin = match[0].second;
                foundStyle = true;
                if (match[1] == "italic")
                {
                    font.m_style = FontStyle::Italic;
                }
            }
            else if (!foundWeight && std::regex_search(begin, end, match, WEIGHT_REGEX))
            {
                begin = match[0].second;
                foundWeight = true;
                if (match[1] == "bold")
                {
                    font.m_weight = BOLD_WEIGHT;
                }
                else if (match[1] != "normal")
                {
                    // WEIGHT_REGEX also accepts the "normal" keyword, which is
                    // simply the default weight. Passing it to std::stoi throws
                    // std::invalid_argument, and because that is not a
                    // Napi::Error it escapes the N-API callback and terminates
                    // the process instead of surfacing as a JS exception.
                    // Babylon GUI composes exactly this string
                    // ("normal normal 18px Arial") from its default font style
                    // and weight.
                    font.m_weight = std::stoi(match[1]);
                }
            }
            else
            {
                break;
            }
        }

        if (!std::regex_search(begin, end, match, SIZE_REGEX))
        {
            return std::nullopt;
        }
        begin = match[0].second;
        font.m_size = std::stof(match[1]);

        if (std::regex_search(begin, end, match, FAMILY_IDENT_REGEX))
        {
            font.m_family = match[1];
        }
        else if (std::regex_search(begin, end, match, FAMILY_STRING_REGEX))
        {
            // The first capture group is used for the quotation mark (" or ')
            font.m_family = match[2];
        }
        else
        {
            return std::nullopt;
        }

        return font;
    }

    Font::operator std::string() const
    {
        std::ostringstream stream;
        if (m_style == FontStyle::Italic)
        {
            stream << "italic ";
        }

        if (m_weight != NORMAL_WEIGHT)
        {
            stream << m_weight << " ";
        }

        stream << m_size << "px \"" << m_family << "\"";
        return stream.str();
    }
}
