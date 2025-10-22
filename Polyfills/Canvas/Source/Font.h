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
