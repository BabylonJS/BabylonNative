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
    public:
        operator std::string() const;
        static std::optional<Font> Parse(const std::string& fontString);

        float GetSize() const { return size; }
        const std::string& GetFamiliy() const { return family; }

    private:
        static constexpr int NORMAL_WEIGHT = 400;
        static constexpr int BOLD_WEIGHT = 700;

        FontStyle style{FontStyle::Normal};
        int weight{NORMAL_WEIGHT};
        float size{10};
        std::string family{"sans-serif"};
    };
}
