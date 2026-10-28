#pragma once
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <regex>
#include <string>
#include "CanvasConstants.h"
#include "nanovg/nanovg.h"

namespace Babylon::Polyfills::Internal
{
    namespace
    {
        const NVGcolor TRANSPARENT_BLACK = nvgRGBA(0, 0, 0, 0);
        constexpr size_t SHORT_RGB_HEX_LENGTH = 4;
        constexpr size_t SHORT_RGBA_HEX_LENGTH = 5;
        constexpr size_t RGB_HEX_LENGTH = 7;
        constexpr size_t RGBA_HEX_LENGTH = 9;
        constexpr int HEX_NIBBLE_SHIFT = 4;
        constexpr size_t FIRST_COMPONENT_CAPTURE_INDEX = 1;
        constexpr size_t SECOND_COMPONENT_CAPTURE_INDEX = 3;
        constexpr size_t THIRD_COMPONENT_CAPTURE_INDEX = 5;
        constexpr size_t ALPHA_CAPTURE_INDEX = 7;
        constexpr size_t UNIT_CAPTURE_OFFSET = 1;
        constexpr float PERCENT_SCALE = 100.f;
        constexpr float DEGREES_PER_TURN = 360.f;
    }
    static NVGcolor StringToColor(Napi::Env env, const std::string& colorString)
    {
        std::string str = colorString;
        std::transform(str.begin(), str.end(), str.begin(),
            [](unsigned char c) { return std::tolower(c); });

        static const std::unordered_map<std::string, uint32_t> webColors = {
            {"aliceblue", 0xf0f8ff},
            {"antiquewhite", 0xfaebd7},
            {"aqua", 0x00ffff},
            {"aquamarine", 0x7fffd4},
            {"azure", 0xf0ffff},
            {"beige", 0xf5f5dc},
            {"bisque", 0xffe4c4},
            {"black", 0x000000},
            {"blanchedalmond", 0xffebcd},
            {"blue", 0x0000ff},
            {"blueviolet", 0x8a2be2},
            {"brown", 0xa52a2a},
            {"burlywood", 0xdeb887},
            {"cadetblue", 0x5f9ea0},
            {"chartreuse", 0x7fff00},
            {"chocolate", 0xd2691e},
            {"coral", 0xff7f50},
            {"cornflowerblue", 0x6495ed},
            {"cornsilk", 0xfff8dc},
            {"crimson", 0xdc143c},
            {"cyan", 0x00ffff},
            {"darkblue", 0x00008b},
            {"darkcyan", 0x008b8b},
            {"darkgoldenrod", 0xb8860b},
            {"darkgray", 0xa9a9a9},
            {"darkgrey", 0xa9a9a9},
            {"darkgreen", 0x006400},
            {"darkkhaki", 0xbdb76b},
            {"darkmagenta", 0x8b008b},
            {"darkolivegreen", 0x556b2f},
            {"darkorange", 0xff8c00},
            {"darkorchid", 0x9932cc},
            {"darkred", 0x8b0000},
            {"darksalmon", 0xe9967a},
            {"darkseagreen", 0x8fbc8f},
            {"darkslateblue", 0x483d8b},
            {"darkslategray", 0x2f4f4f},
            {"darkslategrey", 0x2f4f4f},
            {"darkturquoise", 0x00ced1},
            {"darkviolet", 0x9400d3},
            {"deeppink", 0xff1493},
            {"deepskyblue", 0x00bfff},
            {"dimgray", 0x696969},
            {"dimgrey", 0x696969},
            {"dodgerblue", 0x1e90ff},
            {"firebrick", 0xb22222},
            {"floralwhite", 0xfffaf0},
            {"forestgreen", 0x228b22},
            {"fuchsia", 0xff00ff},
            {"gainsboro", 0xdcdcdc},
            {"ghostwhite", 0xf8f8ff},
            {"gold", 0xffd700},
            {"goldenrod", 0xdaa520},
            {"gray", 0x808080},
            {"grey", 0x808080},
            {"green", 0x008000},
            {"greenyellow", 0xadff2f},
            {"honeydew", 0xf0fff0},
            {"hotpink", 0xff69b4},
            {"indianred", 0xcd5c5c},
            {"indigo", 0x4b0082},
            {"ivory", 0xfffff0},
            {"khaki", 0xf0e68c},
            {"lavender", 0xe6e6fa},
            {"lavenderblush", 0xfff0f5},
            {"lawngreen", 0x7cfc00},
            {"lemonchiffon", 0xfffacd},
            {"lightblue", 0xadd8e6},
            {"lightcoral", 0xf08080},
            {"lightcyan", 0xe0ffff},
            {"lightgoldenrodyellow", 0xfafad2},
            {"lightgray", 0xd3d3d3},
            {"lightgrey", 0xd3d3d3},
            {"lightgreen", 0x90ee90},
            {"lightpink", 0xffb6c1},
            {"lightsalmon", 0xffa07a},
            {"lightseagreen", 0x20b2aa},
            {"lightskyblue", 0x87cefa},
            {"lightslategray", 0x778899},
            {"lightslategrey", 0x778899},
            {"lightsteelblue", 0xb0c4de},
            {"lightyellow", 0xffffe0},
            {"lime", 0x00ff00},
            {"limegreen", 0x32cd32},
            {"linen", 0xfaf0e6},
            {"magenta", 0xff00ff},
            {"maroon", 0x800000},
            {"mediumaquamarine", 0x66cdaa},
            {"mediumblue", 0x0000cd},
            {"mediumorchid", 0xba55d3},
            {"mediumpurple", 0x9370db},
            {"mediumseagreen", 0x3cb371},
            {"mediumslateblue", 0x7b68ee},
            {"mediumspringgreen", 0x00fa9a},
            {"mediumturquoise", 0x48d1cc},
            {"mediumvioletred", 0xc71585},
            {"midnightblue", 0x191970},
            {"mintcream", 0xf5fffa},
            {"mistyrose", 0xffe4e1},
            {"moccasin", 0xffe4b5},
            {"navajowhite", 0xffdead},
            {"navy", 0x000080},
            {"oldlace", 0xfdf5e6},
            {"olive", 0x808000},
            {"olivedrab", 0x6b8e23},
            {"orange", 0xffa500},
            {"orangered", 0xff4500},
            {"orchid", 0xda70d6},
            {"palegoldenrod", 0xeee8aa},
            {"palegreen", 0x98fb98},
            {"paleturquoise", 0xafeeee},
            {"palevioletred", 0xdb7093},
            {"papayawhip", 0xffefd5},
            {"peachpuff", 0xffdab9},
            {"peru", 0xcd853f},
            {"pink", 0xffc0cb},
            {"plum", 0xdda0dd},
            {"powderblue", 0xb0e0e6},
            {"purple", 0x800080},
            {"red", 0xff0000},
            {"rosybrown", 0xbc8f8f},
            {"royalblue", 0x4169e1},
            {"saddlebrown", 0x8b4513},
            {"salmon", 0xfa8072},
            {"sandybrown", 0xf4a460},
            {"seagreen", 0x2e8b57},
            {"seashell", 0xfff5ee},
            {"sienna", 0xa0522d},
            {"silver", 0xc0c0c0},
            {"skyblue", 0x87ceeb},
            {"slateblue", 0x6a5acd},
            {"slategray", 0x708090},
            {"slategrey", 0x708090},
            {"snow", 0xfffafa},
            {"springgreen", 0x00ff7f},
            {"steelblue", 0x4682b4},
            {"tan", 0xd2b48c},
            {"teal", 0x008080},
            {"thistle", 0xd8bfd8},
            {"tomato", 0xff6347},
            {"turquoise", 0x40e0d0},
            {"violet", 0xee82ee},
            {"wheat", 0xf5deb3},
            {"white", 0xffffff},
            {"whitesmoke", 0xf5f5f5},
            {"yellow", 0xffff00},
            {"yellowgreen", 0x9acd32}};

        if (str == "transparent" || !str.length())
        {
            return nvgRGBA(0, 0, 0, 0);
        }

        if (str[0] == '#')
        {
            unsigned int components[CanvasConstants::RGBA_CHANNEL_COUNT]{
                CanvasConstants::COLOR_CHANNEL_MAX,
                CanvasConstants::COLOR_CHANNEL_MAX,
                CanvasConstants::COLOR_CHANNEL_MAX,
                CanvasConstants::COLOR_CHANNEL_MAX};
            int count{};
            int bitShift{HEX_NIBBLE_SHIFT};
            switch (str.length())
            {
                case SHORT_RGB_HEX_LENGTH:
                    count = sscanf(str.c_str(), "#%1x%1x%1x", &components[0], &components[1], &components[2]);
                    break;
                case SHORT_RGBA_HEX_LENGTH:
                    count = sscanf(str.c_str(), "#%1x%1x%1x%1x", &components[0], &components[1], &components[2], &components[3]);
                    break;
                case RGB_HEX_LENGTH:
                    count = sscanf(str.c_str(), "#%02x%02x%02x", &components[0], &components[1], &components[2]);
                    bitShift = 0;
                    break;
                case RGBA_HEX_LENGTH:
                    count = sscanf(str.c_str(), "#%02x%02x%02x%02x", &components[0], &components[1], &components[2], &components[3]);
                    bitShift = 0;
                    break;
                default:
                    throw Napi::Error::New(env, std::string{"Unable to parse color : "} + str);
            }

            if (bitShift)
            {
                for (int i = 0; i < count; i++)
                {
                    components[i] += components[i] << bitShift;
                }
            }
            return nvgRGBA(components[0], components[1], components[2], components[3]);
        }
        else
        {
            auto iter = webColors.find(str);
            if (iter != webColors.end())
            {
                uint32_t color = iter->second;
                return nvgRGBA(
                    color >> CanvasConstants::BLUE_CHANNEL_SHIFT,
                    (color >> CanvasConstants::GREEN_CHANNEL_SHIFT) & CanvasConstants::COLOR_CHANNEL_MASK,
                    color & CanvasConstants::COLOR_CHANNEL_MASK,
                    CanvasConstants::COLOR_CHANNEL_MAX);
            }

            // CSS color components accept fractional values and optional percentages.
            static const std::string number{R"((-?(?:\d+(?:\.\d*)?|\.\d+))(%?))"};
            static const std::string separator{R"((?:\s*,\s*|\s+))"};
            static const std::string alphaSeparator{R"((?:\s*,\s*|\s*/\s*))"};
            static const std::string components{
                R"(\(\s*)" + number + separator + number + separator + number +
                "(?:" + alphaSeparator + number + R"()?\s*\))"};

            // strtof avoids exceptions for unbounded numeric input. Fold infinities back to
            // finite values so clamping and hue wrapping remain defined.
            const auto toFloat = [](const std::ssub_match& match) {
                const std::string text{match.str()};
                const float value = std::strtof(text.c_str(), nullptr);
                if (std::isfinite(value))
                {
                    return value;
                }
                return value < 0.0f ? std::numeric_limits<float>::lowest() : std::numeric_limits<float>::max();
            };
            const auto channel = [&toFloat](const std::smatch& match, size_t index) {
                const float value = toFloat(match[index]);
                const float scaled = match[index + UNIT_CAPTURE_OFFSET].matched && match[index + UNIT_CAPTURE_OFFSET].length() > 0
                    ? value * CanvasConstants::COLOR_CHANNEL_MAX / PERCENT_SCALE
                    : value;
                return static_cast<unsigned char>(std::clamp(scaled, 0.0f, static_cast<float>(CanvasConstants::COLOR_CHANNEL_MAX)) + 0.5f);
            };
            const auto alpha = [&toFloat](const std::smatch& match, size_t index) {
                if (!match[index].matched)
                {
                    return static_cast<unsigned char>(CanvasConstants::COLOR_CHANNEL_MAX);
                }
                const float value = toFloat(match[index]);
                const float normalized = match[index + UNIT_CAPTURE_OFFSET].matched && match[index + UNIT_CAPTURE_OFFSET].length() > 0
                    ? value / PERCENT_SCALE
                    : value;
                return static_cast<unsigned char>(std::clamp(normalized, 0.0f, 1.0f) * CanvasConstants::COLOR_CHANNEL_MAX + 0.5f);
            };

            static const std::regex rgbRegex("rgba?" + components);
            std::smatch rgbMatch;
            if (std::regex_match(str, rgbMatch, rgbRegex))
            {
                return nvgRGBA(
                    channel(rgbMatch, FIRST_COMPONENT_CAPTURE_INDEX),
                    channel(rgbMatch, SECOND_COMPONENT_CAPTURE_INDEX),
                    channel(rgbMatch, THIRD_COMPONENT_CAPTURE_INDEX),
                    alpha(rgbMatch, ALPHA_CAPTURE_INDEX));
            }

            static const std::regex hslRegex("hsla?" + components);
            std::smatch hslMatch;
            if (std::regex_match(str, hslMatch, hslRegex))
            {
                // Hue is an angle in degrees; nvgHSLA wraps it internally but
                // expects turns. Saturation and lightness are percentages
                // whether or not the '%' is spelled out.
                const float hue = toFloat(hslMatch[FIRST_COMPONENT_CAPTURE_INDEX]) / DEGREES_PER_TURN;
                const float saturation = std::clamp(toFloat(hslMatch[SECOND_COMPONENT_CAPTURE_INDEX]) / PERCENT_SCALE, 0.0f, 1.0f);
                const float lightness = std::clamp(toFloat(hslMatch[THIRD_COMPONENT_CAPTURE_INDEX]) / PERCENT_SCALE, 0.0f, 1.0f);
                return nvgHSLA(hue, saturation, lightness, alpha(hslMatch, ALPHA_CAPTURE_INDEX));
            }
        }
        throw Napi::Error::New(env, std::string{"Unable to parse color: "} + str);
    }
} //namespace
