#pragma once

#include <napi/napi.h>

#include <cstdint>
#include <string>
#include <optional>
#include <vector>

namespace Babylon::Polyfills::Internal
{
    // Extracts the font file bytes from the second argument of Canvas.loadTTF / loadTTFAsync.
    //
    // Accepts an ArrayBuffer or any ArrayBuffer view (typed array / DataView), matching what
    // callers actually have on hand. Anything else throws a Napi::TypeError naming the method,
    // the argument and what was received, instead of the bare "Invalid argument" that an
    // unchecked As<Napi::ArrayBuffer>() cast produces -- that message gives no clue which call
    // failed or why, and in practice it is almost always a caller that fetched the font as text
    // rather than binary.
    std::vector<uint8_t> GetFontDataArgument(const Napi::CallbackInfo& info, size_t index, const char* methodName);

    enum class FontStyle
    {
        Normal,
        Italic,
    };

    struct Font
    {
    public:
        operator std::string() const;
        static std::optional<Font> Parse(const std::string& fontString);

        float Size() const { return m_size; }
        const std::string& Familiy() const { return m_family; }

    private:
        static constexpr const int NORMAL_WEIGHT = 400;
        static constexpr const int BOLD_WEIGHT = 700;

        FontStyle m_style{FontStyle::Normal};
        int m_weight{NORMAL_WEIGHT};
        float m_size{10};
        std::string m_family{"sans-serif"};
    };
}
