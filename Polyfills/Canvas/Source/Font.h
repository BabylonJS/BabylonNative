#pragma once

#include <string>
#include <optional>

namespace Babylon::Polyfills::Internal
{
    enum class FontStyle
    {
        Normal,
        Italic,
    };

    struct Font
    {
    private:
        static constexpr int NORMAL_WEIGHT = 400;
        static constexpr int BOLD_WEIGHT = 700;

    public:
        FontStyle Style = FontStyle::Normal;
        int Weight = NORMAL_WEIGHT;
        float Size = 10;
        std::string Family = "sans-serif";

        operator std::string() const;

        static std::optional<Font> Parse(const std::string& fontString);
    };
}
