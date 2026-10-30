#pragma once

#include <UrlLib/UrlLib.h>
#include <basen.hpp>

#include <algorithm>
#include <stdexcept>
#include <string_view>

namespace UrlLib::Detail
{
    inline char AsciiLower(char value)
    {
        return value >= 'A' && value <= 'Z' ? static_cast<char>(value + ('a' - 'A')) : value;
    }

    inline bool IsAsciiWhitespace(char value)
    {
        return value == ' ' || value == '\t' || value == '\n' || value == '\r' || value == '\f';
    }

    inline std::string_view TrimAsciiWhitespace(std::string_view value)
    {
        while (!value.empty() && IsAsciiWhitespace(value.front()))
        {
            value.remove_prefix(1);
        }
        while (!value.empty() && IsAsciiWhitespace(value.back()))
        {
            value.remove_suffix(1);
        }
        return value;
    }

    inline int HexDigit(char value)
    {
        if (value >= '0' && value <= '9')
        {
            return value - '0';
        }
        value = AsciiLower(value);
        return value >= 'a' && value <= 'f' ? value - 'a' + 10 : -1;
    }

    template<typename Container>
    inline void PercentDecodeBytes(std::string_view value, Container& bytes)
    {
        bytes.reserve(value.size());
        for (size_t index = 0; index < value.size(); ++index)
        {
            if (value[index] == '%' && index + 2 < value.size())
            {
                const int high = HexDigit(value[index + 1]);
                const int low = HexDigit(value[index + 2]);
                if (high >= 0 && low >= 0)
                {
                    bytes.push_back(static_cast<typename Container::value_type>((high << 4) | low));
                    index += 2;
                    continue;
                }
            }
            // URL percent decoding preserves incomplete/non-hex escapes and does not map '+' to space.
            bytes.push_back(static_cast<typename Container::value_type>(static_cast<unsigned char>(value[index])));
        }
    }

    inline bool IsMimeToken(char value)
    {
        return (value >= 'a' && value <= 'z') || (value >= 'A' && value <= 'Z') ||
            (value >= '0' && value <= '9') ||
            std::string_view{"!#$%&'*+-.^_`|~"}.find(value) != std::string_view::npos;
    }

    inline std::string DataContentType(std::string_view metadata)
    {
        if (metadata.empty())
        {
            return "text/plain;charset=US-ASCII";
        }
        std::string type{metadata.front() == ';' ? "text/plain" : ""};
        type.append(metadata);
        const auto separator = type.find(';');
        const std::string_view essence{type.data(), separator == std::string::npos ? type.size() : separator};
        const auto slash = essence.find('/');
        if (slash == std::string_view::npos || slash == 0 || slash + 1 == essence.size() ||
            !std::all_of(essence.begin(), essence.begin() + slash, IsMimeToken) ||
            !std::all_of(essence.begin() + slash + 1, essence.end(), IsMimeToken))
        {
            throw std::invalid_argument{"data URL requires a valid MIME type"};
        }
        std::transform(type.begin(), type.begin() + essence.size(), type.begin(), AsciiLower);

        size_t index = essence.size();
        while (index < type.size())
        {
            ++index; // semicolon
            while (index < type.size() && type[index] == ' ')
            {
                ++index;
            }
            const size_t name = index;
            while (index < type.size() && IsMimeToken(type[index]))
            {
                type[index] = AsciiLower(type[index]);
                ++index;
            }
            if (index == name || index == type.size() || type[index++] != '=')
            {
                throw std::invalid_argument{"data URL requires name=value MIME parameters"};
            }
            if (index < type.size() && type[index] == '"')
            {
                ++index;
                bool closed = false;
                while (index < type.size())
                {
                    auto value = static_cast<unsigned char>(type[index++]);
                    if (value == '"')
                    {
                        closed = true;
                        break;
                    }
                    if (value == '\\' && index < type.size())
                    {
                        value = static_cast<unsigned char>(type[index++]);
                    }
                    if (value < 0x20 || value >= 0x7f)
                    {
                        throw std::invalid_argument{"data URL MIME parameter contains a non-ASCII/control byte"};
                    }
                }
                if (!closed)
                {
                    throw std::invalid_argument{"data URL MIME parameter has an unterminated quote"};
                }
            }
            else
            {
                const size_t start = index;
                while (index < type.size() && IsMimeToken(type[index]))
                {
                    ++index;
                }
                if (index == start)
                {
                    throw std::invalid_argument{"data URL MIME parameter has an empty/invalid value"};
                }
            }
            if (index < type.size() && type[index] != ';')
            {
                throw std::invalid_argument{"data URL MIME parameter has an invalid delimiter"};
            }
        }
        return type;
    }

    inline UrlSchemeResolverResult ResolveDataUrl(const std::string& url)
    {
        const std::string_view source = std::string_view{url}.substr(0, url.find('#'));
        const auto comma = source.find(',', 5);
        if (comma == std::string_view::npos)
        {
            throw std::invalid_argument{"data URL is missing its comma separator"};
        }
        auto metadata = TrimAsciiWhitespace(source.substr(5, comma - 5));
        bool base64 = false;
        if (metadata.size() >= 6)
        {
            auto marker = metadata.substr(metadata.size() - 6);
            if (std::equal(marker.begin(), marker.end(), "base64",
                [](char left, char right) { return AsciiLower(left) == right; }))
            {
                const auto prefix = TrimAsciiWhitespace(metadata.substr(0, metadata.size() - 6));
                if (!prefix.empty() && prefix.back() == ';')
                {
                    base64 = true;
                    metadata = prefix.substr(0, prefix.size() - 1);
                }
            }
        }

        UrlSchemeResolverResult result;
        result.contentType = DataContentType(metadata);
        const auto payload = source.substr(comma + 1);
        std::vector<std::byte> body;
        if (base64)
        {
            std::string normalized;
            std::string_view encoded = payload;
            if (payload.find('%') != std::string_view::npos ||
                std::any_of(payload.begin(), payload.end(), IsAsciiWhitespace))
            {
                PercentDecodeBytes(payload, normalized);
                normalized.erase(std::remove_if(normalized.begin(), normalized.end(), IsAsciiWhitespace), normalized.end());
                encoded = normalized;
            }
            if (encoded.size() % 4 == 0)
            {
                for (int padding = 0; padding < 2 && !encoded.empty() && encoded.back() == '='; ++padding)
                {
                    encoded.remove_suffix(1);
                }
            }
            if (encoded.size() % 4 == 1 ||
                !std::all_of(encoded.begin(), encoded.end(), [](char value) {
                    return (value >= 'A' && value <= 'Z') || (value >= 'a' && value <= 'z') ||
                        (value >= '0' && value <= '9') || value == '+' || value == '/';
                }))
            {
                throw std::invalid_argument{"data URL contains invalid base64"};
            }
            // base-n skips malformed input, so validate the forgiving-base64 grammar before decoding.
            body.resize(encoded.size() / 4 * 3 + encoded.size() % 4 * 3 / 4);
            bn::decode_b64(encoded.begin(), encoded.end(), reinterpret_cast<char*>(body.data()));
        }
        else
        {
            PercentDecodeBytes(payload, body);
        }
        result.body = std::make_shared<const std::vector<std::byte>>(std::move(body));
        result.handled = true;
        result.statusCode = UrlStatusCode::Ok;
        return result;
    }
}
