#pragma once

#include <regex>
#include <string>

class nanovg_filterstack
{
public:
    nanovg_filterstack() = default;

    static bool ValidString(const std::string& value)
    {
        static const std::regex noneRegex{R"(^\s*none\s*$)"};
        static const std::regex blurRegex{R"(blur\((\d*\.?\d+)(px|rem)?\)|blur\(\))"};
        return std::regex_match(value, noneRegex) || std::regex_match(value, blurRegex);
    }

    void ParseString(const std::string& value)
    {
        m_blurSigma = 0.0f;
        std::smatch match;
        static const std::regex blurRegex{R"(blur\((\d*\.?\d+)(px|rem)?\)|blur\(\))"};

        if (!std::regex_match(value, match, blurRegex))
        {
            return;
        }

        if (match.size() > 1 && match[1].matched)
        {
            m_blurSigma = std::stof(match[1].str());
            if (m_blurSigma < 0.0f)
            {
                m_blurSigma = 0.0f;
            }
        }
    }

    float BlurSigma() const { return m_blurSigma; }

private:
    float m_blurSigma{0.0f};
};
