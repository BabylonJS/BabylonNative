#pragma once
#include <algorithm>
#include <string>
#include <unordered_map>
#include "nanovg/nanovg.h"

namespace Babylon::Polyfills::Internal
{
    inline NVGlineCap StringToLineCap(Napi::Env, const std::string& lineCapString)
    {
        std::string str = lineCapString;
        std::transform(str.begin(), str.end(), str.begin(),
            [](unsigned char c) { return std::tolower(c); });

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
        std::transform(str.begin(), str.end(), str.begin(),
            [](unsigned char c) { return std::tolower(c); });

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
