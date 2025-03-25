#include <regex>
#include <sstream>

#include "Font.h"

namespace Babylon::Polyfills::Internal
{
    static auto STYLE_REGEX = std::regex(R"(^\s*(normal|italic)\s)");
    static auto WEIGHT_REGEX = std::regex(R"(^\s*(normal|bold|\d+)\s)");
    static auto SIZE_REGEX = std::regex(R"(^\s*((?:\d+(?:\.\d+)?|\.\d+)(?:[eE][+-]?\d+)?)px\s)");
    static auto FAMILY_IDENT_REGEX = std::regex(R"(^\s*((?:[\w-]|\\.)+))");
    static auto FAMILY_STRING_REGEX = std::regex(R"(^\s*(["'])((?:[^\\]|\\.)*?)\1)");

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
                    font.Style = FontStyle::Italic;
                }
            }
            else if (!foundWeight && std::regex_search(begin, end, match, WEIGHT_REGEX))
            {
                begin = match[0].second;
                foundWeight = true;
                if (match[1] == "bold")
                {
                    font.Weight = BOLD_WEIGHT;
                }
                else
                {
                    font.Weight = std::stoi(match[1]);
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
        font.Size = std::stof(match[1]);

        if (std::regex_search(begin, end, match, FAMILY_IDENT_REGEX))
        {
            font.Family = match[1];
        }
        else if (std::regex_search(begin, end, match, FAMILY_STRING_REGEX))
        {
            // The first capture group is used for the quotation mark (" or ')
            font.Family = match[2];
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
        if (Style == FontStyle::Italic)
        {
            stream << "italic ";
        }

        if (Weight != NORMAL_WEIGHT)
        {
            stream << Weight << " ";
        }

        stream << Size << "px \"" << Family << "\"";
        return stream.str();
    }
}
