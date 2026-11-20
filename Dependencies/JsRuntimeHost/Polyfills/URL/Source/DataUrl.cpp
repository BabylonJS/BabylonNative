// Lifted from the fetch polyfill so every UrlLib consumer (XMLHttpRequest, texture and asset
// loaders, fetch) shares one data: URL decoder; see Polyfills/URL/Source/URL.cpp for the scheme
// resolver that registers it.
#include <Babylon/Polyfills/DataUrl.h>

#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <utility>

namespace Babylon::Polyfills::DataUrl
{
    namespace
    {
        bool EqualsIgnoreCase(std::string_view a, std::string_view b)
        {
            return std::equal(a.begin(), a.end(), b.begin(), b.end(), [](unsigned char l, unsigned char r) {
                return std::tolower(l) == std::tolower(r);
            });
        }

        bool IsAsciiWhitespace(uint8_t value)
        {
            return value == 0x09 || value == 0x0A || value == 0x0C || value == 0x0D || value == 0x20;
        }

        void TrimAsciiWhitespace(std::string& value)
        {
            const auto first = std::find_if_not(value.begin(), value.end(), [](unsigned char character) {
                return IsAsciiWhitespace(character);
            });
            const auto last = std::find_if_not(value.rbegin(), value.rend(), [](unsigned char character) {
                return IsAsciiWhitespace(character);
            }).base();
            value = first < last ? std::string{first, last} : std::string{};
        }

        int HexDigitValue(char value)
        {
            if (value >= '0' && value <= '9')
            {
                return value - '0';
            }
            if (value >= 'a' && value <= 'f')
            {
                return value - 'a' + 10;
            }
            if (value >= 'A' && value <= 'F')
            {
                return value - 'A' + 10;
            }
            return -1;
        }

        template<typename TCallback>
        void ForEachPercentDecodedByte(std::string_view value, TCallback&& callback)
        {
            for (size_t index = 0; index < value.size(); ++index)
            {
                if (value[index] == '%' && index + 2 < value.size())
                {
                    const int high = HexDigitValue(value[index + 1]);
                    const int low = HexDigitValue(value[index + 2]);
                    if (high >= 0 && low >= 0)
                    {
                        callback(static_cast<uint8_t>((high << 4) | low));
                        index += 2;
                        continue;
                    }
                }
                callback(static_cast<uint8_t>(value[index]));
            }
        }

        std::vector<uint8_t> PercentDecode(std::string_view value)
        {
            std::vector<uint8_t> decoded;
            decoded.reserve(value.size());
            ForEachPercentDecodedByte(value, [&decoded](uint8_t byte) {
                decoded.push_back(byte);
            });
            return decoded;
        }

        int Base64DigitValue(uint8_t value)
        {
            if (value >= 'A' && value <= 'Z')
            {
                return value - 'A';
            }
            if (value >= 'a' && value <= 'z')
            {
                return value - 'a' + 26;
            }
            if (value >= '0' && value <= '9')
            {
                return value - '0' + 52;
            }
            if (value == '+')
            {
                return 62;
            }
            if (value == '/')
            {
                return 63;
            }
            return -1;
        }

        std::vector<uint8_t> ForgivingBase64Decode(std::string_view input)
        {
            size_t digitCount{};
            size_t paddingCount{};
            bool sawPadding{};
            ForEachPercentDecodedByte(input, [&](uint8_t value) {
                if (IsAsciiWhitespace(value))
                {
                    return;
                }
                if (value == '=')
                {
                    sawPadding = true;
                    ++paddingCount;
                    return;
                }
                if (sawPadding || Base64DigitValue(value) < 0)
                {
                    throw std::runtime_error{"fetch: invalid base64 data URL"};
                }
                ++digitCount;
            });

            const auto encodedCount = digitCount + paddingCount;
            if ((paddingCount > 0 && (encodedCount % 4 != 0 || paddingCount > 2)) || digitCount % 4 == 1)
            {
                throw std::runtime_error{"fetch: invalid base64 data URL"};
            }

            std::vector<uint8_t> decoded(digitCount / 4 * 3 + digitCount % 4 * 3 / 4);
            size_t outputIndex{};
            uint32_t accumulator{};
            size_t availableBits{};
            ForEachPercentDecodedByte(input, [&](uint8_t value) {
                if (IsAsciiWhitespace(value) || value == '=')
                {
                    return;
                }
                accumulator = (accumulator << 6) | static_cast<uint32_t>(Base64DigitValue(value));
                availableBits += 6;
                if (availableBits >= 8)
                {
                    availableBits -= 8;
                    decoded[outputIndex++] = static_cast<uint8_t>(accumulator >> availableBits);
                    accumulator &= (uint32_t{1} << availableBits) - 1;
                }
            });
            return decoded;
        }
    }

    std::optional<Response> Parse(std::string_view url)
    {
        if (url.size() < 5 || !EqualsIgnoreCase(url.substr(0, 4), "data") || url[4] != ':')
        {
            return std::nullopt;
        }

        const auto comma = url.find(',', 5);
        if (comma == std::string_view::npos)
        {
            throw std::runtime_error{"fetch: malformed data URL"};
        }

        std::string mediaType{url.substr(5, comma - 5)};
        TrimAsciiWhitespace(mediaType);
        bool base64 = false;
        if (const auto semicolon = mediaType.rfind(';'); semicolon != std::string::npos)
        {
            std::string finalParameter{mediaType.substr(semicolon + 1)};
            TrimAsciiWhitespace(finalParameter);
            if (EqualsIgnoreCase(finalParameter, "base64"))
            {
                mediaType.resize(semicolon);
                TrimAsciiWhitespace(mediaType);
                base64 = true;
            }
        }
        if (mediaType.empty())
        {
            mediaType = "text/plain;charset=US-ASCII";
        }
        else if (mediaType.front() == ';')
        {
            mediaType.insert(0, "text/plain");
        }

        const auto fragment = url.find('#', comma + 1);
        const auto payload = url.substr(comma + 1, fragment == std::string_view::npos ? std::string_view::npos : fragment - comma - 1);
        auto decodedPayload = base64 ? ForgivingBase64Decode(payload) : PercentDecode(payload);

        return Response{
            std::move(mediaType),
            std::string{url.substr(0, fragment)},
            std::move(decodedPayload)};
    }
}
