#pragma once

#include <Babylon/Api.h>

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace Babylon::Polyfills::DataUrl
{
    // A decoded data: URL (RFC 2397 as processed by the WHATWG Fetch "data: URL processor").
    struct Response
    {
        std::string contentType;
        std::string url;
        std::vector<uint8_t> body;
    };

    // Returns nullopt when `url` is not a data: URL. Throws std::runtime_error when it is one but
    // is malformed (invalid forgiving-base64, etc.), so callers can surface a TypeError / network
    // error as their contract requires.
    std::optional<Response> BABYLON_API Parse(std::string_view url);
}
