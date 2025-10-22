#include <regex>
#include <sstream>

#include "Font.h"

namespace
{
    auto STYLE_REGEX = std::regex(R"(^\s*(normal|italic)\s)");
    auto WEIGHT_REGEX = std::regex(R"(^\s*(normal|bold|\d+)\s)");
    auto SIZE_REGEX = std::regex(R"(^\s*((?:\d+(?:\.\d+)?|\.\d+)(?:[eE][+-]?\d+)?)px\s)");
    auto FAMILY_IDENT_REGEX = std::regex(R"(^\s*((?:[\w-]|\\.)+))");
    auto FAMILY_STRING_REGEX = std::regex(R"(^\s*(["'])((?:[^\\]|\\.)*?)\1)");
}

namespace Babylon::Polyfills::Internal
{
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
                else
                {
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
