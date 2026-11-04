#pragma once
#include <regex>
#include "nanovg/nanovg.h"

namespace Babylon::Polyfills::Internal
{
    static NVGlineCap StringToLineCap(Napi::Env, const std::string& lineCapString)
    {
        std::string str = lineCapString;
        std::transform(str.begin(), str.end(), str.begin(),
            [](unsigned char c) { return std::tolower(c); });

        static const std::unordered_map<std::string, NVGlineCap> lineCaps = {
            {"butt", NVG_BUTT},
            {"round", NVG_ROUND},
            {"square", NVG_SQUARE}};

        const auto iter = lineCaps.find(str);
        return iter != lineCaps.end() ? iter->second : NVG_BUTT;
    }

    static NVGlineCap StringToLineJoin(Napi::Env, const std::string& lineJoinString)
    {
        std::string str = lineJoinString;
        std::transform(str.begin(), str.end(), str.begin(),
            [](unsigned char c) { return std::tolower(c); });

        static const std::unordered_map<std::string, NVGlineCap> lineJoins = {
            {"bevel", NVG_BEVEL},
            {"round", NVG_ROUND},
            {"miter", NVG_MITER}};

        const auto iter = lineJoins.find(str);
        return iter != lineJoins.end() ? iter->second : NVG_MITER;
    }
}
