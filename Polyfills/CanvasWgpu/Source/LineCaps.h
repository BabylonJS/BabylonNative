#pragma once
#include <cctype>
#include <string>
#include <unordered_map>
#include "nanovg/nanovg.h"

namespace Babylon::Polyfills::Internal
{
    // Plain loop instead of std::transform: MSVC 2022's STL leaks the int
    // return type of std::tolower through the transform template, triggering
    // C4244 (int-to-char truncation) inside <algorithm> even when the lambda
    // explicitly narrows.  The unsigned char cast is the canonical way to call
    // std::tolower on a char — tolower(int) is UB for negative values and
    // char is signed on MSVC/x86.
    inline NVGlineCap StringToLineCap(Napi::Env, const std::string& lineCapString)
    {
        std::string str = lineCapString;
        for (auto& ch : str)
        {
            ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
        }

        static const std::unordered_map<std::string, NVGlineCap> lineCaps = {
            {"butt", NVG_BUTT},
            {"round", NVG_ROUND},
            {"square", NVG_SQUARE}};

        auto iter = lineCaps.find(str);
        if (iter != lineCaps.end())
        {
            return iter->second;
        }

        return NVG_BUTT;
    }

    inline NVGlineCap StringToLineJoin(Napi::Env, const std::string& lineJoinString)
    {
        std::string str = lineJoinString;
        for (auto& ch : str)
        {
            ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
        }

        static const std::unordered_map<std::string, NVGlineCap> lineJoins = {
            {"bevel", NVG_BEVEL},
            {"round", NVG_ROUND},
            {"miter", NVG_MITER}};

        auto iter = lineJoins.find(str);
        if (iter != lineJoins.end())
        {
            return iter->second;
        }

        return NVG_MITER;
    }
}
